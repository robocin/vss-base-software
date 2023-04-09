#ifndef VSS_BASE_SOFTWARE_GOTOPOSITIONSM_H
#define VSS_BASE_SOFTWARE_GOTOPOSITIONSM_H

#include "Modules/Behavior/Player/PlayerUtils/PlayerUtils.h"

class GoToPositionSM : public Behavior::PlayerExtension {
 public:
  template <class M>
  inline GoToPositionSM(M* m, const PlayerWorld& world) :
      Behavior::PlayerExtension(this, m, world) {
  }

  inline Behavior::Output operator()(const TacticType::GoToPosition& goToPosition) {
    const Robot& robot = world.robot();
    if (robot.distTo(goToPosition.position()) < Robot::halfAxis()) {
      return Behavior::OutputType::Navigation::halt(index());
    }
    return Behavior::OutputType::Command(
        Motion::GoToPoint(goToPosition.position(), goToPosition.angle()));
  }
};

#endif // VSS_BASE_SOFTWARE_GOTOPOSITIONSM_H
