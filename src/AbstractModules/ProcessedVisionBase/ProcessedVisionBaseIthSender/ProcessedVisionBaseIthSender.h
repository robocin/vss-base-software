//
// Created by jose-cruz on 12/06/2021.
//

#ifndef VSS_BASE_SOFTWARE_PROCESSEDVISIONBASEITHSENDER_H
#define VSS_BASE_SOFTWARE_PROCESSEDVISIONBASEITHSENDER_H

#include "Packages/Packages.h"

class ProcessedVisionBaseIthSender : public QObject {
  Q_OBJECT
 public:
  explicit ProcessedVisionBaseIthSender(QObject* parent);

 signals:
  void sendFrameAndRobot(const Frame& frame, const Robot& robot);
};

#endif // VSS_BASE_SOFTWARE_PROCESSEDVISIONBASEITHSENDER_H
