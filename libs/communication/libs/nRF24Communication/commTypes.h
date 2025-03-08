#ifndef COMM_TYPES_H
#define COMM_TYPES_H

/************* AUXILIAR TYPES ************/
#include <cstdint>

#include "commConfig.h"

namespace robocin::comm {
  enum class RadioFunction { receiver, sender };

  /*
   * Network type / catergory configuration
   *  Attencion: Limited in 15
   */
  enum class NetworkType { unknown = 0, generic, ssl, vss };

  /*
   * Type of the packet position.
   *  Attencion: Limited in 8
   */
  enum class PositionType {
    unknown = 0,
    source,
    destiny,
    motionControl,
    rotateControl,
    rotateInPoint
  };

  struct NetworkConfig {
    uint8_t payload;
    uint64_t addr[2];
    bool ack;
    uint8_t receiveChannel;
    uint8_t sendChannel;
    uint8_t pipeNum;
    bool reConfig;
    RadioFunction function;
  };

  struct Motors {
    double m1 = 0;
    double m2 = 0;
    double m3 = 0;
    double m4 = 0;
  };

  struct Vector {
    double x = 0;
    double y = 0;
    double w = 0;

    inline Vector(double x, double y, double w) : x(x), y(y), w(w) {
    }

    inline Vector() = default;

    inline Vector operator+(const Vector& a) const {
      Vector b;
      b.x = x + a.x;
      b.y = y + a.y;
      b.w = w + a.w;
      return b;
    }

    inline Vector operator-(const Vector& a) const {
      Vector b;
      b.x = x - a.x;
      b.y = y - a.y;
      b.w = w - a.w;
      return b;
    }

    inline Vector operator*(double a) const {
      Vector b;
      b.x = x * a;
      b.y = y * a;
      b.w = w * a;
      return b;
    }
  };

  struct KickFlags {
    bool front = false;
    bool chip = false;
    bool charge = false;
    float kickStrength = 0;
    bool ball = false;
    bool dribbler = false;
    float dribblerSpeed = 0;

    KickFlags& operator=(const KickFlags& a) = default;
  };

  struct RobotPosition {
    Vector v;
    PositionType type = PositionType::unknown;
    double maxSpeed{};
    double minSpeed{};
    double rotateKp{};
    bool usingPropSpeed{};
    double minDistanceToPropSpeed{};
    bool rotateInClockWise{};
    double orbitRadius{};
    double approachKp{};
  };

  struct RobotInfo {
    int id = -1;
    MessageType type = MessageType::NONE;
    Motors m;
    Vector v;
    double dribbler = 0;
    double kickLoad = 0;
    bool ball = false;
    double battery = 0;
    uint8_t count = 0; // Packet Count
  };

  struct VSSRobotInfo {
    int id = -1;
    MessageType type = MessageType::NONE;
    double m1 = 0;
    double m2 = 0;
    double battery = 0;
  };
} // namespace robocin::comm

#endif // COMM_TYPES_H