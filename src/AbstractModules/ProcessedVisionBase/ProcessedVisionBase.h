//
// Created by jose-cruz on 12/06/2021.
//

#ifndef VSS_UNIFICATION_PROCESSEDVISIONBASE_H
#define VSS_UNIFICATION_PROCESSEDVISIONBASE_H

#include "ProcessedVisionBaseIthSender/ProcessedVisionBaseIthSender.h"

class ProcessedVisionBase : public ModuleBase, public IthSender<ProcessedVisionBaseIthSender> {
  Q_OBJECT
 public:
  explicit ProcessedVisionBase(QThreadPool* threadPool);

 signals:
  void sendFrame(const Frame& frame);
};

namespace Factory {
  inline static auto& processedVision =
      singleton_v<InheritanceFactory<ProcessedVisionBase, QThreadPool*>>;
}

#endif // VSS_UNIFICATION_PROCESSEDVISIONBASE_H
