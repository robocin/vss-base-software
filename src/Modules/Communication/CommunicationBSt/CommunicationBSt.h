//
// Created by jose-cruz on 17/07/2021.
//

#ifndef VSS_BASE_SOFTWARE_COMMUNICATIONBST_H
#define VSS_BASE_SOFTWARE_COMMUNICATIONBST_H

#define BATTERY_WARNING_VOLTAGE 7.6
#define BATTERY_CRITICAL_VOLTAGE 7.4

#include <QUdpSocket>
#include <QNetworkInterface>
#include "Modules/Modules.h"
#include "BaseStationUtils/BaseStationUtils.h"
#include "communication/commbst.h"
#include "Modules/Communication/CommunicationBSt/CommBstService/commBstService.h"
#include <ranges>

class CommunicationBSt : public CommunicationBase {
  Q_OBJECT

 public:
  using GenericPacket = robocin::comm::GenericPacket;
  using VSSRobotInfo = robocin::comm::VSSRobotInfo;
  using Telemetry = std::pair<VSSRobotInfo, QTime>;

  explicit CommunicationBSt(QThreadPool* threadPool);

 protected:
  void connectModules(const Modules* modules) override;
  void buildParameters(Parameters::Handler& parameters) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  bool bstConfigured = false;
  GameVisualizer::Key commandsKey;
  GameVisualizer::Key batteryKey;
  struct Shared {
    std::array<SharedOptional<Navigation::Output>, ROBOCIN_MAX_ROBOTS> commands;
    SharedOptional<Frame> frame;
    SharedOptional<bool> hasIsYellow;
  };
  SharedWrapper<Shared, std::mutex> shared;
  QVector<Navigation::Output> commands;
  std::optional<Frame> frame;
  std::optional<bool> hasIsYellow;
  std::array<std::optional<Telemetry>, ROBOCIN_MAX_ROBOTS> telemetries;

  void parseTelemetry(const QVector<GenericPacket>& packages);

 private slots:
  void receiveNavigation(const Navigation::Output& navigation);
  void receiveFrame(const Frame& frame);
  void receiveIsYellow(bool rcvdIsYellow);
};

#endif // VSS_BASE_SOFTWARE_COMMUNICATIONBST_H
