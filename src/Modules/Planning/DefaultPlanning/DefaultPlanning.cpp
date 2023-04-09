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
}

void DefaultPlanning::init(const Modules* modules) {
}

void DefaultPlanning::update() {
  shared->behavior.extract_to(behavior);
}

void DefaultPlanning::exec() {
  if (!behavior) {
    return;
  }
  emit sendPlanning(std::visit(*this, behavior.value()));
}

Planning::Output DefaultPlanning::operator()(const Behavior::OutputType::Planning& planning) {
  return planning;
}

Planning::Output DefaultPlanning::operator()(const Behavior::OutputType::Navigation& navigation) {
  return navigation;
}

void DefaultPlanning::receiveBehavior(const Behavior::Output& rcvBehavior) {
  shared->behavior = rcvBehavior;
  runInParallel();
}

static_block {
  Factory::planning.insert<DefaultPlanning>();
};