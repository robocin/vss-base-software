//
// Created by jose-cruz on 13/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_BEHAVIORBASE_H
#define VSS_BASE_SOFTWARE_BEHAVIORBASE_H

#include "Packages/Packages.h"

class BehaviorBase : public IndexedModuleBase {
  Q_OBJECT
 public:
  BehaviorBase(int index, QThreadPool* threadPool);

 signals:
  void sendBehavior(const Behavior::Output& behavior);
};

namespace Factory {
  inline static auto& behavior = singleton_v<InheritanceFactory<BehaviorBase, int, QThreadPool*>>;
}

#endif // VSS_BASE_SOFTWARE_BEHAVIORBASE_H
