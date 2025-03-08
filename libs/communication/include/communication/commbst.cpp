#include "commbst.h"

CommBst::VSSSpeedPacket CommBst::makeVSSSpeed(uint8_t id, double m1, double m2) {
  VSSSpeedPacket vss;

  std::memset(vss.encoded, 0, VSSSpeedPacket::size());

  vss.decoded.typeMsg = static_cast<uint8_t>(MessageType::VSS_SPEED);
  vss.decoded.id = id;
  vss.decoded.m1 = static_cast<int32_t>(m1 * 1000);
  vss.decoded.m2 = static_cast<int32_t>(m2 * 1000);

  return vss;
}

CommBst::SSLSpeedPacket CommBst::makeSSLSpeedPacket(uint8_t id,
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
                                                    uint8_t command) {
  SSLSpeedPacket ssl;

  std::memset(ssl.encoded, 0, SSLSpeedPacket::size());

  ssl.decoded.typeMsg = static_cast<uint8_t>(MessageType::SSL_SPEED);
  ssl.decoded.id = id;
  ssl.decoded.vx = static_cast<int32_t>(vx * 10000);
  ssl.decoded.vy = static_cast<int32_t>(vy * 10000);
  ssl.decoded.vw = static_cast<int32_t>(vw * 10000);
  ssl.decoded.front = static_cast<uint8_t>(front);
  ssl.decoded.chip = static_cast<uint8_t>(chip);
  ssl.decoded.charge = static_cast<uint8_t>((charge || bypassIR));
  ssl.decoded.strength = static_cast<uint8_t>(strength * 10);
  ssl.decoded.dribbler = static_cast<uint8_t>(dribbler);
  ssl.decoded.dribblerSpeed = static_cast<int16_t>(dribblerSpeed * 10);
  ssl.decoded.command = static_cast<uint8_t>(command);

  return ssl;
}

CommBst::PositionPacket CommBst::makeSourcePositionPacket(uint8_t id, robocin::comm::Vector v) {
  PositionPacket pos;
  std::memset(pos.encoded, 0, PositionPacket::size());

  pos.decoded.typeMsg = static_cast<uint8_t>(MessageType::POSITION);
  pos.decoded.positionType = static_cast<uint8_t>(robocin::comm::PositionType::source);
  pos.decoded.id = id;
  pos.decoded.x = static_cast<int16_t>(v.x * 1000);
  pos.decoded.y = static_cast<int16_t>(v.y * 1000);
  pos.decoded.w = static_cast<int16_t>(v.w * 10000);

  return pos;
}

CommBst::PositionPacket CommBst::makeTargetToGotoPointPacket(uint8_t id,
                                                             robocin::comm::Vector v,
                                                             double maxVelocity,
                                                             double minVelocity,
                                                             double kp,
                                                             bool usingPropSpeed,
                                                             double minDistanceToPropSpeed,
                                                             bool requireHighResolution,
                                                             double customAcceleration) {
  PositionPacket pos;
  std::memset(pos.encoded, 0, PositionPacket::size());
  pos.decoded.id = id;
  pos.decoded.typeMsg = static_cast<uint8_t>(MessageType::POSITION);
  pos.decoded.positionType = static_cast<uint8_t>(robocin::comm::PositionType::motionControl);

  pos.decoded.x = static_cast<int16_t>(v.x * 1000);
  pos.decoded.y = static_cast<int16_t>(v.y * 1000);
  pos.decoded.w = static_cast<int16_t>(v.w * 10000);

  pos.decoded.maxSpeed = static_cast<uint16_t>(maxVelocity * 1000);
  pos.decoded.minSpeed = static_cast<uint16_t>(minVelocity * 1000);
  pos.decoded.rotateKp = static_cast<uint16_t>(kp * 100);
  pos.decoded.usingPropSpeed = static_cast<uint8_t>(usingPropSpeed);
  pos.decoded.minDistanceToPropSpeed = static_cast<uint16_t>(minDistanceToPropSpeed * 1000);

  pos.decoded.clockwise = static_cast<bool>(requireHighResolution);
  pos.decoded.orbitRadius = static_cast<uint16_t>(customAcceleration * 1000);

  return pos;
}

