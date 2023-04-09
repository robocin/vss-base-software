//
// Created by jose-cruz on 20/07/2021.
//

#include "Robot.h"

Robot::Robot(int id,
             real_t angle,
             real_t angularVelocity,
             const Point& position,
             const Point& velocity,
             const Point& acceleration) :
    Common::RawEntity<Point>(position),
    Common::Robot<Point>(id, angle, position, velocity, acceleration),
    m_angularVelocity(angularVelocity) {
}

Robot::Robot(const RawRobot& rawRobot,
             real_t angularVelocity,
             const Point& velocity,
             const Point& acceleration) :
    Robot(rawRobot.id(),
          rawRobot.angle(),
          angularVelocity,
          rawRobot.position(),
          velocity,
          acceleration) {
}

Robot::Robot(int id, real_t angle, real_t angularVelocity, const Entity& entity) :
    Robot(id, angle, angularVelocity, entity.position(), entity.velocity(), entity.acceleration()) {
}

bool Robot::hasCarryCondition(const Point& target, const Field& field) const {
  auto [minAngle, maxAngle] = std::minmax((field.enemyGoalOutsideBottom() - position()).angle(),
                                          (field.enemyGoalOutsideTop() - position()).angle());

  auto robotDirection = Geometry2D::fromPolar<Point>(angle());

  real_t distToTarget = target.distToLine(position(), position() + robotDirection);

  auto angle90 = Geometry::normalizeAngle<real_t>(angle(), 0, PI / 2);
  return (distToTarget <= halfAxis() && (minAngle <= angle90 && angle90 <= maxAngle));
}
