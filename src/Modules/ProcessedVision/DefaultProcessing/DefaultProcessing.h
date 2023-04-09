//
// Created by jose-cruz on 12/06/2021.
//

#ifndef VSS_UNIFICATION_DEFAULTPROCESSING_H
#define VSS_UNIFICATION_DEFAULTPROCESSING_H

#include "Modules/Modules.h"

class DefaultProcessing : public ProcessedVisionBase {
 public:
  explicit DefaultProcessing(QThreadPool* threadPool);
  ~DefaultProcessing() override = default;

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  struct Args {};
  Args args;

  struct Shared {
    SharedOptional<Vision::Output> vision;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::optional<Vision::Output> vision;
  std::optional<Frame> lastFrame;

 private slots:
  void receiveVision(const Vision::Output& vision);

 public:
  std::optional<Frame> operator()(const Vision::OutputType::Raw& raw);
  std::optional<Frame> operator()(const Vision::OutputType::Tracked& tracked);
};

#endif // VSS_UNIFICATION_DEFAULTPROCESSING_H
