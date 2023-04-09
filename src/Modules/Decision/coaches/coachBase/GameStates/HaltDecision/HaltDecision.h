#ifndef VSS_BASE_SOFTWARE_HALTDECISION_H
#define VSS_BASE_SOFTWARE_HALTDECISION_H

#include "Modules/Decision/DecisionUtils/DecisionUtils.h"

class HaltDecision : public Decision::Extension {
 public:
  template <class M>
  inline HaltDecision(M* m, const World& world) : Decision::Extension(this, m, world) {
  }

  inline Decision::Output operator()(const Referee::StateType::Halt&) {
    const QVector<Robot>& allies = world.frame().allies();

    Decision::Output output;

    for (const Robot& ally : allies) {
      const int id = ally.id();
      output += Decision::Play(id, TacticType::Halt(id));
    }

    return output;
  }
};

#endif // VSS_BASE_SOFTWARE_HALTDECISION_H
