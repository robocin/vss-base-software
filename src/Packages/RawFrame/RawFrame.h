//
// Created by jose-cruz on 09/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_RAWFRAME_H
#define VSS_BASE_SOFTWARE_RAWFRAME_H

#include "Entities/Entities.h"

class RawFrame {
  PROPERTY(int, number);
  PROPERTY(int, cameraId);
  PROPERTY(QVector<RawBall>, balls, QVector<RawBall>());
  PROPERTY(Robots<RawRobot>, allies, Robots<RawRobot>());
  PROPERTY(Robots<RawRobot>, enemies, Robots<RawRobot>());

 public:
  CTOR(RawFrame);
  CTOR(RawFrame, number, cameraId, balls, allies, enemies);
};

#endif // VSS_BASE_SOFTWARE_RAWFRAME_H
