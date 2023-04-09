//
// Created by jose-cruz on 20/07/2021.
//

#ifndef VSS_UNIFICATION_ROBOT_H
#define VSS_UNIFICATION_ROBOT_H

#include <soccer-common/soccer-common.h>
#include "Field/Field.h"

using RawEntity = Common::RawEntity<Point>;
using Entity = Common::Entity<Point>;

using RawRobot = Common::RawRobot<Point>;

class Robot : public Common::Robot<Point> {
 public:
  Robot(int id,
        real_t angle,
        real_t angularVelocity,
        const Point& position,
        const Point& velocity,
        const Point& acceleration);
  Robot(const RawRobot<Point>& rawRobot,
        real_t angularVelocity,
        const Point& velocity,
        const Point& acceleration);
  Robot(int id, real_t angle, real_t angularVelocity, const Entity<Point>& entity);

  [[nodiscard]] inline real_t angularVelocity() const {
    return m_angularVelocity;
  }

  static constexpr real_t halfAxis() {
    return 37.5;
  }

  [[nodiscard]] bool hasCarryCondition(const Point& target, const Field& field) const;

 private:
  real_t m_angularVelocity;
};

#endif // VSS_UNIFICATION_ROBOT_H
