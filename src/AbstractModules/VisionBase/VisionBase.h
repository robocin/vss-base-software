//
// Created by jose-cruz on 08/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_VISIONBASE_H
#define VSS_BASE_SOFTWARE_VISIONBASE_H

#include "Packages/Packages.h"

class VisionBase : public ModuleBase {
  Q_OBJECT

 public:
  explicit VisionBase(QThreadPool* threadPool);

 signals:
  void sendIsYellow(bool isYellow);
  void sendField(const Field& field);
  void sendVision(const Vision::Output& vision);
};

namespace Factory {
  inline static auto& vision = singleton_v<InheritanceFactory<VisionBase, QThreadPool*>>;
}

#endif // VSS_BASE_SOFTWARE_VISIONBASE_H
