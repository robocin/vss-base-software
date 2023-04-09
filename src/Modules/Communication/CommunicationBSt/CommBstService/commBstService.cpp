#include "commBstService.h"

CommBstService::CommBstService() {
  _commBstEth.setup(robocin::comm::NetworkType::vss,
                    CommBst::DEFAULT_DEVICE_IP,
                    CommBst::DEFAULT_DEVICE_PORT,
                    CommBst::DEFAULT_RECEIVE_IP,
                    (enableReceive ? CommBst::DEFAULT_RECEIVE_PORT : 0));
}
void CommBstService::setup() {
  if (!bstConfigured) {
    this->_commBstEth.setup(robocin::comm::NetworkType::vss,
                            CommBst::DEFAULT_DEVICE_IP,
                            CommBst::DEFAULT_DEVICE_PORT,
                            CommBst::DEFAULT_RECEIVE_IP,
                            (enableReceive ? CommBst::DEFAULT_RECEIVE_PORT : 0));
    bstConfigured = true;
    // only one setup by execution
  }
  QThread::usleep(600); // wait base-station config radio
}

void CommBstService::configTelemetry(bool turnOn) {
  this->_commBstEth.configReceiveChannel(turnOn,
                                         CommBst::DEFAULT_RECEIVE_IP,
                                         (enableReceive ? CommBst::DEFAULT_RECEIVE_PORT : 0));
}

void CommBstService::send(const QByteArray& data) {
  this->_commBstEth.sendPackage(data);
  QThread::usleep(600); // socket data queue control, equalize send and base-station consumer
}

QVector<robocin::comm::GenericPacket> CommBstService::read() {
  return this->_commBstEth.receive();
}
