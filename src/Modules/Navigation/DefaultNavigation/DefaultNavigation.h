//
// Created by jose-cruz on 06/08/2021.
//

#ifndef VSS_BASE_SOFTWARE_DEFAULTNAVIGATION_H
#define VSS_BASE_SOFTWARE_DEFAULTNAVIGATION_H

#include "Modules/Modules.h"

class DefaultNavigation : public NavigationBase {
 public:
  DefaultNavigation(int index, QThreadPool* threadPool);
  ~DefaultNavigation() override = default;

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

  struct Args {
    Parameters::Arg<double> kP = 20.0;
    Parameters::Arg<double> kD = 2.5;
    Parameters::Arg<double> RK = 1.0;
    Parameters::Arg<double> LK = 1.0;

    Parameters::Arg<bool> drawTarget = false;
  };
  Args args;

  struct Shared {
    SharedOptional<Frame> frame;
    SharedOptional<Robot> robot;
    SharedOptional<Planning::Output> planning;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::optional<Frame> frame;
  std::optional<Robot> robot;
  std::optional<Planning::Output> planning;

  GameVisualizer::Key targetKey;

  struct PIDArgs {
    double lastError = 0;
  };
  PIDArgs pidArgs;

  Navigation::Output pid(const Point& target);

 public:
  Navigation::Output operator()(const Planning::OutputType::Navigation& navigation);
  Navigation::Output operator()(const Planning::OutputType::Command& command);
  Navigation::Output operator()(const Motion::GoToPoint& goToPoint);
  Navigation::Output operator()(const Motion::Spin& spin);
  Navigation::Output operator()(const Motion::WheelsRad& WheelsRad);
  // Functions for A* algorithm
  //  function heuristics cost estimate
  double heuristic(const Point& start, const Point& end);
  // function to return a list of neighboring points
  std::vector<Point> neighbors(const Point& node,
                               double grid,
                               const std::vector<Point>& enemies,
                               double min_distance,
                               const Point& destination);
  // function to find the shortest path
  std::vector<Point> aStar(const Point& start,
                           const Point& end,
                           double grid,
                           const std::vector<Point>& enemies,
                           double min_distance);

 private slots:
  Navigation::Output makeNavigationOutput(double ls, double rs);
  void receivePlanning(const Planning::Output& planning);
  void receiveFrameAndRobot(const Frame& frame, const Robot& robot);
};

#endif // VSS_BASE_SOFTWARE_DEFAULTNAVIGATION_H