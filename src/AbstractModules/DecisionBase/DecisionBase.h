//
// Created by jose-cruz on 11/09/2021.
//

#ifndef VSS_UNIFICATION_DECISIONBASE_H
#define VSS_UNIFICATION_DECISIONBASE_H

#include "DecisionBaseIthSender/DecisionBaseIthSender.h"

class DecisionBase : public ModuleBase, public IthSender<DecisionBaseIthSender> {
  Q_OBJECT

 public:
  explicit DecisionBase(QThreadPool* threadPool);
};

namespace Factory {
  inline static auto& decision = singleton_v<InheritanceFactory<DecisionBase, QThreadPool*>>;
}

#endif // VSS_UNIFICATION_DECISIONBASE_H
