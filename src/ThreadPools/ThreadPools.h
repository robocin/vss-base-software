//
// Created by jose-cruz on 08/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_THREADPOOLS_H
#define VSS_BASE_SOFTWARE_THREADPOOLS_H

#include <QThreadPool>

class ThreadPools {
  mutable QThreadPool m_mainThreadPool;

 public:
  ThreadPools() = default;
  QThreadPool* mainThreadPool() const;
};

#endif // VSS_BASE_SOFTWARE_THREADPOOLS_H
