#ifndef VSS_BASE_SOFTWARE_DECISIONUTILS_H
#define VSS_BASE_SOFTWARE_DECISIONUTILS_H

#include <protobufs/protobufs.h>

#include "Packages/Packages.h"

namespace Concepts {
  namespace detail {
    template <class T>
    struct is_variant : std::false_type {};

    template <class... Args>
    struct is_variant<std::variant<Args...>> : std::true_type {};

    template <class T>
    inline constexpr bool is_variant_v = is_variant<T>::value;
  } // namespace detail

  template <class V>
  concept Variant = detail::is_variant_v<V>;
}; // namespace Concepts

namespace Decision {
  class Extension : public ModuleExtension {
   public:
    template <class E, class D>
    inline Extension(E* e, D* decision, const World& world) :
        ModuleExtension(e, decision),
        world(world),
        teleportSender([decision](const QByteArray& datagram) {
          decision->sendTeleport(datagram);
        }) {
    }

    void sendTeleport(const QByteArray& datagram) {
      teleportSender(datagram);
    }

    virtual void clear() {
    }

   protected:
    const World& world;

   private:
    std::function<void(const QByteArray&)> teleportSender;
  };

  template <Concepts::Variant V, std::derived_from<ModuleExtension>... Es>
  class Extensions {
    template <size_t N>
    static constexpr bool dependent_false_v = N > 0;

    template <class State,
              std::derived_from<ModuleExtension> Functor,
              std::derived_from<ModuleExtension>... Functors>
    static constexpr auto PointerToMatchingFunctorHelper() {
      if constexpr (requires(Functor f, State state) { f.operator()(state); }) {
        return (Functor*) nullptr;
      } else if constexpr (sizeof...(Functors) > 0) {
        return PointerToMatchingFunctorHelper<State, Functors...>();
      } else {
        static_assert(dependent_false_v<sizeof...(Functors)>);
      }
    }

    template <class State, std::derived_from<ModuleExtension>... Functors>
    using MatchingFunctor = std::remove_pointer_t<
        std::invoke_result_t<decltype(PointerToMatchingFunctorHelper<State, Functors...>)>>;

    V last;
    overloaded_visitor_t<Es...> visitor;

   public:
    template <class D>
    Extensions(D* decision, const World& world) requires(std::default_initializable<V>) :
        last{},
        visitor(decision, world) {
    }

    template <class D>
    Extensions(const V& state, D* decision, const World& world) :
        last{state},
        visitor(decision, world) {
    }

    inline decltype(auto) operator()(const V& state) {
      auto output = std::visit(visitor, state);
      if (last.index() != state.index()) {
        clear();
      }
      last = state;
      return output;
    }

    inline void clear() {
      std::visit(
          [&](auto&& state) {
            using Cleanable = MatchingFunctor<std::decay_t<decltype(state)>, Es...>;
            if constexpr (requires(Cleanable c) { c.clear(); }) {
              static_cast<Cleanable&>(visitor).clear();
            }
          },
          last);
    }
  };

  struct ArgRobot {
    Parameters::Arg<real_t> x;
    Parameters::Arg<real_t> y;
    Parameters::Arg<real_t> angle;

    inline ArgRobot(real_t x, real_t y, real_t angle) : x(x), y(y), angle(angle) {
    }

    [[nodiscard]] inline Point position(bool isAttackingToRight) const {
      Point position = {x.value(), y.value()};
      return isAttackingToRight ? position : -position;
    }

    [[nodiscard]] inline QPair<Point, real_t> positionAndAngle(bool isAttackingToRight) const {
      return {position(isAttackingToRight),
              isAttackingToRight ? angle.value() : Geometry::normalizeInPI(angle.value() + PI)};
    }

    [[nodiscard]] inline Parameters::Handler handler(real_t min, real_t max) {
      using namespace Parameters;
      Handler handler;
      handler["x"] = DoubleSpinBox(x, min, max);
      handler["y"] = DoubleSpinBox(y, min, max);
      handler["angle"] = MappedAngleInRadiansToDegrees(angle);
      return handler;
    }
  };

  template <std::same_as<ArgRobot>... Args>
  QVector<RawRobot> getFormation(const World& world,
                                 const Args&... args) requires(sizeof...(Args) >= 3) {
    using PositionAndAngle = QPair<Point, real_t>;

    bool isAttackingToRight = world.field().isAttackingToRight();

    QVector<PositionAndAngle> targets;
    targets.reserve(sizeof...(args));
    (targets.push_back(args.positionAndAngle(isAttackingToRight)), ...);

    auto allies = world.frame().allies();

    auto matches = Hungarian::match(allies, targets);

    QVector<RawRobot> result;
    result.reserve(matches.size());
    for (const auto& [robot, positionAndAngle] : matches) {
      const auto& [position, angle] = positionAndAngle;
      result.emplace_back(robot.id(), angle, position);
    }
    return result;
  }

