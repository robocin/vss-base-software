//
// Created by jose-cruz on 19/07/2021.
//

#ifndef VSS_BASE_SOFTWARE_FIELD_H
#define VSS_BASE_SOFTWARE_FIELD_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"

#include <soccer-common/soccer-common.h>

/*!
 * @brief Field inherits from Common::Field<Point>.
 */
class Field : public Common::Field<Point> {
 public:
  constexpr Field(bool isAttackingToRight,
                  real_t length,
                  real_t width,
                  real_t goalDepth,
                  real_t goalWidth,
                  real_t penaltyAreaDepth,
                  real_t penaltyAreaWidth,
                  real_t freeBallMarkOffsetX,
                  real_t freeBallMarkOffsetY) :
      Common::Field<Point>(isAttackingToRight,
                           length,
                           width,
                           goalDepth,
                           goalWidth,
                           penaltyAreaDepth,
                           penaltyAreaWidth),
      m_freeBallMarkOffsetX(freeBallMarkOffsetX),
      m_freeBallMarkOffsetY(freeBallMarkOffsetY) {
  }

  [[nodiscard]] constexpr real_t freeBallMarkOffsetX() const {
    return m_freeBallMarkOffsetX;
  }

  [[nodiscard]] constexpr real_t freeBallMarkOffsetY() const {
    return m_freeBallMarkOffsetY;
  }

  [[nodiscard]] constexpr Point leftFreeKickMark() const {
    return Point{-m_freeBallMarkOffsetX, 0};
  }

  [[nodiscard]] constexpr Point rightFreeKickMark() const {
    return Point{+m_freeBallMarkOffsetX, 0};
  }

  [[nodiscard]] constexpr Point allyFreeKickMark() const {
    return isAttackingToRight() ? leftFreeKickMark() : rightFreeKickMark();
  }

  [[nodiscard]] constexpr Point enemyFreeKickMark() const {
    return isAttackingToRight() ? rightFreeKickMark() : leftFreeKickMark();
  }

  enum class Quadrant : uint8_t {
    Unspecified, // equivalent to VSSRef::Quadrant::NO_QUADRANT
    TopRight,    // equivalent to VSSRef::Quadrant::QUADRANT_1
    TopLeft,     // equivalent to VSSRef::Quadrant::QUADRANT_2
    BottomLeft,  // equivalent to VSSRef::Quadrant::QUADRANT_3
    BottomRight  // equivalent to VSSRef::Quadrant::QUADRANT_4
  };

  [[nodiscard]] inline Point freeBallMark(const Quadrant& quadrant) const {
    switch (quadrant) {
      case Quadrant::TopRight: {
        return Point{m_freeBallMarkOffsetX, m_freeBallMarkOffsetY};
      }
      case Quadrant::TopLeft: {
        return Point{-m_freeBallMarkOffsetX, m_freeBallMarkOffsetY};
      }
      case Quadrant::BottomLeft: {
        return Point{-m_freeBallMarkOffsetX, -m_freeBallMarkOffsetY};
      }
      case Quadrant::BottomRight: {
        return Point{m_freeBallMarkOffsetX, -m_freeBallMarkOffsetY};
      }
      default: {
        return center();
      }
    }
  }

  enum class Wall : uint8_t {
    NoWall = 0u,
    Top = 1u << 0u,
    Bottom = 1u << 1u,
    Left = 1u << 2u,
    Right = 1u << 3u,
    BottomLeft = Bottom | Left,
    BottomRight = Bottom | Right,
    TopLeft = Top | Left,
    TopRight = Top | Right
  };

  [[nodiscard]] inline Wall isNearToWall(const Point& point, real_t radius) const {
    Wall walls = Wall::NoWall;

    if (point.y() - radius < min().y()) {
      walls = static_cast<Wall>(static_cast<uint8_t>(walls) | static_cast<uint8_t>(Wall::Bottom));
    }

    if (point.y() + radius > max().y()) {
      walls = static_cast<Wall>(static_cast<uint8_t>(walls) | static_cast<uint8_t>(Wall::Top));
    }

    if (!(rightGoalOutsideBottom().y() <= point.y() && point.y() <= rightGoalOutsideTop().y())) {
      if (point.x() - radius < min().x()) {
        walls = static_cast<Wall>(static_cast<uint8_t>(walls) | static_cast<uint8_t>(Wall::Left));
      }

      if (point.x() + radius > max().x()) {
        walls = static_cast<Wall>(static_cast<uint8_t>(walls) | static_cast<uint8_t>(Wall::Right));
      }
    }

    return walls;
  }

  enum class Size : uint8_t {
    Unspecified, // decays to Small
    Small,       // 3x3
    Middle,      // 5x5
    Large        // 11x11
  };

  template <Size S>
  static constexpr Field field(bool isAttackingToRight);
  static constexpr Field field(Size size, bool isAttackingToRight);

 private:
  real_t m_freeBallMarkOffsetX;
  real_t m_freeBallMarkOffsetY;
};

template <>
constexpr Field Field::field<Field::Size::Small>(bool isAttackingToRight) {
  return Field{isAttackingToRight, 1500, 1300, 100, 400, 150, 700, 375, 400};
}

template <>
constexpr Field Field::field<Field::Size::Middle>(bool isAttackingToRight) {
  return Field{isAttackingToRight, 2200, 1800, 150, 400, 350, 800, 550, 600};
}

template <>
constexpr Field Field::field<Field::Size::Large>(bool isAttackingToRight) {
  return Field{isAttackingToRight, 4000, 2800, 200, 600, 600, 1200, 1000, 800};
}

constexpr Field Field::field(Field::Size size, bool isAttackingToRight) {
  switch (size) {
    case Size::Small: {
      return field<Size::Small>(isAttackingToRight);
    }
    case Size::Middle: {
      return field<Size::Middle>(isAttackingToRight);
    }
    case Size::Large: {
      return field<Size::Large>(isAttackingToRight);
    }
    default: {
      return field<Size::Small>(isAttackingToRight);
    }
  }
}

#pragma clang diagnostic pop

#endif // VSS_BASE_SOFTWARE_FIELD_H
