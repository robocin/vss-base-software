#ifndef VSS_UNIFICATION_PREPARETOGOALKICKDECISION_H
#define VSS_UNIFICATION_PREPARETOGOALKICKDECISION_H

#include "Modules/Decision/DecisionUtils/DecisionUtils.h"
#include "Utils/Utils.h"

namespace {
  using namespace Decision;
}

class PrepareToGoalKickDecision : public Extension {

  struct Args {
    struct Defending {
      ArgRobot firstRobot{-675.0, 0.0, 0.0};
      ArgRobot secondRobot{225.0, -100.0, 0.0};
      ArgRobot thirdRobot{150.0, 320.0, 0.0};
    };
    Defending defending;

    struct Kicking {
      ArgRobot firstRobot{-675.0, -270.0, PI / 4};
      ArgRobot secondRobot{-450.0, -320.0, 0.0};
      ArgRobot thirdRobot{-300.0, 470.0, 0.0};
    };
    Kicking kicking;
  };
  Args args;

 public:
  template <class M>
  inline PrepareToGoalKickDecision(M* m, const World& world) : Extension(this, m, world) {
  }

  void extendedBuildParameters(Parameters::Handler& parameters) final {
    static constexpr real_t LIMIT = 5000.0;

    parameters["Kicking"]["1st Robot"] = args.kicking.firstRobot.handler(-LIMIT, +LIMIT);
    parameters["Kicking"]["2nd Robot"] = args.kicking.secondRobot.handler(-LIMIT, +LIMIT);
    parameters["Kicking"]["3rd Robot"] = args.kicking.thirdRobot.handler(-LIMIT, +LIMIT);

    parameters["Defending"]["1st Robot"] = args.defending.firstRobot.handler(-LIMIT, +LIMIT);
    parameters["Defending"]["2nd Robot"] = args.defending.secondRobot.handler(-LIMIT, +LIMIT);
    parameters["Defending"]["3rd Robot"] = args.defending.thirdRobot.handler(-LIMIT, +LIMIT);
  }

  Output operator()(const Referee::StateType::PrepareToDefendGoalKick&) {
    auto formation = getFormation(world,
                                  args.defending.firstRobot,
                                  args.defending.secondRobot,
                                  args.defending.thirdRobot);

    emit sendTeleport(toTeleportDatagram(world, formation));

    Output output;
    for (const RawRobot& robot : formation) {
      output += Play(robot.id(), TacticType::GoToPosition(robot.position()));
    }
    return output;
  }

  Output operator()(const Referee::StateType::PrepareToKickGoalKick&) {
    auto formation = getFormation(world,
                                  args.kicking.firstRobot,
                                  args.kicking.secondRobot,
                                  args.kicking.thirdRobot);

    emit sendTeleport(toTeleportDatagram(world, formation));

    Output output;
    for (const RawRobot& robot : formation) {
      output += Play(robot.id(), TacticType::GoToPosition(robot.position()));
    }
    return output;
  }
};

#endif // VSS_UNIFICATION_PREPARETOGOALKICKDECISION_H
