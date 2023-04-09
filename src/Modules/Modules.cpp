//
// Created by jose-cruz on 08/02/2021.
//

#include "Modules.h"

Modules::Modules(MainWindow* gui, ThreadPools* threads) : ModulesPrivate(gui) {
  makeModule(this, m_environment, Factory::environment, threads->mainThreadPool());

  makeModule(this, m_vision, Factory::vision, threads->mainThreadPool());
  makeModule(this, m_processedVision, Factory::processedVision, threads->mainThreadPool());
  makeModule(this, m_decision, Factory::decision, threads->mainThreadPool());
  makeModule(this, m_communication, Factory::communication, threads->mainThreadPool());
  makeModule(this, m_referee, Factory::referee, threads->mainThreadPool());

  makeModule(this, m_behavior, Factory::behavior, threads->mainThreadPool());
  makeModule(this, m_planning, Factory::planning, threads->mainThreadPool());
  makeModule(this, m_navigation, Factory::navigation, threads->mainThreadPool());
}

VisionBase* Modules::vision() const {
  return m_vision;
}

ProcessedVisionBase* Modules::processedVision() const {
  return m_processedVision;
}

DecisionBase* Modules::decision() const {
  return m_decision;
}

CommunicationBase* Modules::communication() const {
  return m_communication;
}

RefereeBase* Modules::referee() const {
  return m_referee;
}

const QVector<BehaviorBase*>& Modules::behavior() const {
  return m_behavior;
}

const QVector<PlanningBase*>& Modules::planning() const {
  return m_planning;
}

const QVector<NavigationBase*>& Modules::navigation() const {
  return m_navigation;
}