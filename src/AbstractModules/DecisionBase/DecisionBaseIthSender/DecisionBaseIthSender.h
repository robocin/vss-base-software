//
// Created by jose-cruz on 17/11/2021.
//

#ifndef VSS_BASE_SOFTWARE_DECISIONBASEITHSENDER_H
#define VSS_BASE_SOFTWARE_DECISIONBASEITHSENDER_H

#include <soccer-common/soccer-common.h>
#include "Packages/Packages.h"

class DecisionBaseIthSender : public QObject {
  Q_OBJECT

 public:
  explicit DecisionBaseIthSender(QObject* parent);

 signals:
  void sendDecision(const Decision::Play& play, const Decision::Output& plays);
};

#endif // VSS_BASE_SOFTWARE_DECISIONBASEITHSENDER_H
