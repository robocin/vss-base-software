//
// Created by jose-cruz on 06/08/2021.
//

#include "TurnedOff.h"

TurnedOff::TurnedOff(int index, QThreadPool* threadPool) : BehaviorBase(index, threadPool) {
}

void TurnedOff::connectModules(const Modules* modules) {
  modules->timers()->hertz<1>()->callOnTimeout(this, &TurnedOff::runInParallel);
}

void TurnedOff::exec() {
  emit sendBehavior(Navigation::Output::halt(index()));
}

static_block {
  Factory::behavior.insert<TurnedOff>();
};