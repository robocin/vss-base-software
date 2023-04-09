//
// Created by jose-cruz on 08/02/2021.
//

#include "ThreadPools.h"

QThreadPool* ThreadPools::mainThreadPool() const {
  return &m_mainThreadPool;
}
