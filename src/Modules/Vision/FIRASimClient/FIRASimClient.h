//
// Created by jose-cruz on 08/02/2021.
//

#ifndef VSS_UNIFICATION_FIRASIMCLIENT_H
#define VSS_UNIFICATION_FIRASIMCLIENT_H

#include <deque>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <protobufs/protobufs.h>
#include "Modules/Modules.h"
#include "Paintings/Paintings.h"

class FIRASimClient : public VisionBase {
  using EnvironmentPacket = fira_message::sim_to_ref::Environment;

  static constexpr double M_TO_MM = 1000.0;

 public:
  explicit FIRASimClient(QThreadPool* threadPool);

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  struct Args {
    Parameters::Arg<QString> ip = "224.0.0.1";
    Parameters::Arg<int> port = 10002;
    Parameters::Arg<QString> inet = QNetworkInterface::allInterfaces().front().name();
    Parameters::Arg<bool> isYellow = true;
    Parameters::Arg<bool> isAttackingToRight = true;

    Parameters::Arg<Field::Size> defaultFieldSize = Field::Size::Small;

    Parameters::Arg<bool> drawFieldMarkings = false;
    Parameters::Arg<bool> drawCIninho = true;
  };
  Args args;

  struct Shared {
    SharedValue<std::deque<EnvironmentPacket>> packets;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socket;

  std::deque<EnvironmentPacket> packets;

  GameVisualizer::Key frameKey;
  GameVisualizer::Key fieldKey;

  std::optional<Field> field;

  void setupClient();

 private slots:
  void receiveDatagram();
};

#endif // VSS_UNIFICATION_FIRASIMCLIENT_H
