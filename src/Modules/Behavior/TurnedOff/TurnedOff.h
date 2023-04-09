//
// Created by jose-cruz on 06/08/2021.
//

#ifndef VSS_BASE_SOFTWARE_TURNEDOFF_H
#define VSS_BASE_SOFTWARE_TURNEDOFF_H

#include "Modules/Modules.h"

class TurnedOff : public BehaviorBase {
 public:
  TurnedOff(int index, QThreadPool* threadPool);
  ~TurnedOff() override = default;

  void connectModules(const Modules* modules) override;
  void exec() override;
};

#endif // VSS_BASE_SOFTWARE_TURNEDOFF_H
