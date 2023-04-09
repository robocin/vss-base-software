#ifndef VSS_BASE_SOFTWARE_VSSCOACH3X3_H
#define VSS_BASE_SOFTWARE_VSSCOACH3X3_H

#include <QUdpSocket>
#include <QNetworkInterface>

#include "Modules/Modules.h"

#include "GameStates/GameStates.h"

namespace StateType = Referee::StateType;

class VSSCOACH3x3 : public DecisionBase,
                    public Decision::Extensions<Referee::State,
                                                GameRunningDecision,
                                                HaltDecision,
                                                PrepareToFreeBallDecision,
                                                PrepareToGoalKickDecision,
                                                PrepareToKickoffDecision,
                                                PrepareToPenaltyDecision> {
  Q_OBJECT

  struct Args {
    Parameters::Arg<bool> teleport = false;
    Parameters::Arg<bool> debug = false;

    struct SocketArgs {
      Parameters::Arg<QString> ip;
      Parameters::Arg<int> port;
      Parameters::Arg<QString> inet;
    };

    SocketArgs teleporter{"224.5.23.2", 10004, QNetworkInterface::allInterfaces().front().name()};
  };
  Args args;

  struct Shared {
    SharedOptional<bool> hasIsYellow;
    SharedOptional<Frame> frame;
    SharedOptional<Field> field;
    SharedOptional<Referee::Output> referee;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::unique_ptr<QUdpSocket, DeleteLaterDeleter> teleporter;

  World world;

 public:
  explicit VSSCOACH3x3(QThreadPool* threadPool);
  ~VSSCOACH3x3() override = default;

  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;
  void debug();
  void receiveIsYellow(bool isYellow);
  void receiveFrame(const Frame& frame);
  void receiveField(const Field& field);
  void receiveReferee(const Referee::Output& referee);
  QPolygonF area0;
  QPolygonF area1;
  QPolygonF area2;
  QPolygonF area3;
  QPolygonF area4;
  std::vector<QPolygonF> areas;

 private:
  void setupTeleporter(bool enabled);
  GameVisualizer::Key paintKey;

 signals:
  void sendTeleport(const QByteArray& datagram);
};

#endif // VSS_BASE_SOFTWARE_VSSCOACH3X3_H
