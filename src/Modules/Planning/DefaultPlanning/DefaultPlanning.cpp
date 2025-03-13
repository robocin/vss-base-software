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


void DefaultPlanning::exec() {
  if (!behavior) {
    return;
  }
  emit sendPlanning(std::visit(*this, behavior.value()));
}


std::vector<Point> DefaultPlanning::Douglas_Peucker(std::vector<Point> path, double epsilon) {
  std::vector<Point> result;
  double dmax = 0;
  int index = 0;
  int end = path.size() - 1; //narrowing conversion
  for (int i = 1; i < end; i++) {
    double d = Geometry2D::distancePointLine(path[0], path[end], path[i]); 
    if (d > dmax) {
      index = i;
      dmax = d;
    }
  }
  if (dmax > epsilon) {
    std::vector<Point> rec1 = Douglas_Peucker(std::vector<Point>(path.begin(), path.begin() + index), epsilon);
    std::vector<Point> rec2 = Douglas_Peucker(std::vector<Point>(path.begin() + index, path.end()), epsilon);
    result.insert(result.end(), rec1.begin(), rec1.end() - 1);
    result.insert(result.end(), rec2.begin(), rec2.end());
  } else {
    result.push_back(path[0]);
    result.push_back(path[end]);
  }
  return result;

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