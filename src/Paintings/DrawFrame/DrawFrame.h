#ifndef VSS_BASE_SOFTWARE_DRAWFRAME_H
#define VSS_BASE_SOFTWARE_DRAWFRAME_H

#include "Packages/Packages.h"

class DrawFrame : public Painting {
  static constexpr real_t AXIS = 75;

  class DrawingRobot {
    REQUIRED(int, id);
    REQUIRED(Point, position);
    REQUIRED(real_t, angle);
    REQUIRED(QColor, teamColor);

   public:
    CTOR(DrawingRobot, id, position, angle, teamColor);

    inline void drawRobot(GameVisualizerPainter2D* f) const {
      QPolygonF body(QRectF(-AXIS / 2, -AXIS / 2, AXIS, AXIS));
      for (auto& pt : body) {
        pt = Geometry2D::rotateCCW(pt, angle());
      }
      real_t eps = AXIS / 11;
      QPolygonF primaryTag(QRectF(-AXIS / 2 + eps, -AXIS / 2 + eps, 4 * eps, 9 * eps));
      for (auto& pt : primaryTag) {
        pt = Geometry2D::rotateCCW(pt, angle());
      }

      QPolygonF secondaryTag(QRectF(-AXIS / 2 + 6 * eps, -AXIS / 2 + eps, 4 * eps, 9 * eps));
      for (auto& pt : secondaryTag) {
        pt = Geometry2D::rotateCCW(pt, angle());
      }

      QPolygonF secondSecondaryTag(
          QRectF(-AXIS / 2 + 6 * eps, -AXIS / 12 + eps, 4 * eps, 4.5 * eps));
      for (auto& pt : secondSecondaryTag) {
        pt = Geometry2D::rotateCCW(pt, angle());
      }

      f->drawFilledPolygon(body.translated(position()), Color::Black);
      f->drawFilledPolygon(primaryTag.translated(position()), teamColor());
      {
        QColor color = Color::Black;
        static constexpr std::array colorById{Color::Red,
                                              Color::Green,
                                              Color::Magenta,
                                              Color::LightBlue,
                                              Color::Purple};
        size_t index = static_cast<size_t>(id()) / 3;
        if (index < 4) {
          color = colorById[index];
        } else {
          color = colorById[4];
        }
        f->drawFilledPolygon(secondaryTag.translated(position()), color);
      }
      {
        QColor color = Color::Black;
        static constexpr std::array colorById{Color::Green,
                                              Color::Magenta,
                                              Color::LightBlue,
                                              Color::Red,
                                              Color::Magenta,
                                              Color::LightBlue,
                                              Color::Red,
                                              Color::Green,
                                              Color::LightBlue,
                                              Color::Red,
                                              Color::Green,
                                              Color::Magenta};
        if (static_cast<size_t>(id()) < colorById.size()) {
          color = colorById[id()];
        }
        f->drawFilledPolygon(secondSecondaryTag.translated(position()), color);
      }
      f->drawFilledCircle(position(), AXIS / 4, Color::Black);
      f->drawLine(position(),
                  secondSecondaryTag.translated(position())[1],
                  Color::Black,
                  AXIS / 10);
      f->drawText(QString::number(id()), position(), AXIS / 2, Color::White);
    }
  };

  class DrawingBall {
    REQUIRED(Point, position);

   public:
    CTOR(DrawingBall, position);

    inline explicit DrawingBall(const Ball& ball) : DrawingBall(ball.position()) {
    }

    inline void drawBall(GameVisualizerPainter2D* f) const {
      f->drawFilledCircle(position(), 21, Color::DarkOrange);
      f->drawFilledCircle(position(), 15, Color::Orange);
    }
  };

  QVector<DrawingBall> balls;
  QVector<DrawingRobot> robots;

 public:
  inline explicit DrawFrame(const Vision::OutputType::Raw& raw, bool isYellow) {
    for (auto&& frame : raw) {
      for (auto&& ball : frame.balls()) {
        balls.emplace_back(ball.position());
      }

      for (auto&& ally : frame.allies()) {
        robots.emplace_back(ally.id(),                             // id
                            ally.position(),                       // position
                            ally.angle(),                          // angle
                            isYellow ? Color::Yellow : Color::Blue // teamColor
        );
      }

      for (auto&& enemy : frame.enemies()) {
        robots.emplace_back(enemy.id(),                            // id
                            enemy.position(),                      // position
                            enemy.angle(),                         // angle
                            isYellow ? Color::Blue : Color::Yellow // teamColor
        );
      }
    }
  }

  inline explicit DrawFrame(const Vision::OutputType::Tracked& tracked, bool isYellow) {
    if (tracked.has_ball()) {
      balls.emplace_back(tracked.ball());
    }

    for (auto&& ally : tracked.allies()) {
      robots.emplace_back(ally.id(),                             // id
                          ally.position(),                       // position
                          ally.angle(),                          // angle
                          isYellow ? Color::Yellow : Color::Blue // teamColor
      );
    }

    for (auto&& enemy : tracked.enemies()) {
      robots.emplace_back(enemy.id(),                            // id
                          enemy.position(),                      // position
                          enemy.angle(),                         // angle
                          isYellow ? Color::Blue : Color::Yellow // teamColor
      );
    }
  }

  inline explicit DrawFrame(const Vision::Output& vision, bool isYellow) :
      DrawFrame(std::visit(
          [isYellow](auto&& rawOrTracked) {
            return DrawFrame(rawOrTracked, isYellow);
          },
          vision)) {
  }

  inline explicit DrawFrame(const Frame& frame, bool isYellow) {
    balls.emplace_back(frame.ball().position());

    for (auto&& ally : frame.allies()) {
      robots.emplace_back(ally.id(),                             // id
                          ally.position(),                       // position
                          ally.angle(),                          // angle
                          isYellow ? Color::Yellow : Color::Blue // teamColor
      );
    }

    for (auto&& enemy : frame.enemies()) {
      robots.emplace_back(enemy.id(),                            // id
                          enemy.position(),                      // position
                          enemy.angle(),                         // angle
                          isYellow ? Color::Blue : Color::Yellow // teamColor
      );
    }
  }

  inline void run(GameVisualizerPainter2D* f) override {
    for (const auto& ball : balls) {
      ball.drawBall(f);
    }

    for (const auto& robot : robots) {
      robot.drawRobot(f);
    }
  }

  [[nodiscard]] inline std::unique_ptr<Painting> clone() const override {
    return std::make_unique<DrawFrame>(*this);
  }
};

#endif // VSS_BASE_SOFTWARE_DRAWFRAME_H
