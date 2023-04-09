//
// Created by jose-cruz on 17/07/2021.
//

#ifndef VSS_UNIFICATION_COMMUNICATIONBST_H
#define VSS_UNIFICATION_COMMUNICATIONBST_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include "Modules/Modules.h"
#include "BaseStationUtils/BaseStationUtils.h"
#include "communication/commbst.h"
#include "Modules/Communication/CommunicationBSt/CommBstService/commBstService.h"

class CommunicationBSt : public CommunicationBase {
  Q_OBJECT
 public:
  explicit CommunicationBSt(QThreadPool* threadPool);

 protected:
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  struct Shared {
    std::array<SharedOptional<Navigation::Output>, ROBOCIN_MAX_ROBOTS> commands;
  };
  SharedWrapper<Shared, std::mutex> shared;
  QVector<Navigation::Output> commands;

 private slots:
  void receiveNavigation(const Navigation::Output& navigation);
};

#endif // VSS_UNIFICATION_COMMUNICATIONBST_H
