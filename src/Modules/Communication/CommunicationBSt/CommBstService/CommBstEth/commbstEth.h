#ifndef VSS_UNIFICATION_COMMBST_ETH_H
#define VSS_UNIFICATION_COMMBST_ETH_H

#include "communication/commbst.h"
#include <QUdpSocket>

class CommBstEth {
 public:
  CommBstEth();

  void setup(robocin::comm::NetworkType type,
             const char* d_ip = CommBst::DEFAULT_DEVICE_IP,
             quint16 d_p = CommBst::DEFAULT_DEVICE_PORT,
             const char* r_ip = CommBst::DEFAULT_RECEIVE_IP,
             quint16 r_p = CommBst::DEFAULT_RECEIVE_PORT);

  void configReceiveChannel(bool turnOn,
                            const char* r_ip = CommBst::DEFAULT_RECEIVE_IP,
                            quint16 r_p = CommBst::DEFAULT_RECEIVE_PORT);

  void sendPackage(const QByteArray& data);

  QVector<robocin::comm::GenericPacket> receive();

 private:
  void sendConfigBst(robocin::comm::NetworkType type);

  std::unique_ptr<QUdpSocket> _udpSocket = nullptr;
  QString _host;
  quint16 _port;
};

#endif // VSS_UNIFICATION_COMMBST_ETH_H
