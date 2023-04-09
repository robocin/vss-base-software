//
// Created by jose-cruz on 04/08/2021.
//

#ifndef VSS_UNIFICATION_REFEREEWORLD_H
#define VSS_UNIFICATION_REFEREEWORLD_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include <protobufs/protobufs.h>
#include "Modules/Modules.h"
#include "Modules/Referee/RefereeUtils/RefereeUtils.h"

class RefereeWorld : public RefereeBase {
  using RefereePacket = VSSRef::ref_to_team::VSSRef_Command;

 public:
  explicit RefereeWorld(QThreadPool* threadPool);
  ~RefereeWorld() override = default;

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

  struct Args {
    Parameters::Arg<QString> ip = "224.5.23.2";
    Parameters::Arg<int> port = 10003;
    Parameters::Arg<QString> inet = QNetworkInterface::allInterfaces().front().name();
  };
  Args args;

  struct Shared {
    SharedOptional<RefereePacket> packet;
    SharedOptional<bool> hasIsYellow;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::optional<bool> hasIsYellow;

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socket;
  std::optional<RefereePacket> packet;

  void setupClient();

 private slots:
  void receiveDatagrams();
  void receiveIsYellow(bool rcvIsYellow);
};

#endif // VSS_UNIFICATION_REFEREEWORLD_H
