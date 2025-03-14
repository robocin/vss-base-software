//
// Created by jose-cruz on 06/08/2021.
//

#include "DefaultPlanning.h"

DefaultPlanning::DefaultPlanning(int index, QThreadPool* threadPool) :
    PlanningBase(index, threadPool) {
}

void DefaultPlanning::buildParameters(Parameters::Handler& parameters) {
}

void DefaultPlanning::connectModules(const Modules* modules) {
  connect(modules->behavior()[index()],
          &BehaviorBase::sendBehavior,
          this,
          &DefaultPlanning::receiveBehavior,
          Qt::DirectConnection);
  connect(modules->processedVision()->ithSender(index()),
          &ProcessedVisionBaseIthSender::sendFrameAndRobot,
          this,
          &DefaultPlanning::receiveFrameAndRobot,
          Qt::DirectConnection);
}

void DefaultPlanning::init(const Modules* modules) {
}

void DefaultPlanning::update() {
  shared->behavior.extract_to(behavior);
  shared->frame.extract_to(frame);
  shared->robot.extract_to(robot);
}

void DefaultPlanning::exec() {
  if (!behavior) {
    return;
  }
  emit sendPlanning(std::visit(*this, behavior.value()));
}

// Implement here
Planning::Output DefaultPlanning::operator()(const Behavior::OutputType::Planning& planning) {

  RobotCommand command = std::get<RobotCommand>(planning);
  if (!std::holds_alternative<Motion::GoToPoint>(command.motion())) {
    return planning;
  }

  Motion::GoToPoint goToPoint = std::get<Motion::GoToPoint>(command.motion());

  // Point target = goToPoint.target();
  // Point robotPosition = shared->robot.value().position();
  // Point ball = frame->ball().position();
  // auto newGoToPoint = Motion::GoToPoint(target);
  // auto newGoToPoint = Motion::GoToPoint(Point(0, 0));

  return Behavior::OutputType::Command(goToPoint);
}

Planning::Output DefaultPlanning::operator()(const Behavior::OutputType::Navigation& navigation) {
  return navigation;
}

void DefaultPlanning::receiveBehavior(const Behavior::Output& rcvBehavior) {
  shared->behavior = rcvBehavior;
  runInParallel();
}

void DefaultPlanning::receiveFrameAndRobot(const Frame& frame, const Robot& robot) {
  shared->frame = frame;
  shared->robot = robot;
}

static_block {
  Factory::planning.insert<DefaultPlanning>();
};