  inline QByteArray toTeleportDatagram(const World& world, const QVector<RawRobot>& formation) {
    using WorldType = VSSRef::Frame;
    using PacketType = VSSRef::team_to_ref::VSSRef_Placement;
    using TeamColor = VSSRef::Color;

    static constexpr double M_TO_MM = 1000.0;

    WorldType w;

    w.set_teamcolor(world.isYellow() ? TeamColor::YELLOW : TeamColor::BLUE);

    for (auto&& robot : formation) {
      auto r = w.add_robots();
      r->set_robot_id(robot.id());
      r->set_x(robot.x() / M_TO_MM);
      r->set_y(robot.y() / M_TO_MM);
      r->set_orientation(Geometry::radiansToDegrees(robot.angle()));
    }

    PacketType packet;
    packet.set_allocated_world(new WorldType(std::move(w)));

    return Protobuf::toByteArray(packet);
  }

  inline std::vector<Point> discretizeEllipse(Point center, int greater, int minor) {
    int segments = std::max((int) std::floor(std::sqrt(((greater + minor) / 2) * 20)), 8);
    float angle_shift = 2 * M_PI / segments, phi = 0;
    std::vector<Point> vertices;
    for (int i = 0; i < segments; ++i) {
      phi += angle_shift;
      vertices.push_back(center + Point(greater * std::cos(phi), minor * std::sin(phi)));
    }

    return vertices;
  }
  static constexpr Field fieldBlue = Field::field(Field::Size::Middle, true);
  static constexpr Field fieldYellow = Field::field(Field::Size::Middle, false);
  inline std::vector<Point> blueEllipse() {
    auto ellipse = discretizeEllipse(fieldBlue.allyGoalInsideCenter(),
                                     fieldBlue.goalWidth(),
                                     fieldBlue.penaltyAreaDepth());
    std::vector<Point> myPoints;
    for (Point point : ellipse) {
      if (point.x() > -fieldBlue.length() / 2)
        myPoints.push_back(point);
    }
    std::sort(myPoints.begin(), myPoints.end(), [](const Point& a, const Point& b) {
      return a.y() > b.y();
    });
    return myPoints;
  }
  inline std::vector<Point> yellowEllipse() {
    auto ellipse = discretizeEllipse(fieldYellow.allyGoalInsideCenter(),
                                     fieldYellow.goalWidth(),
                                     fieldYellow.penaltyAreaDepth());
    std::vector<Point> myPoints;
    for (Point point : ellipse) {
      if (point.x() < fieldYellow.length() / 2)
        myPoints.push_back(point);
    }
    std::sort(myPoints.begin(), myPoints.end(), [](const Point& a, const Point& b) {
      return a.y() > b.y();
    });
    return myPoints;
  }

  inline std::pair<Point, int> getEllipsePoint(Point point, std::vector<Point> ellipse) {
    double minorDist = 19999999;
    Point closestPoint = ellipse[0];
    int index = 0;
    for (int i = 0; i < ellipse.size(); ++i) {
      if (ellipse[i].distTo(point) < minorDist) {
        minorDist = ellipse[i].distTo(point);
        closestPoint = ellipse[i];
        index = i;
      }
    }
    return std::pair<Point, int>{closestPoint, index};
  }

  inline std::pair<Point, double> defensePointAngle(Field field, Ball& ball) {
    auto defPoints =
        (field.isAttackingToRight()) ? Decision::blueEllipse() : Decision::yellowEllipse();
    Point futureBall = ball + ball.velocity() * 0.08;
    double futureBallY = ((ball.x() > 0 && field.isAttackingToRight()) ||
                          (ball.x() < 0 && !field.isAttackingToRight())) ?
                             0 :
                             futureBall.y();
    futureBall.setY(futureBallY);
    auto closestTocurve = Decision::getEllipsePoint(futureBall, defPoints);
    Point target = closestTocurve.first;
    int index = closestTocurve.second;
    if (index == defPoints.size() - 1) {
      index = index - 1;
    } else {
      index = index + 1;
    }
    double angle = target.angleTo(defPoints[index]);
    return std::pair<Point, double>{target, angle};
  }

  static Point goaliePoint(Field field, Ball& ball) {
    double posx = field.allyGoalOutsideCenter().x();
    posx += (field.isAttackingToRight()) ? -10 : 10;
    double poxy = ball.y();
    poxy = std::clamp(poxy, field.allyGoalInsideBottom().y(), field.allyGoalInsideTop().y());

    Point pos(posx, poxy);
    return pos;
  }
} // namespace Decision

#endif // VSS_BASE_SOFTWARE_DECISIONUTILS_H