CommBst::PositionPacket
CommBst::makeTargetToRotateOnSelfPacket(uint8_t id, double targetAngle, double kp) {
  PositionPacket pos;
  std::memset(pos.encoded, 0, PositionPacket::size());
  pos.decoded.id = id;
  pos.decoded.typeMsg = static_cast<uint8_t>(MessageType::POSITION);
  pos.decoded.positionType = static_cast<uint8_t>(robocin::comm::PositionType::rotateControl);

  pos.decoded.w = static_cast<int16_t>(targetAngle * 10000);
  pos.decoded.rotateKp = static_cast<uint16_t>(kp * 100);

  return pos;
}

void CommBst::setPeripheralActuation(PositionPacket& targetPacket,
                                     uint8_t front,
                                     uint8_t chip,
                                     uint8_t charge,
                                     uint8_t bypassIR,
                                     double strength,
                                     uint8_t dribbler,
                                     double dribblerSpeed,
                                     uint8_t command) {
  targetPacket.decoded.front = static_cast<uint8_t>(front);
  targetPacket.decoded.chip = static_cast<uint8_t>(chip);
  targetPacket.decoded.charge = static_cast<uint8_t>((charge || bypassIR));
  targetPacket.decoded.strength = static_cast<uint8_t>(strength * 10);
  targetPacket.decoded.dribbler = static_cast<uint8_t>(dribbler);
  targetPacket.decoded.dribblerSpeed = static_cast<int16_t>(dribblerSpeed * 10);
  targetPacket.decoded.command = static_cast<uint8_t>(command);
}

CommBst::PositionPacket CommBst::makeTargetToRotateInPointPacket(uint8_t id,
                                                                 robocin::comm::Vector v,
                                                                 double maxVelocity,
                                                                 double minVelocity,
                                                                 double kp,
                                                                 double orbitRadius,
                                                                 double approachKp,
                                                                 bool clockwise) {
  PositionPacket pos;
  std::memset(pos.encoded, 0, PositionPacket::size());
  pos.decoded.id = id;
  pos.decoded.typeMsg = static_cast<uint8_t>(MessageType::POSITION);
  pos.decoded.positionType = static_cast<uint8_t>(robocin::comm::PositionType::rotateInPoint);

  pos.decoded.x = static_cast<int16_t>(v.x * 1000);
  pos.decoded.y = static_cast<int16_t>(v.y * 1000);
  pos.decoded.w = static_cast<int16_t>(v.w * 10000);

  pos.decoded.maxSpeed = static_cast<uint16_t>(maxVelocity * 1000);
  pos.decoded.minSpeed = static_cast<uint16_t>(minVelocity * 1000);
  pos.decoded.rotateKp = static_cast<uint16_t>(kp * 100);
  pos.decoded.clockwise = static_cast<uint8_t>(clockwise);
  pos.decoded.orbitRadius = static_cast<uint16_t>(orbitRadius * 1000);
  pos.decoded.approachKp = static_cast<uint16_t>(approachKp * 100);

  return pos;
}

std::optional<CommBst::VSSRobotInfo> CommBst::readVSSTelemetry(const GenericPacket& received) {
  if (static_cast<MessageType>(received.decoded.typeMsg) == MessageType::VSS_TELEMETRY) {
    VSSTelemetryPacket telemetry;

    std::memcpy(telemetry.encoded, received.encoded, VSSTelemetryPacket::size());

    VSSRobotInfo info;
    info.id = telemetry.decoded.id;
    info.type = MessageType::VSS_TELEMETRY;

    info.m1 = telemetry.decoded.m1 / 1000.0;
    info.m2 = telemetry.decoded.m2 / 1000.0;

    info.battery = telemetry.decoded.battery / 10.0;

    return info;
  }
  return std::nullopt;
}

