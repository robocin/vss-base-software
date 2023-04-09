//
// Created by jose-cruz on 08/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_MODULES_H
#define VSS_BASE_SOFTWARE_MODULES_H

#include "Utils/Utils.h"
#include "Packages/Packages.h"
#include "ThreadPools/ThreadPools.h"
#include "AbstractModules/AbstractModules.h"

class Modules : public ModulesPrivate {
  Environment* m_environment{};

  VisionBase* m_vision{};
  ProcessedVisionBase* m_processedVision{};
  DecisionBase* m_decision{};
  CommunicationBase* m_communication{};
  RefereeBase* m_referee{};

  QVector<BehaviorBase*> m_behavior{};
  QVector<PlanningBase*> m_planning{};
  QVector<NavigationBase*> m_navigation{};

 public:
  explicit Modules(MainWindow* gui, ThreadPools* threads);
  ~Modules() override = default;

  VisionBase* vision() const;
  ProcessedVisionBase* processedVision() const;
  DecisionBase* decision() const;
  CommunicationBase* communication() const;
  RefereeBase* referee() const;

  const QVector<BehaviorBase*>& behavior() const;
  const QVector<PlanningBase*>& planning() const;
  const QVector<NavigationBase*>& navigation() const;
};

#endif // VSS_BASE_SOFTWARE_MODULES_H
