#ifndef VSS_BASE_SOFTWARE_CHOOSEPLAYERS_H
#define VSS_BASE_SOFTWARE_CHOOSEPLAYERS_H

#include "Modules/Modules.h"

class ChoosePlayers : public ModuleExtension {
  const World& world;

 public:
  template <class M>
  ChoosePlayers(M* m, const World& world) : ModuleExtension(this, m), world(world) {
  }

  struct Args {
    Parameters::Arg<int> framesToPredict = 5;
  };
  Args args;

  inline void extendedBuildParameters(Parameters::Handler& parameters) override {
    parameters["Frames to predict"] = Parameters::Text<int>(args.framesToPredict);
  }

  void extendedInit(const Modules* modules) override;

  [[nodiscard]] std::optional<Robot> takeGoalkeeper(Robots<Robot>& allies) const;
  [[nodiscard]] std::optional<Robot> takeDefender(Robots<Robot>& allies, Point allyPoint) const;
  [[nodiscard]] std::optional<Robot>
  takeAttacker(Robots<Robot>& allies, Robots<Robot>& enemies, Ball ball) const;
};

#endif // VSS_BASE_SOFTWARE_CHOOSEPLAYERS_H
