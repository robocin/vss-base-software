//
// Created by felipe on 16/05/2022.
//

#include "VSSVisionClient.h"
#include "Packages/Packages.h"

VSSVISIONClient::VSSVISIONClient(QThreadPool* threadPool) : VisionBase(threadPool) {
}

void VSSVISIONClient::buildParameters(Parameters::Handler& parameters) {
  using namespace Parameters;

  parameters["PacketType"]["Raw"]["IP"] = Text(args.raw.ip);
  parameters["PacketType"]["Raw"]["Port"] = Text(args.raw.port);
  parameters["PacketType"]["Tracked"]["IP"] = Text(args.tracked.ip);
  parameters["PacketType"]["Tracked"]["Port"] = Text(args.tracked.port);
  /* Network Interface */ {
    QStringList names;
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto& interface : interfaces) {
      names += interface.name();
    }
    parameters["PacketType"]["Raw"]["INet"] = ComboBox(args.raw.inet, names);
    parameters["PacketType"]["Tracked"]["INet"] = ComboBox(args.tracked.inet, names);
  }

  parameters["PacketType"] = MappedComboBox(args.isTracked, {{false, "Raw"}, {true, "Tracked"}});

  parameters["TeamColor"] = MappedComboBox(args.isYellow, {{false, "Blue"}, {true, "Yellow"}});
  parameters["AttackingSide"] =
      MappedComboBox(args.isAttackingToRight, {{false, "Left"}, {true, "Right"}});

  parameters["DefaultFieldSize"] = ComboBox(args.defaultFieldSize,
                                            MagicEnum::values<Field::Size>(),
                                            "fills Field's missing values with default ones");

  parameters["Draw"]["Field Markings"] = CheckBox(args.drawFieldMarkings);
  parameters["Draw"]["CIninho"] = CheckBox(args.drawCIninho);
}

void VSSVISIONClient::connectModules(const Modules*) {
}

void VSSVISIONClient::init(const Modules* modules) {
  frameKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::AboveBottom);
  fieldKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Bottom);
}

void VSSVISIONClient::setupRawClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socketRaw.reset(new QUdpSocket(this));
        if (socketRaw->bind(QHostAddress::AnyIPv4, args.raw.port, QUdpSocket::ShareAddress)) {
          qDebug() << "VSSVISIONClient::rawSocket connected on port" << args.raw.port;
        }
        if (socketRaw->joinMulticastGroup(QHostAddress(args.raw.ip),
                                          QNetworkInterface::interfaceFromName(args.raw.inet))) {
          qDebug() << "VSSVISIONClient::rawSocket connected with ip" << args.raw.ip
                   << "and interface" << QNetworkInterface::interfaceFromName(args.raw.inet);
        }
        connect(socketRaw.get(),
                &QUdpSocket::readyRead,
                this,
                &VSSVISIONClient::receiveRawDatagrams,
                Qt::DirectConnection); // can be directed.
      },
      Qt::QueuedConnection); // must be queued.
}

void VSSVISIONClient::setupTrackedClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socketTracked.reset(new QUdpSocket(this));
        if (socketTracked->bind(QHostAddress::AnyIPv4,
                                args.tracked.port,
                                QUdpSocket::ShareAddress)) {
          qDebug() << "VSSVISIONClient::socketTracked connected on port" << args.tracked.port;
        }
        if (socketTracked->joinMulticastGroup(
                QHostAddress(args.tracked.ip),
                QNetworkInterface::interfaceFromName(args.tracked.inet))) {
          qDebug() << "VSSVISIONClient::socketTracked connected with ip" << args.tracked.ip
                   << "and interface" << QNetworkInterface::interfaceFromName(args.tracked.inet);
        }
        connect(socketTracked.get(),
                &QUdpSocket::readyRead,
                this,
                &VSSVISIONClient::receiveTrackedDatagrams,
                Qt::DirectConnection); // can be directed.
      },
      Qt::QueuedConnection); // must be queued.
}

