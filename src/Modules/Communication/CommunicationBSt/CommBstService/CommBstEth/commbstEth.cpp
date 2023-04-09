#include "commbstEth.h"

CommBstEth::CommBstEth() {
  _udpSocket = std::make_unique<QUdpSocket>(nullptr);
}

void CommBstEth::setup(robocin::comm::NetworkType type,
                       const char* d_ip,
                       quint16 d_p,
                       const char* r_ip,
                       quint16 r_p) {
  _host = QString(d_ip);
  _port = d_p;

  if (r_p != 0)
    configReceiveChannel(true, r_ip, r_p);

  sendConfigBst(type);
}

void CommBstEth::sendConfigBst(robocin::comm::NetworkType type) {
  auto configBst = CommBst::makeConfigBst(type);
  _udpSocket->writeDatagram(configBst.c_str(),
                            CommBst::BStConfigPacket::size(),
                            QHostAddress(_host),
                            _port);
}

QVector<robocin::comm::GenericPacket> CommBstEth::receive() {
  QVector<robocin::comm::GenericPacket> packages;
  while (_udpSocket->hasPendingDatagrams()) {
    robocin::comm::GenericPacket packet;
    QByteArray rcvDatagram(static_cast<int>(_udpSocket->pendingDatagramSize()),
                           static_cast<char>(0));
    if (_udpSocket->readDatagram(rcvDatagram.data(), rcvDatagram.size()) > 0) {
      CommBst::GenericPacket data;
      std::memcpy(data.encoded, rcvDatagram, rcvDatagram.size());
      packages.append(data);
    }
  }
  return packages;
}

void CommBstEth::configReceiveChannel(bool turnOn, const char* r_ip, quint16 r_p) {
  if (turnOn) {
    _udpSocket->bind(QHostAddress(r_ip), r_p);
  } else {
    _udpSocket->close();
  }
}

void CommBstEth::sendPackage(const QByteArray& data) {
  _udpSocket->writeDatagram(data, QHostAddress(_host), _port);
}
