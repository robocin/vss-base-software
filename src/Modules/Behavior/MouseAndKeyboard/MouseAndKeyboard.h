#ifndef VSS_BASE_SOFTWARE_MOUSEANDKEYBOARD_H
#define VSS_BASE_SOFTWARE_MOUSEANDKEYBOARD_H

#include "Modules/Modules.h"

class MouseAndKeyboard : public BehaviorBase {
 public:
  MouseAndKeyboard(int index, QThreadPool* threadPool);

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  struct Args {
    Parameters::Arg<Qt::Key> goToPointKey = Qt::Key_T;
    Parameters::Arg<Qt::Key> ccwSpin = Qt::Key_Z;
    Parameters::Arg<Qt::Key> cwSpin = Qt::Key_X;
  };
  Args args;

  struct Shared {
    SharedOptional<std::tuple<Frame, Robot>> frameAndRobot;
    SharedOptional<Robot> robot;
    SharedOptional<Point> mouse;
    SharedOptional<Field> field;
    SharedValue<QSet<Qt::Key>> keys;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::optional<Field> field;
  std::optional<Frame> frame;
  std::optional<Robot> robot;
  std::optional<Point> mouse;

  GameVisualizer::Key targetKey;

 private slots:
  void receiveField(const Field& field);
  void receiveFrameAndRobot(const Frame& frame, const Robot& robot);
  void receiveMousePos(const Point& mouse);
  void receiveKeyPressed(Qt::Key key);
  void receiveKeyReleased(Qt::Key key);
};

#endif // VSS_BASE_SOFTWARE_MOUSEANDKEYBOARD_H
