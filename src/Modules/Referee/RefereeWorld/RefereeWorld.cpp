//
// Created by jose-cruz on 04/08/2021.
//

#include "RefereeWorld.h"

RefereeWorld::RefereeWorld(QThreadPool* threadPool) : RefereeBase(threadPool) {
}

void RefereeWorld::buildParameters(Parameters::Handler& parameters) {
  using namespace Parameters;

  parameters["IP"] = Text(args.ip);
  parameters["Port"] = Text(args.port);
  // Network Interface //
  {
    QStringList names;
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto& interface : interfaces) {
      names += interface.name();
    }
    parameters["INet"] = ComboBox(args.inet, names);
  }
}

void RefereeWorld::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &VisionBase::sendIsYellow,
          this,
          &RefereeWorld::receiveIsYellow,
          Qt::DirectConnection);
}

void RefereeWorld::init(const Modules* modules) {
}

void RefereeWorld::update() {
  if (args.ip.updated() || args.port.updated() || args.inet.updated()) {
    setupClient();
  }
  shared->hasIsYellow.extractTo(hasIsYellow);
  shared->packet.extractTo(packet);
}

void RefereeWorld::exec() {
  if (!hasIsYellow || !packet) {
    return;
  }
  emit sendReferee(Referee::Parser::parse(packet.value(), hasIsYellow.value()));
  packet.reset();
}

void RefereeWorld::setupClient() {
  QMetaObject::invokeMethod(
      this,
      [this]() {
        socket.reset(new QUdpSocket(this));
        if (socket->bind(QHostAddress::AnyIPv4, args.port, QUdpSocket::ShareAddress)) {
          qDebug() << "RefereeWorld::socket connected on port" << args.port;
        }
        if (socket->joinMulticastGroup(QHostAddress(args.ip),
                                       QNetworkInterface::interfaceFromName(args.inet))) {
          qDebug() << "RefereeWorld::socket connected with ip" << args.ip << "and interface"
                   << args.inet;
        }
        connect(socket.get(),
                &QUdpSocket::readyRead,
                this,
                &RefereeWorld::receiveDatagrams,
                Qt::DirectConnection); // can be directed.
      },
      Qt::QueuedConnection); // must be queued.
}

void RefereeWorld::receiveDatagrams() {
  std::optional<RefereePacket> received;
  while (socket->hasPendingDatagrams()) {
    QByteArray datagram(static_cast<int>(socket->pendingDatagramSize()), static_cast<char>(0));
    socket->readDatagram(datagram.data(), datagram.size());
    if (auto parsed = Protobuf::fromByteArray<RefereePacket>(datagram)) {
      received = parsed;
    }
  }
  if (received) {
    shared->packet = received;
    runInParallel();
  }
}

void RefereeWorld::receiveIsYellow(bool rcvIsYellow) {
  shared->hasIsYellow = rcvIsYellow;
}

static_block {
  Factory::referee.insert<RefereeWorld>();
};