void VSSVISIONClient::receiveRawDatagrams() {
  std::deque<RawWrapperPacket> receivedPackets;
  while (socketRaw->hasPendingDatagrams()) {
    QByteArray datagram(static_cast<int>(socketRaw->pendingDatagramSize()), static_cast<char>(0));
    socketRaw->readDatagram(datagram.data(), datagram.size());
    if (auto packet = Protobuf::fromByteArray<RawWrapperPacket>(datagram)) {
      receivedPackets.emplace_back(std::move(packet.value()));
    }
  }
  shared->rawPackets.apply([&from = receivedPackets](std::deque<RawWrapperPacket>& to) {
    if (to.empty()) {
      to.swap(from);
    } else {
      std::move(from.begin(), from.end(), std::back_inserter(to));
      from.clear();
    }
  });
  runInParallel();
}

void VSSVISIONClient::receiveTrackedDatagrams() {
  std::optional<TrackedWrapperPacket> packet;
  while (socketTracked->hasPendingDatagrams()) {
    QByteArray datagram(static_cast<int>(socketTracked->pendingDatagramSize()),
                        static_cast<char>(0));
    socketTracked->readDatagram(datagram.data(), datagram.size());
    packet = Protobuf::fromByteArray<TrackedWrapperPacket>(datagram);
  }
  shared->trackedPacket = packet;
  runInParallel();
}

