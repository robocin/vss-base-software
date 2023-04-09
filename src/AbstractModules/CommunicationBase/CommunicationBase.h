//
// Created by jose-cruz on 09/02/2021.
//

#ifndef VSS_UNIFICATION_COMMUNICATIONBASE_H
#define VSS_UNIFICATION_COMMUNICATIONBASE_H

#include <soccer-common/soccer-common.h>

class CommunicationBase : public ModuleBase {
  Q_OBJECT
 public:
  explicit CommunicationBase(QThreadPool* threadPool);

 signals:
  void sendDatagram(const QByteArray& datagram);
};

namespace Factory {
  inline static auto& communication =
      singleton_v<InheritanceFactory<CommunicationBase, QThreadPool*>>;
}

#endif // VSS_UNIFICATION_COMMUNICATIONBASE_H