std::optional<CommBst::RobotInfo> CommBst::readTelemetry(const GenericPacket& received) {
  if (static_cast<MessageType>(received.decoded.typeMsg) == MessageType::TELEMETRY) {
    TelemetryPacket telemetry;

    std::memcpy(telemetry.encoded, received.encoded, TelemetryPacket::size());

    RobotInfo info;
    info.id = telemetry.decoded.id;
    info.type = MessageType::TELEMETRY;

    info.v.x = telemetry.decoded.x / 1000.0;
    info.v.y = telemetry.decoded.y / 1000.0;
    info.v.w = telemetry.decoded.w / 10000.0;

    info.m.m1 = telemetry.decoded.m1 / 100.0;
    info.m.m2 = telemetry.decoded.m2 / 100.0;
    info.m.m3 = telemetry.decoded.m3 / 100.0;
    info.m.m4 = telemetry.decoded.m4 / 100.0;

    info.dribbler = telemetry.decoded.dribbler / 10.0;
    info.ball = telemetry.decoded.ball;
    info.battery = telemetry.decoded.battery / 10.0;
    info.kickLoad = telemetry.decoded.kickLoad / 100.0;

    info.count = telemetry.decoded.pcktCount;

    return info;
  }
  return std::nullopt;
}

std::optional<CommBst::RobotInfo> CommBst::readOdometry(const GenericPacket& received) {
  if (static_cast<MessageType>(received.decoded.typeMsg) == MessageType::ODOMETRY) {
    OdometryPacket odometry;

    std::memcpy(odometry.encoded, received.encoded, OdometryPacket::size());

    RobotInfo info;
    info.type = MessageType::ODOMETRY;
    info.id = odometry.decoded.id;

    info.v.x = odometry.decoded.x / 1000.0;
    info.v.y = odometry.decoded.y / 1000.0;
    info.v.w = odometry.decoded.w / 10000.0;

    info.dribbler = odometry.decoded.dribbler / 10.0;
    info.ball = odometry.decoded.ball;
    info.battery = odometry.decoded.battery / 10.0;
    info.kickLoad = odometry.decoded.kickLoad / 100.0;

    info.m.m1 = odometry.decoded.m1 / 100.0;
    info.m.m2 = odometry.decoded.m2 / 100.0;
    info.m.m3 = odometry.decoded.m3 / 100.0;
    info.m.m4 = odometry.decoded.m4 / 100.0;

    info.count = odometry.decoded.pcktCount;

    return info;
  }
  return std::nullopt;
}

CommBst::BStConfigPacket CommBst::makeConfigBst(robocin::comm::NetworkType category) {
  BStConfigPacket config;

  if (category == robocin::comm::NetworkType::ssl) {
    config.decoded.typeMsg = static_cast<uint8_t>(MessageType::BST_CONFIG);
    config.decoded.duplex = true;
    config.decoded.team = static_cast<uint8_t>(robocin::comm::NetworkType::ssl);
    config.decoded.addr1 = SSL_ADDR_1;
    config.decoded.addr2 = SSL_ADDR_2;
    config.decoded.payload = SSL_PAYLOAD_LENGTH;
    config.decoded.channel1 = SSL_1_BASE_SEND_CH;
    config.decoded.channel2 = SSL_2_BASE_RECV_CH;
  } else if (category == robocin::comm::NetworkType::vss) {
    config.decoded.typeMsg = static_cast<uint8_t>(MessageType::BST_CONFIG);
    config.decoded.duplex = true;
    config.decoded.team = static_cast<uint8_t>(robocin::comm::NetworkType::vss);
    config.decoded.addr1 = VSS_ADDR_1;
    config.decoded.addr2 = VSS_ADDR_2;
    config.decoded.payload = VSS_PAYLOAD_LENGTH;
    config.decoded.channel1 = VSS_1_BASE_SEND_CH;
    config.decoded.channel2 = VSS_2_BASE_RECV_CH;
  }

  return config;
}
