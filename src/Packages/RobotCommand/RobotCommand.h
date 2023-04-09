//
// Created by jose-cruz on 13/02/2021.
//

#ifndef VSS_UNIFICATION_ROBOTCOMMAND_H
#define VSS_UNIFICATION_ROBOTCOMMAND_H

#include <soccer-common/soccer-common.h>

namespace Motion {
  class GoToPoint {
    REQUIRED(Point, target);
    PROPERTY(double, angle, 0.0);

   public:
    CTOR(GoToPoint, target);
    CTOR(GoToPoint, target, angle);
  };

  class WheelsRad {
    PROPERTY(double, ls, 30.0); // TODO: normalize between [0, 1.0]
    PROPERTY(double, rs, 30.0); // TODO: normalize between [0, 1.0]
   public:
    CTOR(WheelsRad, ls, rs);
  };

  class Spin {
    REQUIRED(bool, clockwise);
    PROPERTY(double, power, 30.0); // TODO: normalize between [0, 1.0]

   public:
    CTOR(Spin, clockwise);
    CTOR(Spin, clockwise, power);
  };
  using Type = std::variant<GoToPoint, Spin, WheelsRad>;
} // namespace Motion

class RobotCommand {
  REQUIRED(Motion::Type, motion);

 public:
  CTOR(RobotCommand, motion);
};

#endif // VSS_UNIFICATION_ROBOTCOMMAND_H
