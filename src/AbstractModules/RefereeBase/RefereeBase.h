//
// Created by jp on 7/17/21.
//

#ifndef VSS_BASE_SOFTWARE_REFEREEBASE_H
#define VSS_BASE_SOFTWARE_REFEREEBASE_H

#include "Packages/Packages.h"

class RefereeBase : public ModuleBase {
  Q_OBJECT

 public:
  explicit RefereeBase(QThreadPool* threadPool);

 signals:
  void sendReferee(const Referee::Output& referee);
};

namespace Factory {
  inline static auto& referee =
      Singleton<InheritanceFactory<RefereeBase, QThreadPool*>>::instance();
}
#endif // VSS_BASE_SOFTWARE_REFEREEBASE_H
