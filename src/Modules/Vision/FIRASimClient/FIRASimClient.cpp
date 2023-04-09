//
// Created by jose-cruz on 08/02/2021.
//

#include "FIRASimClient.h"
#include "Packages/Packages.h"

FIRASimClient::FIRASimClient(QThreadPool* threadPool) : VisionBase(threadPool) {
}

void FIRASimClient::buildParameters(Parameters::Handler& parameters) {
  using namespace Parameters;

  parameters["IP"] = Text(args.ip);
  parameters["Port"] = Text(args.port);
  /* Network Interface */ {
    QStringList names;
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto& interface : interfaces) {
      names += interface.name();
    }
    parameters["INet"] = ComboBox(args.inet, names);
  }
  parameters["TeamColor"] = MappedComboBox(args.isYellow, {{false, "Blue"}, {true, "Yellow"}});
  parameters["AttackingSide"] =
      MappedComboBox(args.isAttackingToRight, {{false, "Left"}, {true, "Right"}});

  parameters["DefaultFieldSize"] = ComboBox(args.defaultFieldSize,
                                            MagicEnum::values<Field::Size>(),
                                            "fills Field's missing values with default ones");

  parameters["Draw"]["Field Markings"] = CheckBox(args.drawFieldMarkings);
  parameters["Draw"]["CIninho"] = CheckBox(args.drawCIninho);
}

void FIRASimClient::connectModules(const Modules*) {
}

void FIRASimClient::init(const Modules* modules) {
  frameKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::AboveBottom);
  fieldKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Bottom);
}

void FIRASimClient::setupClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socket.reset(new QUdpSocket(this));
        if (socket->bind(QHostAddress::AnyIPv4, args.port, QUdpSocket::ShareAddress)) {
          qDebug() << "FIRASimClient::socket connected on port" << args.port;
        }
        if (socket->joinMulticastGroup(QHostAddress(args.ip),
                                       QNetworkInterface::interfaceFromName(args.inet))) {
          qDebug() << "FIRASimClient::socket connected with ip" << args.ip << "and interface"
                   << args.inet;
        }
        connect(socket.get(),
                &QUdpSocket::readyRead,
                this,
                &FIRASimClient::receiveDatagram,
                Qt::DirectConnection);
      },
      Qt::QueuedConnection);
}

void FIRASimClient::receiveDatagram() {
  std::deque<EnvironmentPacket> receivedPackets;
  while (socket->hasPendingDatagrams()) {
    QByteArray datagram(static_cast<int>(socket->pendingDatagramSize()), static_cast<char>(0));
    socket->readDatagram(datagram.data(), datagram.size());
    if (auto packet = Protobuf::fromByteArray<EnvironmentPacket>(datagram)) {
      receivedPackets.emplace_back(std::move(packet.value()));
    }
  }
  shared->packets.apply([&from = receivedPackets](std::deque<EnvironmentPacket>& to) {
    if (to.empty()) {
      to.swap(from);
    } else {
      std::move(from.begin(), from.end(), std::back_inserter(to));
      from.clear();
    }
  });
  runInParallel();
}

void FIRASimClient::update() {
  if (args.ip.updated() || args.port.updated() || args.inet.updated()) {
    setupClient();
  }
  if (args.isYellow.updated()) {
    emit sendIsYellow(args.isYellow);
    if (args.isYellow) {
      emit setInterfaceTeamColor(Color::Yellow, Color::Blue);
    } else {
      emit setInterfaceTeamColor(Color::Blue, Color::Yellow);
    }
  }
  if (args.isAttackingToRight.updated()) {
    emit setInterfaceAttackSide(args.isAttackingToRight);
  }
  shared->packets.extract_and_clear(packets);
}

void FIRASimClient::exec() {
  Vision::OutputType::Tracked trackedFrame;
  while (!packets.empty()) {
    auto environment = std::move(packets.front());
    packets.pop_front();
    if (environment.has_frame()) {
      const auto& f = environment.frame();

      Robots<Robot> blueRobots;
      for (const auto& robot : f.robots_blue()) {
        blueRobots += Robot(static_cast<int>(robot.robot_id()),
                            static_cast<real_t>(robot.orientation()),
                            static_cast<real_t>(robot.vorientation()),
                            QPointF(robot.x(), robot.y()) * M_TO_MM,
                            QPointF(robot.vx(), robot.vy()) * M_TO_MM,
                            QPointF(0, 0) * M_TO_MM); // acceleration is not received
      }

      Robots<Robot> yellowRobots;
      for (const auto& robot : f.robots_yellow()) {
        yellowRobots += Robot(static_cast<int>(robot.robot_id()),
                              static_cast<real_t>(robot.orientation()),
                              static_cast<real_t>(robot.vorientation()),
                              QPointF(robot.x(), robot.y()) * M_TO_MM,
                              QPointF(robot.vx(), robot.vy()) * M_TO_MM,
                              QPointF(0, 0) * M_TO_MM); // acceleration is not received
      }
      if (args.isYellow) {
        trackedFrame.emplace_allies(std::move(yellowRobots));
        trackedFrame.emplace_enemies(std::move(blueRobots));
      } else {
        trackedFrame.emplace_allies(std::move(blueRobots));
        trackedFrame.emplace_enemies(std::move(yellowRobots));
      }

      if (f.has_ball()) {
        trackedFrame.emplace_ball(QPointF(f.ball().x(), f.ball().y()) * M_TO_MM,
                                  QPointF(f.ball().vx(), f.ball().vy()) * M_TO_MM,
                                  QPointF(0, 0) * M_TO_MM // acceleration is not received
        );
      }
    }
    if (environment.has_field()) {
      const auto& f = environment.field();
      const auto& defaultField = Field::field(args.defaultFieldSize, args.isAttackingToRight);
      field.emplace(args.isAttackingToRight,
                    f.length() * M_TO_MM,
                    f.width() * M_TO_MM,
                    f.goal_depth() * M_TO_MM,
                    f.goal_width() * M_TO_MM,
                    defaultField.penaltyAreaDepth(),
                    defaultField.penaltyAreaWidth(),
                    defaultField.freeBallMarkOffsetX(),
                    defaultField.freeBallMarkOffsetY());
      emit sendField(field.value());
      fieldKey.draw(DrawField(field.value(), args.drawFieldMarkings, args.drawCIninho));
    }
  }
  emit sendVision(trackedFrame);
  frameKey.draw(
      [vision = trackedFrame, isYellow = args.isYellow.value()](GameVisualizerPainter2D* f) {
        DrawFrame(vision, isYellow).run(f);
      });
}

static_block {
  Factory::vision.insert<FIRASimClient>("");
};
