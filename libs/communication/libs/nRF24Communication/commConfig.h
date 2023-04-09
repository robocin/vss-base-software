#ifndef COMM_CONFIG_H
#define COMM_CONFIG_H

// v3.1

#include <array>
#include <cstdint>

// NRF Config Defitions
#define SSL_1_BASE_SEND_CH 103
#define SSL_2_BASE_RECV_CH 115

#define SSL_1_ROBOT_RECV_CH 103
#define SSL_2_ROBOT_SEND_CH 115

#define SSL_ADDR_1 0xABAAADA99ALL
#define SSL_ADDR_2 0xADAADC9A4BLL

#define VSS_1_BASE_SEND_CH 108
#define VSS_2_BASE_RECV_CH 110

#define VSS_1_ROBOT_SEND_CH 110
#define VSS_2_ROBOT_RECV_CH 108

#define VSS_ADDR_1 0x752FAF0A9ALL
#define VSS_ADDR_2 0x5D4BFBC2BBLL

#define ACK_RADIO 0
#define NRF_MAX_PAYLOAD 32

// PAYLOAD DEFINITIONS
#define BST_CONFIG_LENGTH 21

#define VSS_PAYLOAD_LENGTH 10
#define VSS_SPEED_LENGTH 4

#define SSL_PAYLOAD_LENGTH DEFAULT_PAYLOAD_LENGTH // 15 //
#define SSL_SPEED_LENGTH DEFAULT_PAYLOAD_LENGTH   // 12 //
#define POSITION_LENGTH DEFAULT_PAYLOAD_LENGTH    // 9 //
#define TELEMETRY_LENGTH DEFAULT_PAYLOAD_LENGTH   // 13 //
#define ODOMETRY_LENGTH DEFAULT_PAYLOAD_LENGTH    // 11 //

#define DEFAULT_PAYLOAD_LENGTH 20

#pragma pack(push, 1)

namespace robocin::comm {
  enum class MessageType {
    NONE = -1,
    BST_CONFIG,
    VSS_SPEED,
    SSL_SPEED,
    TELEMETRY,
    ODOMETRY,
    POSITION
  };

  template <class T, std::size_t N = DEFAULT_PAYLOAD_LENGTH>
  union Encodable {
    inline static constexpr std::size_t size() {
      return N;
    }

    [[nodiscard]] const char* c_str() const {
      return reinterpret_cast<const char*>(encoded);
    }

    std::byte encoded[size()]{};
    T decoded;

    Encodable() = default;
  };

  struct GenericPacketType {
    uint8_t typeMsg : 4;
    uint8_t id : 4;
    int64_t rest_a : 64;
    int64_t rest_b : 64;
    int64_t rest_c : 64;
    int64_t rest_d : 56;
  };

  using GenericPacket = Encodable<GenericPacketType, NRF_MAX_PAYLOAD>;

  struct BStConfigPacketType {
    uint8_t typeMsg : 4;
    uint8_t id : 4;
    bool duplex : 1;
    uint8_t team : 4;
    uint64_t addr1 : 64;
    uint64_t addr2 : 64;
    uint8_t payload : 8;
    uint8_t channel1 : 8;
    uint8_t channel2 : 8;
    uint8_t free : 3;
  };

  using BStConfigPacket = Encodable<BStConfigPacketType, BST_CONFIG_LENGTH>;

  struct VSSSpeedPacketType {
    uint8_t typeMsg : 4;
    uint8_t id : 4;
    int8_t leftSpeed : 8;
    int8_t rightSpeed : 8;
    uint8_t flags : 8;
  };

  using VSSSpeedPacket = Encodable<VSSSpeedPacketType, VSS_SPEED_LENGTH>;

  struct SSLSpeedPacketType {
    uint8_t typeMsg : 4;
    uint8_t id : 4;
    int32_t vx : 20;
    int32_t vy : 20;
    int32_t vw : 20;
    uint8_t front : 1;
    uint8_t chip : 1;
    uint8_t charge : 1;
    uint8_t strength : 8;
    uint8_t dribbler : 1;
    int16_t dribblerSpeed : 11;
    uint8_t command : 8;
    uint64_t free_1 : 61;
  };

  using SSLSpeedPacket = Encodable<SSLSpeedPacketType>;

  struct PositionPackType {
    uint8_t typeMsg : 4;
    uint8_t id : 4;
    int16_t x : 16; // -32.767 - 32.767 m
    int16_t y : 16; // -32.767 - 32.767 m
    int16_t w : 16; // 0 - 6.5535 rad
    // motion parameters
    uint16_t maxSpeed : 12; // 0 - 4.095 m/s
    uint16_t minSpeed : 12; // 0 - 4.095 m/s
    uint16_t rotateKp : 10; // 0 - 10.23
    uint8_t usingPropSpeed : 1;
    uint16_t minDistanceToPropSpeed : 12; // 0 - 4.095 m
    uint8_t clockwise : 1;
    uint16_t orbitRadius : 12; // 0 - 4.095 m
    uint16_t approachKp : 10;  // 0 - 10.23
    uint8_t positionType : 3;
    // Kick Options
    uint8_t front : 1;
    uint8_t chip : 1;
    uint8_t charge : 1;
    uint8_t strength : 8;
    uint8_t dribbler : 1;
    int16_t dribblerSpeed : 11;
    uint8_t command : 8;
  };

  using PositionPacket = Encodable<PositionPackType>;

  struct TelemetryPacketType {
    uint8_t typeMsg : 4;
    uint8_t id : 4;

    int16_t x : 16; // -32.767 - 32.767 m/s
    int16_t y : 16; // -32.767 - 32.767 m/s
    int16_t w : 16; // 0 - 6.5535 rad/s

    int16_t dribbler : 15; // -1638.3 - 1638.3 rad/s
    uint8_t kickLoad : 8;  // 0 - 2.55
    bool ball : 1;
    uint8_t battery : 8;

    int16_t m1 : 16;
    int16_t m2 : 16;
    int16_t m3 : 16;
    int16_t m4 : 16;

    uint8_t pcktCount : 8;
  };

  using TelemetryPacket = Encodable<TelemetryPacketType>;

  struct OdometryPacketType {
    uint8_t typeMsg : 4;
    uint8_t id : 4;
    int16_t x : 16;        // -32.767 - 32.767 m
    int16_t y : 16;        // -32.767 - 32.767 m
    int16_t w : 16;        // 0 - 6.5535 rad
    int16_t dribbler : 15; // -1638.3 - 1638.3 rad/s
    uint8_t kickLoad : 8;  // 0 - 2.55
    bool ball : 1;
    uint8_t battery : 8; // 0 - 25.5 V
    int16_t m1 : 16;     // -327.67 - 327.67 m/s
    int16_t m2 : 16;     // -327.67 - 327.67 m/s
    int16_t m3 : 16;     // -327.67 - 327.67 m/s
    int16_t m4 : 16;     // -327.67 - 327.67 m/s
    uint8_t pcktCount : 8;
  };

  using OdometryPacket = Encodable<OdometryPacketType>;
} // namespace robocin::comm

// restoring the standard alignment
#pragma pack(pop)

#endif // COMM_CONFIG_H
