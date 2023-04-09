//
// Created by jose-cruz on 06/08/2021.
//

#ifndef VSS_BASE_SOFTWARE_PLAYER_H
#define VSS_BASE_SOFTWARE_PLAYER_H

#include "Modules/Modules.h"

#include "StateMachines/StateMachines.h"

class Player : public BehaviorBase, public overloaded_visitor_t<HaltSM, GoToPositionSM, GuardSM> {
 public:
  Player(int index, QThreadPool* threadPool);

  using BehaviorBase::index;

  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

  struct Shared {
    SharedOptional<bool> hasIsYellow;
    SharedOptional<Field> field;
    SharedOptional<Referee::Output> referee;
    SharedOptional<std::tuple<Frame, Robot>> frameAndRobot;
    SharedOptional<std::tuple<Decision::Play, Decision::Output>> decision;
  };
  SharedWrapper<Shared, std::mutex> shared;

  PlayerWorld world;

 public slots:
  void receiveIsYellow(bool rcvdIsYellow);
  void receiveField(const Field& rcvdField);
  void receiveReferee(const Referee::Output& rcvdReferee);
  void receiveFrameAndRobot(const Frame& rcvdFrame, const Robot& rcvdRobot);
  void receiveDecision(const Decision::Play& rcvdPlay, const Decision::Output& rcvdPlays);
};

#endif // VSS_BASE_SOFTWARE_PLAYER_H
