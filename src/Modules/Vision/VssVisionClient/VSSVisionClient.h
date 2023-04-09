//
// Created by felipe on 16/05/2022.
//

#ifndef VSS_BASE_SOFTWARE_VSSVISIONCLIENT_H
#define VSS_BASE_SOFTWARE_VSSVISIONCLIENT_H

#include <deque>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <protobufs/protobufs.h>
#include "Modules/Modules.h"
#include "Paintings/Paintings.h"

class VSSVISIONClient : public VisionBase {
  using RawWrapperPacket = RoboCupSSL::SSL_WrapperPacket;
  using TrackedWrapperPacket = RoboCupSSL::TrackerWrapperPacket;

 public:
  explicit VSSVISIONClient(QThreadPool* threadPool);

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  struct Args {
    struct SocketArgs {
      Parameters::Arg<QString> ip;
      Parameters::Arg<int> port;
      Parameters::Arg<QString> inet;
    };

    SocketArgs raw{"224.5.23.2", 10008, QNetworkInterface::allInterfaces().front().name()};
    SocketArgs tracked{"224.5.23.2", 10018, QNetworkInterface::allInterfaces().front().name()};

    Parameters::Arg<bool> isTracked = false;

    Parameters::Arg<bool> isYellow = true;
    Parameters::Arg<bool> isAttackingToRight = true;

    Parameters::Arg<Field::Size> defaultFieldSize = Field::Size::Small;

    Parameters::Arg<bool> drawFieldMarkings = false;
    Parameters::Arg<bool> drawCIninho = true;
  };
  Args args;

  struct Shared {
    SharedValue<std::deque<RawWrapperPacket>> rawPackets;
    SharedOptional<TrackedWrapperPacket> trackedPacket;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socketRaw;
  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> socketTracked;

  std::deque<RawWrapperPacket> rawPackets;
  std::optional<TrackedWrapperPacket> trackedPacket;

  GameVisualizer::Key frameKey;
  GameVisualizer::Key fieldKey;

  void setupRawClient();
  void setupTrackedClient();

  using ParsedRawPacket = std::pair<Vision::OutputType::Raw, std::optional<Field>>;
  using ParsedTrackedPacket = std::optional<Vision::OutputType::Tracked>;

  ParsedRawPacket parseRawPacket();
  ParsedTrackedPacket parseTrackedPacket();

 private slots:
  void receiveRawDatagrams();
  void receiveTrackedDatagrams();
};

#endif // VSS_BASE_SOFTWARE_VSSVISIONCLIENT_H
