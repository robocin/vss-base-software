//
// Created by jose-cruz on 20/07/2021.
//

#ifndef VSS_BASE_SOFTWARE_SIMULATIONUTILS_H
#define VSS_BASE_SOFTWARE_SIMULATIONUTILS_H

#include <protobufs/protobufs.h>
#include "Packages/Packages.h"

namespace Simulation {
  using PacketType = fira_message::sim_to_ref::Packet;

  namespace Replacement {
    using ReplacementType = fira_message::sim_to_ref::Replacement;
    using RobotReplacementType = fira_message::sim_to_ref::RobotReplacement;
    using BallReplacementType = fira_message::sim_to_ref::BallReplacement;

    static constexpr double M_TO_MM = 1000.0;

    /*!
     * @brief receives position and velocity in centimeters and sends in meters. Receives angle in
     * radians and sends in degrees.
     */
    inline PacketType robot(bool isYellow,
                            int id,
                            float angle,
                            QPointF position,
                            QPointF velocity,
                            float angularVelocity = 0,
                            bool isOnField = true) {
      RobotReplacementType robot;
      {
        using RobotType = fira_message::Robot;
        RobotType r;

        r.set_robot_id(static_cast<unsigned int>(id));
        r.set_orientation(Geometry::radiansToDegrees(angle));

        position /= M_TO_MM;
        r.set_x(static_cast<float>(position.x()));
        r.set_y(static_cast<float>(position.y()));

        velocity /= M_TO_MM;
        r.set_vx(static_cast<float>(velocity.x()));
        r.set_vy(static_cast<float>(velocity.y()));

        r.set_vorientation(angularVelocity);

        robot.set_allocated_position(new RobotType(r));
      }
      robot.set_yellowteam(isYellow);
      robot.set_turnon(isOnField);

      ReplacementType replacement;
      replacement.add_robots()->CopyFrom(robot);

      PacketType packet;
      packet.set_allocated_replace(new ReplacementType(replacement));
      return packet;
    }

    /*!
     * @brief receives in centimeters and sends in meters.
     */
    inline PacketType ball(Point position, Point velocity) {
      BallReplacementType ball;

      position /= M_TO_MM;
      ball.set_x(position.x());
      ball.set_y(position.y());

      velocity /= M_TO_MM;
      ball.set_vx(velocity.x());
      ball.set_vy(velocity.y());

      ReplacementType replacement;
      replacement.set_allocated_ball(new BallReplacementType(ball));

      PacketType packet;
      packet.set_allocated_replace(new ReplacementType(replacement));
      return packet;
    }
  } // namespace Replacement

  inline PacketType toCommunicationPacket(const QVector<Navigation::Output>& commands,
                                          bool isYellow) {
    PacketType packet;
    for (const auto& command : commands) {
      if (auto cmd = packet.mutable_cmd()->add_robot_commands()) {
        cmd->set_id(command.id());
        cmd->set_yellowteam(isYellow);
        cmd->set_wheel_left(command.leftWheel());
        cmd->set_wheel_right(command.rightWheel());
      } else {
        throw std::runtime_error("command for robot is nullptr.");
      }
    }
    return packet;
  }
} // namespace Simulation

#endif // VSS_BASE_SOFTWARE_SIMULATIONUTILS_H
