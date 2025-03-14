//
// Created by jose-cruz on 06/08/2021.
//

#include "DefaultNavigation.h"
#include <queue>

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
  Point start = robot->position();
  Point ball = goToPoint.target();
  // Point ball = frame->ball().position();
  double grid = 100;
  double min_distance = 80;

  std::vector<Point> obstacles;
  for (const auto& enemy : frame->enemies()) {
    obstacles.push_back(enemy.position());
  }

  std::vector<Point> path = aStar(start, ball, grid, obstacles, min_distance);
  // algoritmo dougler peuker
  double tolereance = 50;
  path = douglasPeucker(path, tolereance);
  if (path.empty()) {
    return pid(goToPoint.target());
  }

  Point nextPoint = path[1];
  return pid(nextPoint);
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

double DefaultNavigation::heuristic(const Point& start, const Point& end) {
  return start.distTo(end);
}

std::vector<Point> DefaultNavigation::neighbors(const Point& node,
                                                double grid,
                                                const std::vector<Point>& enemies,
                                                double min_distance,
                                                const Point& destination) {
  std::vector<Point> result;
  std::vector<Point> directions = {Point(0, 1),
                                   Point(0, -1),
                                   Point(1, 0),
                                   Point(-1, 0),
                                   Point(1, 1),
                                   Point(-1, -1),
                                   Point(1, -1),
                                   Point(-1, 1)};
  for (const auto& direction : directions) {
    Point neighbor = node + direction * grid;
    if (neighbor.x() < 0 || neighbor.x() > 1500 || neighbor.y() < 0 || neighbor.y() > 1200) { // mm
      continue;
    }
    bool valid = true;
    for (const auto& enemy : enemies) {
      if (enemy.distTo(neighbor) < min_distance) {
        valid = false;
        break;
      }
    }
    if (!valid) {
      continue;
    }
    result.push_back(neighbor);
  }
  return result;
}

std::vector<Point> DefaultNavigation::aStar(const Point& start,
                                            const Point& destination,
                                            double grid,
                                            const std::vector<Point>& obstacles,
                                            double min_distance) {
  // Priority queue (menor f_cost primeiro)
  std::priority_queue<std::pair<double, Point>,
                      std::vector<std::pair<double, Point>>,
                      std::greater<std::pair<double, Point>>>
      p_queue;
  p_queue.push({0, start});

  std::map<Point, Point> came_from;
  std::map<Point, double> g_cost;
  std::map<Point, double> f_cost;

  g_cost[start] = 0;
  f_cost[start] = heuristic(start, destination);

  while (!p_queue.empty()) {
    Point current = p_queue.top().second;
    p_queue.pop();

    if (current.distTo(destination) < std::max(grid, min_distance * 0.5)) {
      std::vector<Point> path;
      while (came_from.find(current) != came_from.end()) {
        path.push_back(current);
        current = came_from[current];
      }
      path.push_back(start);
      std::reverse(path.begin(), path.end());
      return path;
    }

    for (const auto& neighbor : neighbors(current, grid, obstacles, min_distance, destination)) {
      double tentative_g_cost = g_cost[current] + heuristic(current, neighbor);
      if (g_cost.find(neighbor) == g_cost.end() || tentative_g_cost < g_cost[neighbor]) {
        came_from[neighbor] = current;
        g_cost[neighbor] = tentative_g_cost;
        f_cost[neighbor] = g_cost[neighbor] + heuristic(neighbor, destination);
        p_queue.push({f_cost[neighbor], neighbor});
      }
    }
  }
  return {};
}


std::vector<Point> DefaultNavigation::douglasPeucker(std::vector<Point> path, double epsilon) {
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
    std::vector<Point> rec1 = douglasPeucker(std::vector<Point>(path.begin(), path.begin() + index), epsilon);
    std::vector<Point> rec2 = douglasPeucker(std::vector<Point>(path.begin() + index, path.end()), epsilon);
    result.insert(result.end(), rec1.begin(), rec1.end() - 1);
    result.insert(result.end(), rec2.begin(), rec2.end());
  } else {
    result.push_back(path[0]);
    result.push_back(path[end]);
  }
  return result;

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