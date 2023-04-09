//
// Created by jose-cruz on 08/02/2021.
//

#ifndef VSS_UNIFICATION_THREADPOOLS_H
#define VSS_UNIFICATION_THREADPOOLS_H

#include <QThreadPool>

class ThreadPools {
  mutable QThreadPool m_mainThreadPool;

 public:
  ThreadPools() = default;
  QThreadPool* mainThreadPool() const;
};

#endif // VSS_UNIFICATION_THREADPOOLS_H
