//
// Created by jose-cruz on 09/02/2021.
//

#include "FIRASimCommunication.h"

FIRASimCommunication::FIRASimCommunication(QThreadPool* threadPool) :
    CommunicationBase(threadPool) {
}

void FIRASimCommunication::buildParameters(Parameters::Handler& parameters) {
  using namespace Parameters;

  parameters["IP"] = Text(args.ip);
  parameters["Port"] = Text(args.port);
  parameters["Replacement"]["Enable"] =
      CheckBox(args.replacement,
               "\n  * Key P replaces the ball;\n"
               "  * Key Y/B (Y for Yellow, B for Blue) + Key [0-9]");
  parameters["Replacement"]["Enable"]["true"]["Robot Angle"] =
      MappedAngleInRadiansToDegrees(args.replacementAngle, -PI, PI);
}

void FIRASimCommunication::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &VisionBase::sendIsYellow,
          this,
          &FIRASimCommunication::receiveIsYellow,
          Qt::DirectConnection);

  for (NavigationBase* navigation : modules->navigation()) {
    connect(navigation,
            &NavigationBase::sendNavigation,
            this,
            &FIRASimCommunication::receiveNavigation,
            Qt::DirectConnection);
  }

  /* keyboard and mouse replacement (connection type implicit queued). */ {
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizer::relativeMousePos,
            this,
            &FIRASimCommunication::receiveRelativeMousePos);
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizer::onKeyPress,
            this,
            &FIRASimCommunication::receiveKeyPress);
    connect(modules->gui()->gameVisualizer(),
            &GameVisualizer::onKeyRelease,
            this,
            &FIRASimCommunication::receiveKeyRelease);
  }
}

void FIRASimCommunication::init(const Modules* modules) {
}

void FIRASimCommunication::update() {
  if (args.ip.updated() || args.port.updated()) {
    setupClient();
  }
  if (args.replacement) {
    if (auto ballToReplace = shared.apply(&FIRASimCommunication::isBallReplacement)) {
      auto position = ballToReplace.value();
      auto packet = Simulation::Replacement::ball(position, Point(0, 0));
      emit sendDatagram(Protobuf::toByteArray(packet));
    }
    if (auto robotToReplace = shared.apply(&FIRASimCommunication::isRobotReplacement)) {
      auto [isYellowRobot, index, position] = robotToReplace.value();
      auto packet = Simulation::Replacement::robot(isYellowRobot,
                                                   index,
                                                   args.replacementAngle,
                                                   position,
                                                   Point(0, 0));
      emit sendDatagram(Protobuf::toByteArray(packet));
    }
  }
  shared->hasIsYellow.extract_to(hasIsYellow);
  packets = shared->packets.get_and_clear();
}

void FIRASimCommunication::exec() {
  if (!hasIsYellow) {
    return;
  }
  bool isYellow = hasIsYellow.value();
  if (!packets.empty()) {
    emit sendDatagram(Protobuf::toByteArray(Simulation::toCommunicationPacket(packets, isYellow)));
    packets.clear();
  }
}

void FIRASimCommunication::setupClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socket.reset(new QUdpSocket(this));
        qDebug() << "FIRASimCommunication::socket:" << socket.get() << "was created";
        connect(
            this,
            &CommunicationBase::sendDatagram,
            socket.get(),
            [this, ip = QHostAddress(args.ip.value()), port = args.port.value()](
                const QByteArray& datagram) {
              socket->writeDatagram(datagram, ip, port);
            },
            Qt::QueuedConnection); // must be queued.
      },
      Qt::QueuedConnection); // must be queued.
}

void FIRASimCommunication::receiveIsYellow(bool isYellow) {
  shared->hasIsYellow.emplace(isYellow);
}

void FIRASimCommunication::receiveRelativeMousePos(const Point& position) {
  shared->replacementPos.set(position);
}

void FIRASimCommunication::receiveKeyPress(Qt::Key key) {
  shared->pressedKeys->insert(key);
  runInParallel();
}

void FIRASimCommunication::receiveKeyRelease(Qt::Key key) {
  shared->pressedKeys->remove(key);
  runInParallel();
}

void FIRASimCommunication::receiveNavigation(const Navigation::Output& navigation) {
  shared->packets->append(navigation);
  runInParallel();
}

std::optional<Point> FIRASimCommunication::isBallReplacement(const Shared& obj) {
  if (obj.replacementPos.has_value() && obj.pressedKeys->contains(Qt::Key::Key_P)) {
    return obj.replacementPos.value();
  } else {
    return std::nullopt;
  }
}

std::optional<std::tuple<bool, int, Point>>
FIRASimCommunication::isRobotReplacement(const Shared& obj) {
  if (!obj.replacementPos.has_value()) {
    return std::nullopt;
  }
  std::optional<bool> optIsYellow;
  if (obj.pressedKeys->contains(Qt::Key::Key_Y)) {
    optIsYellow = true;
  } else if (obj.pressedKeys->contains(Qt::Key::Key_B)) {
    optIsYellow = false;
  } else {
    return std::nullopt;
  }
  auto isYellow = optIsYellow.value();
  for (int idx = 0; idx <= 9; ++idx) {
    if (obj.pressedKeys->contains(static_cast<Qt::Key>(static_cast<int>(Qt::Key::Key_0) + idx))) {
      return std::make_tuple(isYellow, idx, obj.replacementPos.value());
    }
  }
  return std::nullopt;
}

static_block {
  Factory::communication.insert<FIRASimCommunication>("");
};