#ifndef VSS_BASE_SOFTWARE_ENV_H
#define VSS_BASE_SOFTWARE_ENV_H

#include <soccer-common/soccer-common.h>

/*
Every environment parameter must be defined in this file, with the following rules:
  - The variable must be defined in the struct Env;
  - The variable must be public (default visibility);
  - The variable must be inline and static;
  - The variable must be SNAKE_UPPER_CASE;
  - The variable must be wrapped by Parameters::Atomic<T>;
  - The variable must be an enum or an arithmetic type;
    * (see: https://en.cppreference.com/w/cpp/types/is_arithmetic)
    * strings are not allowed :(

  - To modify these variables in runtime,
    put them in the method buildParameters of the Environment class:
    * "AbstractModules/Environment/Environment.h"

  - It's reasonable to group the variables in nested structs;

  Example:
    struct Env {
      inline static Parameters::Atomic<int> PARAMETER_A = 10;
      inline static Parameters::Atomic<float> PARAMETER_B = 20;

      struct Nested {
        inline static Parameters::Atomic<int> PARAMETER_C = 30;
        inline static Parameters::Atomic<double> PARAMETER_D = 40;
      };
    };
*/

struct Env {
  struct Robot {
    static inline Parameters::Atomic<bool> SEND_PWM = true;
    static inline Parameters::Atomic<bool> SEND_PWM_CALIBRATION_PRINTS = false;
    static inline Parameters::Atomic<double> WHEEL_RADIUS = 0.026;
  };
};

#endif // VSS_BASE_SOFTWARE_ENV_H