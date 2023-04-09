//
// Created by jose-cruz on 17/07/2021.
//

#include "CommunicationBSt.h"

CommunicationBSt::CommunicationBSt(QThreadPool* threadPool) : CommunicationBase(threadPool) {
}

void CommunicationBSt::connectModules(const Modules* modules) {
  for (NavigationBase* navigation : modules->navigation()) {
    connect(navigation,
            &NavigationBase::sendNavigation,
            this,
            &CommunicationBSt::receiveNavigation,
            Qt::DirectConnection);
  }
}

void CommunicationBSt::init(const Modules* modules) {
  // Increase hertz not decrease receive time between packages in robot, frequency optimized for
  // capability of base-station sending, require reduce package size to try this.
  modules->timers()->hertz<120>()->callOnTimeout(this,
                                                 &CommunicationBSt::runInParallel,
                                                 Qt::DirectConnection);
}

void CommunicationBSt::update() {
  BstService::instance().setup();

  commands = shared.apply([](Shared& obj) -> QVector<Navigation::Output> {
    QVector<Navigation::Output> result;
    for (SharedOptional<Navigation::Output>& command : obj.commands) {
      if (command.has_value()) {
        result.emplace_back(command.value());
      }
    }
    return result;
  });
}

void CommunicationBSt::exec() {

  for (auto&& command : commands) {
    // direct velocity control
    BstService::instance().send(BaseStation::toRobotControl(command));
  }
}

void CommunicationBSt::receiveNavigation(const Navigation::Output& navigation) {
  shared->commands[navigation.id()] = navigation;
}

static_block {
  Factory::communication.insert<CommunicationBSt>();
};