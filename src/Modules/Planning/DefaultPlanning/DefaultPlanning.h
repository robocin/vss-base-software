//
// Created by jose-cruz on 06/08/2021.
//

#ifndef VSS_BASE_SOFTWARE_DEFAULTPLANNING_H
#define VSS_BASE_SOFTWARE_DEFAULTPLANNING_H

#include "Modules/Modules.h"

class DefaultPlanning : public PlanningBase {
 public:
  DefaultPlanning(int index, QThreadPool* threadPool);
  ~DefaultPlanning() override = default;

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

  struct Shared {
    SharedOptional<Behavior::Output> behavior;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::optional<Behavior::Output> behavior;

 public:
  Planning::Output operator()(const Behavior::OutputType::Planning& planning);
  Planning::Output operator()(const Behavior::OutputType::Navigation& navigation);

 private slots:
  void receiveBehavior(const Behavior::Output& rcvBehavior);
};

#endif // VSS_BASE_SOFTWARE_DEFAULTPLANNING_H