void VSSVISIONClient::update() {
  if (args.raw.ip.updated() || args.raw.port.updated() || args.raw.inet.updated()) {
    setupRawClient();
  }
  if (args.tracked.ip.updated() || args.tracked.port.updated() || args.tracked.inet.updated()) {
    setupTrackedClient();
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
  rawPackets = shared->rawPackets.get_and_clear();
  trackedPacket = shared->trackedPacket.get_optional_and_reset();
}

void VSSVISIONClient::exec() {
  auto [rawFrames, field] = parseRawPacket();
  auto trackedFrame = parseTrackedPacket();

  if (args.isTracked) {
    if (trackedFrame.has_value()) {
      emit sendVision(trackedFrame.value());
      frameKey.draw([vision = trackedFrame.value(),
                     isYellow = args.isYellow.value()](GameVisualizerPainter2D* f) {
        DrawFrame(vision, isYellow).run(f);
      });
    }
  } else {
    if (!rawFrames.empty()) {
      emit sendVision(rawFrames);
      frameKey.draw(
          [vision = rawFrames, isYellow = args.isYellow.value()](GameVisualizerPainter2D* f) {
            DrawFrame(vision, isYellow).run(f);
          });
    }
  }
  if (field) {
    emit sendField(field.value());
  }
}

VSSVISIONClient::ParsedRawPacket VSSVISIONClient::parseRawPacket() {
  QVector<RawFrame> rawFrames;
  std::optional<Field> field;

  while (!rawPackets.empty()) {
    auto packet = std::move(rawPackets.front());
    rawPackets.pop_front();

    if (packet.has_detection()) {
      const auto& detection = packet.detection();

      RawFrame rawFrame;
      rawFrame.set_cameraId(static_cast<int>(detection.camera_id()));
      rawFrame.set_number(static_cast<int>(detection.frame_number()));
      /* getting balls info */ {
        QVector<std::pair<float, RawBall>> balls;
        for (const auto& ball : detection.balls()) {
          balls += std::pair(ball.confidence(), RawBall(Point(ball.x(), ball.y())));
        }
        /* TODO: How we can do it better? (ball confidence) */ {
          std::sort(balls.begin(), balls.end(), std::greater());
          if (!balls.empty()) {
            rawFrame.set_balls(QVector<RawBall>({balls.front().second}));
          }
        }
      }
      /* getting robots info */ {
        Robots<RawRobot> yellowRobots;
        for (const auto& robot : detection.robots_yellow()) {
          yellowRobots += RawRobot(static_cast<int>(robot.robot_id()),
                                   robot.orientation(),
                                   Point(robot.x(), robot.y()));
        }

        Robots<RawRobot> blueRobots;
        for (const auto& robot : detection.robots_blue()) {
          blueRobots += RawRobot(static_cast<int>(robot.robot_id()),
                                 robot.orientation(),
                                 Point(robot.x(), robot.y()));
        }

        if (args.isYellow) {
          rawFrame.set_allies(yellowRobots);
          rawFrame.set_enemies(blueRobots);
        } else {
          rawFrame.set_allies(blueRobots);
          rawFrame.set_enemies(yellowRobots);
        }
      }

      rawFrames += rawFrame;
    }

    if (packet.has_geometry()) {
      const auto& f = packet.geometry().field();
      const auto& defaultField = Field::field(args.defaultFieldSize, args.isAttackingToRight);
      field.emplace(args.isAttackingToRight,
                    f.field_length(),
                    f.field_width(),
                    f.goal_depth(),
                    f.goal_width(),
                    f.penalty_area_depth(),
                    f.penalty_area_width(),
                    defaultField.freeBallMarkOffsetX(),
                    defaultField.freeBallMarkOffsetY());
      fieldKey.draw(DrawField(field.value(), args.drawFieldMarkings, args.drawCIninho));
    }
  }
  return {rawFrames, field};
}

VSSVISIONClient::ParsedTrackedPacket VSSVISIONClient::parseTrackedPacket() {
  if (!trackedPacket) {
    return std::nullopt;
  }
  static constexpr Geometry2D::CoordType<Point> MM_SCALE = 1000.0; // tracked packet is in meters.
  auto packet = std::move(trackedPacket.value());
  trackedPacket.reset();
  if (packet.has_tracked_frame()) {
    const auto& detection = packet.tracked_frame();

    Vision::OutputType::Tracked trackedFrame;
    /* getting balls info */ {
      QVector<std::pair<float, Ball>> balls;
      for (const auto& ball : detection.balls()) {
        auto position = Point(ball.pos().x(), ball.pos().y()) * MM_SCALE;
        auto velocity = Point(ball.vel().x(), ball.vel().y()) * MM_SCALE;

        // to be computed inside processed vision module.
        auto acceleration = Point(0, 0); // * MM_SCALE;

        balls += std::pair(ball.visibility(), Ball(position, velocity, acceleration));
      }
      /* TODO: How we can do it better? (ball visibility) */ {
        std::sort(balls.begin(), balls.end(), std::greater());
        if (!balls.empty()) {
          trackedFrame.set_ball(balls.front().second);
        }
      }
    }
    /* getting robots info */ {
      Robots<Robot> yellowRobots;
      Robots<Robot> blueRobots;
      for (const auto& robot : detection.robots()) {
        auto id = static_cast<int>(robot.robot_id().id());
        float angle = robot.orientation();
        float angularVelocity = 0; // robot.vel_angular();
        auto position = Point(robot.pos().x(), robot.pos().y()) * MM_SCALE;
        auto velocity = Point(robot.vel().x(), robot.vel().y()) * MM_SCALE;

        // to be computed inside processed vision module.
        auto acceleration = Point(0, 0); // * MM_SCALE;

        if (robot.robot_id().team() == RoboCupSSL::Team::YELLOW) {
          yellowRobots += Robot(id, angle, angularVelocity, position, velocity, acceleration);
        } else if (robot.robot_id().team() == RoboCupSSL::Team::BLUE) {
          blueRobots += Robot(id, angle, angularVelocity, position, velocity, acceleration);
        }
      }

      if (args.isYellow) {
        trackedFrame.set_allies(yellowRobots);
        trackedFrame.set_enemies(blueRobots);
      } else {
        trackedFrame.set_allies(blueRobots);
        trackedFrame.set_enemies(yellowRobots);
      }
    }
    return trackedFrame;
  }
  return std::nullopt;
}

static_block {
  Factory::vision.insert<VSSVISIONClient>("");
};
