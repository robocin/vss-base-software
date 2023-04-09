#ifndef VSS_UNIFICATION_ENVIRONMENT_H
#define VSS_UNIFICATION_ENVIRONMENT_H

#include <soccer-common/soccer-common.h>
#include "Env/Env.h"

class Environment : public ModuleBase {
 public:
  inline explicit Environment(QThreadPool* threadPool) : ModuleBase(threadPool) {
  }

 private:
  inline void buildParameters(Parameters::Handler& parameters) override {
    parameters["Robot"]["SEND_PWM"] = Parameters::CheckBox(Env::Robot::SEND_PWM);
    parameters["Robot"]["SEND_PWM_CALIBRATION_PRINTS"] =
        Parameters::CheckBox(Env::Robot::SEND_PWM_CALIBRATION_PRINTS);
    parameters["Robot"]["WHEEL_RADIUS"] =
        Parameters::DoubleSpinBox(Env::Robot::WHEEL_RADIUS, 0.001, 1.000, 3);
  }
  inline void exec() override {
  }
};

namespace Factory {
  inline static auto& environment = singleton_v<InheritanceFactory<Environment, QThreadPool*>>;
}

#endif // VSS_UNIFICATION_ENVIRONMENT_H
