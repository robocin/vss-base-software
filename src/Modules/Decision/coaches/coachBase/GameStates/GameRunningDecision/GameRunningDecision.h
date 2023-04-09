#ifndef VSS_UNIFICATION_GAMERUNNINGDECISION_H
#define VSS_UNIFICATION_GAMERUNNINGDECISION_H

#include "Modules/Decision/DecisionUtils/DecisionUtils.h"
#include "Modules/Decision/ChoosePlayers/ChoosePlayers.h"

class GameRunningDecision : public Decision::Extension {
 public:
  template <class M>
  inline GameRunningDecision(M* m, const World& world) :
      Decision::Extension(this, m, world),
      choosePlayers(m, world) {
  }

  ChoosePlayers choosePlayers;

  Decision::Output operator()(const Referee::StateType::GameRunning&) {
    // TODO: Create your game Decision here
    auto field = world.field();
    if (world.has_world() && world.has_frame()) {
      if (field.allyPenaltyAreaContains(world.frame().ball())) {
        return avoidPenaltyDecision();
      } else {
        return attackDecision();
      }
    }

    return {};
  }

  Decision::Output halt() {
    Decision::Output output;
    auto allies = world.frame().allies();

    for (const auto& ally : allies) {
      output += Decision::Play(ally.id(), TacticType::Halt(ally.id()));
    }

    return output;
  }

  Decision::Output avoidPenaltyDecision() {
    // {Guard, GoToPosition}
    Decision::Output output;
    auto allies = world.frame().allies();

    if (auto goalkeeper = choosePlayers.takeGoalkeeper(allies)) {
      const int id = goalkeeper->id();
      output += Decision::Play(id, TacticType::Guard());
    }

    Point ballMask{world.field().allyPenaltyAreaCenter().x() + 200, world.frame().ball().y()};

    for (const auto& ally : allies) {
      output += Decision::Play(ally.id(), TacticType::GoToPosition(ballMask));
    }

    return output;
  }

  Decision::Output attackDecision() {
    // TODO: Create here your attack decision.
    Decision::Output output;

    return output;
  };
};

#endif // VSS_UNIFICATION_GAMERUNNINGDECISION_H
