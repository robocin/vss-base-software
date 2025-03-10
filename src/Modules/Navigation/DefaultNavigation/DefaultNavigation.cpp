//
// Created by jose-cruz on 06/08/2021.
//

#include "DefaultNavigation.h"

DefaultNavigation::DefaultNavigation(int index, QThreadPool* threadPool) :
    NavigationBase(index, threadPool) {
}

void DefaultNavigation::buildParameters(Parameters::Handler& parameters) {
  parameters["kP"] = Parameters::DoubleSpinBox(args.kP, 0, 100.0); // TODO: fix range
  parameters["kD"] = Parameters::DoubleSpinBox(args.kD, 0, 100.0); // TODO: fix range
  parameters["LK"] = Parameters::DoubleSpinBox(args.LK, 0, 100.0); // TODO: fix range
  parameters["RK"] = Parameters::DoubleSpinBox(args.RK, 0, 100.0); // TODO: fix range
}

void DefaultNavigation::connectModules(const Modules* modules) {
  connect(modules->processedVision()->ithSender(index()),
          &ProcessedVisionBaseIthSender::sendFrameAndRobot,
          this,
          &DefaultNavigation::receiveFrameAndRobot,
          Qt::DirectConnection);

  connect(modules->planning()[index()],
          &PlanningBase::sendPlanning,
          this,
          &DefaultNavigation::receivePlanning,
          Qt::DirectConnection);
}

void DefaultNavigation::init(const Modules* modules) {
  targetKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Bottom);
}

void DefaultNavigation::update() {
  shared->frame.extract_to(frame);
  shared->robot.extract_to(robot);
  shared->planning.extract_to(planning);

  if (args.drawTarget.updated()) {
    targetKey.setVisibility(args.drawTarget);
  }
}

void DefaultNavigation::exec() {
  if (!frame || !robot || !planning) {
    return;
  }
  emit sendNavigation(std::visit(*this, planning.value()));
}

Navigation::Output
DefaultNavigation::operator()(const Planning::OutputType::Navigation& navigation) {
  return navigation;
}

Navigation::Output DefaultNavigation::operator()(const Planning::OutputType::Command& command) {
  return std::visit(*this, command.motion());
}

Navigation::Output DefaultNavigation::pid(const Point& target) {
  bool reversed = false;

  double robotAngle = robot->angle();
  double angleToTarget = (target - robot->position()).angle();
  double error = Geometry::smallestAngleDiff(angleToTarget, robotAngle);

  if (std::abs(error) > PI / 2.0 + PI / 20.0) {
    reversed = true;
    robotAngle = Geometry::normalizeInPI(robotAngle + PI);
    // Calculates the error and reverses the front of the robot
    error = Geometry::smallestAngleDiff(angleToTarget, robotAngle);
  }

  double motorSpeed =
      (args.kP * error) + (args.kD * (error - pidArgs.lastError)); // + 0.2 * sumErr;
  pidArgs.lastError = error;

  double baseSpeed = 30;

  // Normalize
  motorSpeed = motorSpeed > 30 ? 30 : motorSpeed;
  motorSpeed = motorSpeed < -30 ? -30 : motorSpeed;

  double rightMotorSpeed;
  double leftMotorSpeed;

  if (motorSpeed > 0) {
    leftMotorSpeed = baseSpeed;
    rightMotorSpeed = baseSpeed - motorSpeed;
  } else {
    leftMotorSpeed = baseSpeed + motorSpeed;
    rightMotorSpeed = baseSpeed;
  }

  if (reversed) {
    if (motorSpeed > 0) {
      leftMotorSpeed = -baseSpeed + motorSpeed;
      rightMotorSpeed = -baseSpeed;
    } else {
      leftMotorSpeed = -baseSpeed;
      rightMotorSpeed = -baseSpeed - motorSpeed;
    }
  }
  return makeNavigationOutput(leftMotorSpeed, rightMotorSpeed);
}

Navigation::Output DefaultNavigation::operator()(const Motion::GoToPoint& goToPoint) {
  return pid(goToPoint.target());
}
Navigation::Output DefaultNavigation::operator()(const Motion::Spin& spin) {
  const auto [left, right] = spin.clockwise() ? std::pair{1, -1} : std::pair{-1, 1};
  return makeNavigationOutput(spin.power() * left, spin.power() * right);
}
Navigation::Output DefaultNavigation::operator()(const Motion::WheelsRad& WheelsRad) {
  return makeNavigationOutput(WheelsRad.ls(), WheelsRad.rs());
}

Navigation::Output DefaultNavigation::makeNavigationOutput(double ls, double rs) {
  return {robot->id(), ls, rs};
}

void DefaultNavigation::receivePlanning(const Planning::Output& planning) {
  shared->planning = planning;
  runInParallel();
}

void DefaultNavigation::receiveFrameAndRobot(const Frame& frame, const Robot& robot) {
  shared->frame = frame;
  shared->robot = robot;
}

static_block {
  Factory::navigation.insert<DefaultNavigation>();
};