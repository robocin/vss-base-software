#ifndef VSS_BASE_SOFTWARE_PREPARETOKICKOFFDECISION_H
#define VSS_BASE_SOFTWARE_PREPARETOKICKOFFDECISION_H

#include "Modules/Decision/DecisionUtils/DecisionUtils.h"
#include "Utils/Utils.h"

namespace {
  using namespace Decision;
}

class PrepareToKickoffDecision : public Extension {
  struct Args {
    struct Defending {
      ArgRobot firstRobot{-675.0, 0.0, 0.0}; // Goalkeeper
      ArgRobot secondRobot{-200.0, 0.0, 0.0};
      ArgRobot thirdRobot{-400.0, 0.0, 0.0};
    };
    Defending defending;

    struct Attacking {
      ArgRobot firstRobot{-675.0, 0.0, 0.0}; // Goalkeeper
      ArgRobot secondRobot{-200.0, 0.0, 0.0};
      ArgRobot thirdRobot{-400.0, 0.0, 0.0};
    };
    Attacking attacking;
  };
  Args args;

 public:
  template <class M>
  inline PrepareToKickoffDecision(M* m, const World& world) : Extension(this, m, world) {
  }

  void extendedBuildParameters(Parameters::Handler& parameters) final {
    static constexpr real_t LIMIT = 5000.0;

    parameters["Attacking"]["1st Robot"] = args.attacking.firstRobot.handler(-LIMIT, +LIMIT);
    parameters["Attacking"]["2nd Robot"] = args.attacking.secondRobot.handler(-LIMIT, +LIMIT);
    parameters["Attacking"]["3rd Robot"] = args.attacking.thirdRobot.handler(-LIMIT, +LIMIT);

    parameters["Defending"]["1st Robot"] = args.defending.firstRobot.handler(-LIMIT, +LIMIT);
    parameters["Defending"]["2nd Robot"] = args.defending.secondRobot.handler(-LIMIT, +LIMIT);
    parameters["Defending"]["3rd Robot"] = args.defending.thirdRobot.handler(-LIMIT, +LIMIT);
  }

  Output operator()(const Referee::StateType::PrepareToKickKickoff&) {
    auto formation = getFormation(world,
                                  args.attacking.firstRobot,
                                  args.attacking.secondRobot,
                                  args.attacking.thirdRobot);

    emit sendTeleport(toTeleportDatagram(world, formation));

    Output output;
    for (const RawRobot& robot : formation) {
      output += Play(robot.id(), TacticType::GoToPosition(robot.position()));
    }
    return output;
  }

  Output operator()(const Referee::StateType::PrepareToDefendKickoff&) {
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
};

#endif // VSS_BASE_SOFTWARE_PREPARETOKICKOFFDECISION_H
