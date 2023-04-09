//
// Created by jose-cruz on 06/08/2021.
//

#ifndef VSS_UNIFICATION_NAVIGATIONOUTPUT_H
#define VSS_UNIFICATION_NAVIGATIONOUTPUT_H

#include <soccer-common/soccer-common.h>

namespace Navigation {
  class Output {
    REQUIRED(int, id);
    PROPERTY(double, leftWheel, 0);
    PROPERTY(double, rightWheel, 0);

    CTOR(Output, id);

   public:
    CTOR(Output, id, leftWheel, rightWheel);

    static inline Output halt(int id) {
      return Output(id);
    }
  };
} // namespace Navigation

#endif // VSS_UNIFICATION_NAVIGATIONOUTPUT_H
