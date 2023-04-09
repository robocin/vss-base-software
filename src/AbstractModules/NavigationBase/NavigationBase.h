//
// Created by jose-cruz on 12/09/2021.
//

#ifndef VSS_UNIFICATION_NAVIGATIONBASE_H
#define VSS_UNIFICATION_NAVIGATIONBASE_H

#include "Packages/Packages.h"

class NavigationBase : public IndexedModuleBase {
  Q_OBJECT

 public:
  NavigationBase(int index, QThreadPool* threadPool);

 signals:
  void sendNavigation(const Navigation::Output& navigation);
};

namespace Factory {
  inline static auto& navigation =
      singleton_v<InheritanceFactory<NavigationBase, int, QThreadPool*>>;
}

#endif // VSS_UNIFICATION_NAVIGATIONBASE_H
