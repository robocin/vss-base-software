#ifndef VSS_UNIFICATION_GUARDSM_H
#define VSS_UNIFICATION_GUARDSM_H

#include "Modules/Behavior/Player/PlayerUtils/PlayerUtils.h"

// TODO: Create your goalkeeper logic here.
class GuardSM : public Behavior::PlayerExtension {
 public:
  template <class M>
  inline GuardSM(M* m, const PlayerWorld& world) : Behavior::PlayerExtension(this, m, world) {
  }

  struct Args {
    Parameters::Arg<float> guardOffset = -10.;
  };
  Args args;

  inline void extendedBuildParameters(Parameters::Handler& parameters) final {
    parameters["Guard offset"] = Parameters::Text<float>(args.guardOffset);
  }

  inline Behavior::Output operator()(const TacticType::Guard&) {
    auto field = world.field();
    auto ball = world.frame().ball() + world.frame().ball().velocity() * 0.08;
    auto robot = world.robot();
    const float attackDirection = static_cast<float>(world.field().attackDirection().x());
    double posx = field.allyGoalOutsideCenter().x();
    posx += attackDirection * args.guardOffset;
    double poxy = ball.y();
    poxy = std::clamp(poxy, field.allyGoalInsideBottom().y(), field.allyGoalInsideTop().y());

    auto pos = Point(posx, poxy);
    if (robot.distTo(pos) < Robot::halfAxis()) {
      return Behavior::OutputType::Navigation::halt(index());
    }
    return Behavior::OutputType::Command(Motion::GoToPoint(pos));
  }
};

#endif // VSS_UNIFICATION_GUARDSM_H
