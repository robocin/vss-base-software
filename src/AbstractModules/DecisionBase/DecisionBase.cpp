//
// Created by jose-cruz on 19/02/2021.
//

#include "DecisionBase.h"

DecisionBase::DecisionBase(QThreadPool* threadPool) :
    ModuleBase(threadPool),
    IthSender<DecisionBaseIthSender>(this) {
}
