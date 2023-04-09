#include "MouseAndKeyboard.h"

MouseAndKeyboard::MouseAndKeyboard(int index, QThreadPool* threadPool) :
    BehaviorBase(index, threadPool) {
}

void MouseAndKeyboard::buildParameters(Parameters::Handler& parameters) {
  parameters["Key"]["GoToPoint"] =
      Parameters::ComboBox(args.goToPointKey, MagicEnum::values<Qt::Key>());
  parameters["Key"]["CCWSpin"] = Parameters::ComboBox(args.ccwSpin, MagicEnum::values<Qt::Key>());
  parameters["Key"]["CWSpin"] = Parameters::ComboBox(args.cwSpin, MagicEnum::values<Qt::Key>());
}

void MouseAndKeyboard::connectModules(const Modules* modules) {
  connect(modules->processedVision()->ithSender(index()),
          &ProcessedVisionBaseIthSender::sendFrameAndRobot,
          this,
          &MouseAndKeyboard::receiveFrameAndRobot,
          Qt::DirectConnection);

  connect(modules->vision(),
          &VisionBase::sendField,
          this,
          &MouseAndKeyboard::receiveField,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::relativeMousePos,
          this,
          &MouseAndKeyboard::receiveMousePos,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::onKeyPress,
          this,
          &MouseAndKeyboard::receiveKeyPressed,
          Qt::DirectConnection);

  connect(modules->gui()->gameVisualizer(),
          &GameVisualizer::onKeyRelease,
          this,
          &MouseAndKeyboard::receiveKeyReleased,
          Qt::DirectConnection);
}

void MouseAndKeyboard::init(const Modules* modules) {
  targetKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Top);
}

void MouseAndKeyboard::update() {
  shared->field.extract_to(field);
  if (auto frameAndRobot = shared->frameAndRobot.get_optional_and_reset()) {
    std::tie(frame, robot) = *frameAndRobot;
  }
  shared->mouse.extract_to(mouse);
}

void MouseAndKeyboard::exec() {
  if (!field || !frame || !robot || !mouse) {
    return;
  }
  targetKey.draw([mouse = this->mouse](GameVisualizerPainter2D* f) {
    f->drawFilledCircle(mouse.value(), 15, Color::Red);
  });

  const QSet<Qt::Key> keys = shared->keys.get();

  if (keys.contains(args.goToPointKey)) {
    emit sendBehavior(Behavior::OutputType::Command(Motion::GoToPoint(mouse.value())));
  } else if (keys.contains(args.ccwSpin)) {
    emit sendBehavior(Behavior::OutputType::Command(Motion::Spin(false)));
  } else if (keys.contains(args.cwSpin)) {
    emit sendBehavior(Behavior::OutputType::Command(Motion::Spin(true)));
  } else {
    emit sendBehavior(Navigation::Output::halt(index()));
  }
}

void MouseAndKeyboard::receiveField(const Field& field) {
  shared->field = field;
}

void MouseAndKeyboard::receiveFrameAndRobot(const Frame& frame, const Robot& robot) {
  shared->frameAndRobot = std::tie(frame, robot);
  runInParallel();
}

void MouseAndKeyboard::receiveMousePos(const Point& mouse) {
  shared.apply([&mouse](Shared& obj) {
    if (!obj.keys->empty()) {
      obj.mouse = mouse;
    }
  });
}

void MouseAndKeyboard::receiveKeyPressed(Qt::Key key) {
  shared->keys->insert(key);
}

void MouseAndKeyboard::receiveKeyReleased(Qt::Key key) {
  shared->keys->remove(key);
}

static_block {
  Factory::behavior.insert<MouseAndKeyboard>();
};