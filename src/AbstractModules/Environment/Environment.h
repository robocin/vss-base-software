#ifndef VSS_BASE_SOFTWARE_ENVIRONMENT_H
#define VSS_BASE_SOFTWARE_ENVIRONMENT_H

#include <soccer-common/soccer-common.h>
#include "Env/Env.h"

class Environment : public ModuleBase {
 public:
  inline explicit Environment(QThreadPool* threadPool) : ModuleBase(threadPool) {
  }

 private:
  inline void buildParameters(Parameters::Handler& parameters) override {
  }
  inline void exec() override {
  }
};

namespace Factory {
  inline static auto& environment = singleton_v<InheritanceFactory<Environment, QThreadPool*>>;
}

#endif // VSS_BASE_SOFTWARE_ENVIRONMENT_H
