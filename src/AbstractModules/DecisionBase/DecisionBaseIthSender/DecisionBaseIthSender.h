//
// Created by jose-cruz on 17/11/2021.
//

#ifndef VSS_UNIFICATION_DECISIONBASEITHSENDER_H
#define VSS_UNIFICATION_DECISIONBASEITHSENDER_H

#include <soccer-common/soccer-common.h>
#include "Packages/Packages.h"

class DecisionBaseIthSender : public QObject {
  Q_OBJECT

 public:
  explicit DecisionBaseIthSender(QObject* parent);

 signals:
  void sendDecision(const Decision::Play& play, const Decision::Output& plays);
};

#endif // VSS_UNIFICATION_DECISIONBASEITHSENDER_H
