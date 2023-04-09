#ifndef VSS_BASE_SOFTWARE_HALTSM_H
#define VSS_BASE_SOFTWARE_HALTSM_H

#include "Modules/Behavior/Player/PlayerUtils/PlayerUtils.h"

class HaltSM : public Behavior::PlayerExtension {
 public:
  template <class M>
  inline HaltSM(M* m, const PlayerWorld& world) : Behavior::PlayerExtension(this, m, world) {
  }

  inline Behavior::Output operator()(const TacticType::Halt&) {
    return Behavior::OutputType::Navigation::halt(index());
  }
};

#endif // VSS_BASE_SOFTWARE_HALTSM_H
