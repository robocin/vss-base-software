//
// Created by jose-cruz on 12/09/2021.
//

#ifndef VSS_UNIFICATION_PLANNINGBASE_H
#define VSS_UNIFICATION_PLANNINGBASE_H

#include "Packages/Packages.h"

class PlanningBase : public IndexedModuleBase {
  Q_OBJECT

 public:
  PlanningBase(int index, QThreadPool* threadPool);

 signals:
  void sendPlanning(const Planning::Output& path);
};

namespace Factory {
  inline static auto& planning = singleton_v<InheritanceFactory<PlanningBase, int, QThreadPool*>>;
}

#endif // VSS_UNIFICATION_PLANNINGBASE_H
