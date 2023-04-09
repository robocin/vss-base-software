//
// Created by jose-cruz on 12/06/2021.
//

#include "ProcessedVisionBase.h"

ProcessedVisionBase::ProcessedVisionBase(QThreadPool* threadPool) :
    ModuleBase(threadPool),
    IthSender<ProcessedVisionBaseIthSender>(this) {
}
