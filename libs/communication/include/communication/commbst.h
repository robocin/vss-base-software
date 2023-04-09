/**Communication class between PC and Robots
 * @author RoboCIn
 *
 * This class has a group of methods to be used for communication between the main PC and all the
 * robots using the nRF24L01+ radios.
 *
 */
#ifndef VSS_BASE_SOFTWARE_COMMBST_H
#define VSS_BASE_SOFTWARE_COMMBST_H

#include <iostream>
#include <cstring>
#include <optional>

#include "nRF24Communication/commConfig.h"
#include "nRF24Communication/commTypes.h"

class CommBst {
 public:
  // Definitions
  inline static const char* DEFAULT_DEVICE_IP = "199.0.1.1";
  static constexpr int DEFAULT_DEVICE_PORT = 9600;
  inline static const char* DEFAULT_RECEIVE_IP = "199.0.1.2";
  static constexpr int DEFAULT_RECEIVE_PORT = 9601;

  using MessageType = robocin::comm::MessageType;

  using GenericPacket = robocin::comm::GenericPacket;

  using TelemetryPacket = robocin::comm::TelemetryPacket;
  using OdometryPacket = robocin::comm::OdometryPacket;

  using VSSSpeedPacket = robocin::comm::VSSSpeedPacket;
  using SSLSpeedPacket = robocin::comm::SSLSpeedPacket;

  using PositionPacket = robocin::comm::PositionPacket;

  using BStConfigPacket = robocin::comm::BStConfigPacket;

  using RobotInfo = robocin::comm::RobotInfo;

  /**
   * Method to make a VSS speed message type!
   * @param id
   * @param motorSpeed1
   * @param motorSpeed2
   * @param flags
   */
  static VSSSpeedPacket
  makeVSSSpeed(uint8_t id, int8_t motorSpeed1, int8_t motorSpeed2, uint8_t flags);

  /**
   * Method to make a SSL speed message type!
   * @param id
   * @param vx
   * @param vy
   * @param vw
   * @param front
   * @param chip
   * @param charge
   * @param bypassIR
   * @param strength
   * @param dribbler
   * @param dribblerSpeed
   */
  static SSLSpeedPacket makeSSLSpeedPacket(uint8_t id,
                                           double vx,
                                           double vy,
                                           double vw,
                                           uint8_t front,
                                           uint8_t chip,
                                           uint8_t charge,
                                           uint8_t bypassIR,
                                           double strength,
                                           uint8_t dribbler,
                                           double dribblerSpeed,
                                           uint8_t command);

  static PositionPacket makeSourcePositionPacket(uint8_t id, robocin::comm::Vector v);
  static PositionPacket makeTargetToGotoPointPacket(uint8_t id,
                                                    robocin::comm::Vector v,
                                                    double maxVelocity,
                                                    double minVelocity,
                                                    double kp,
                                                    bool usingPropSpeed,
                                                    double minDistanceToPropSpeed,
                                                    bool requireHighResolution,
                                                    double customAcceleration);

  static PositionPacket makeTargetToRotateInPointPacket(uint8_t id,
                                                        robocin::comm::Vector v,
                                                        double maxVelocity,
                                                        double minVelocity,
                                                        double kp,
                                                        double orbitRadius,
                                                        double approachKp,
                                                        bool clockwise);

  static PositionPacket makeTargetToRotateOnSelfPacket(uint8_t id, double targetAngle, double kp);

  static void setPeripheralActuation(PositionPacket& targetPacket,
                                     uint8_t front,
                                     uint8_t chip,
                                     uint8_t charge,
                                     uint8_t bypassIR,
                                     double strength,
                                     uint8_t dribbler,
                                     double dribblerSpeed,
                                     uint8_t command);

  static std::optional<RobotInfo> readTelemetry(const GenericPacket& received);
  static std::optional<RobotInfo> readOdometry(const GenericPacket& received);

  static BStConfigPacket makeConfigBst(robocin::comm::NetworkType category);
};

#endif // VSS_BASE_SOFTWARE_COMMBST_H
