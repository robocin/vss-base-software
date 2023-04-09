
#ifndef VSS_UNIFICATION_COMMBSTSERVICE_H
#define VSS_UNIFICATION_COMMBSTSERVICE_H

#include "Modules/Communication/CommunicationBSt/CommBstService/CommBstEth/commbstEth.h"
#include "communication/commbst.h"
#include <QByteArray>
#include <QObject>
#include <QUdpSocket>
#include "Modules/Modules.h"

class CommBstService {
 public:
  CommBstService();
  void setup();
  void send(const QByteArray& data);
  QVector<robocin::comm::GenericPacket> read();
  void configTelemetry(bool turnOn);

 private:
  CommBstEth _commBstEth;
  bool bstConfigured = false;
  bool enableReceive = false;
};

typedef Singleton<CommBstService> BstService;

#endif // VSS_UNIFICATION_COMMBSTSERVICE_H
