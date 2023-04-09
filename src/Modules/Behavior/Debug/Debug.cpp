#include "Debug.h"

Debug::Debug(int index, QThreadPool* threadPool) : BehaviorBase(index, threadPool) {
}

void Debug::buildParameters(Parameters::Handler& parameters) {
  parameters["General"]["DebugMode"] =
      Parameters::ComboBox(args.debugMode, MagicEnum::values<DebugMode>());
  parameters["ManualPackage"]["wLeftWheel"] =
      Parameters::DoubleSpinBox(args.manualPackage.wLeftWheel,
                                -100,
                                100,
                                2,
                                "Left Wheel Angular Speed (Radians)");
  parameters["ManualPackage"]["wRightWheel"] =
      Parameters::DoubleSpinBox(args.manualPackage.wRightWheel,
                                -100,
                                100,
                                2,
                                "Right Wheel Angular Speed (Radians)");
}

void Debug::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &VisionBase::sendIsYellow,
          this,
          &Debug::receiveIsYellow,
          Qt::DirectConnection);
  connect(modules->vision(),
          &VisionBase::sendField,
          this,
          &Debug::receiveField,
          Qt::DirectConnection);
  connect(modules->processedVision()->ithSender(index()),
          &ProcessedVisionBaseIthSender::sendFrameAndRobot,
          this,
          &Debug::receiveFrameAndRobot,
          Qt::DirectConnection);
  connect(modules->referee(),
          &RefereeBase::sendReferee,
          this,
          &Debug::receiveReferee,
          Qt::DirectConnection);
}

void Debug::init(const Modules* modules) {
}

void Debug::update() {
  shared->field.extract_to(field);
  if (auto frameAndRobot = shared->frameAndRobot.get_optional_and_reset()) {
    std::tie(frame, robot) = *frameAndRobot;
  }
}

void Debug::exec() {
  if (!field || !frame || !robot) {
    return;
  }
  // TODO: Use referee to play pause?!
  if (args.debugMode == DebugMode::ManualPackage) {
    emit sendBehavior(manualPackage());
  } else {
    emit sendBehavior(Behavior::OutputType::Navigation::halt(index()));
  }
}

Behavior::Output Debug::manualPackage() {
  return Behavior::OutputType::Command(
      Motion::WheelsRad(args.manualPackage.wLeftWheel, args.manualPackage.wRightWheel));
}

void Debug::receiveIsYellow(bool rcvdIsYellow) {
  shared->hasIsYellow = rcvdIsYellow;
}

void Debug::receiveField(const Field& rcvdField) {
  shared->field = rcvdField;
}

void Debug::receiveReferee(const Referee::Output& rcvdReferee) {
  shared->referee = rcvdReferee;
}

void Debug::receiveFrameAndRobot(const Frame& rcvdFrame, const Robot& rcvdRobot) {
  shared->frameAndRobot = std::tuple(rcvdFrame, rcvdRobot);
  runInParallel();
}

static_block {
  Factory::behavior.insert<Debug>();
};