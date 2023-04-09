//
// Created by jose-cruz on 09/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_FRAME_H
#define VSS_BASE_SOFTWARE_FRAME_H

#include "Entities/Entities.h"

class Frame {
  REQUIRED(Ball, ball);
  REQUIRED(Robots<Robot>, allies);
  REQUIRED(Robots<Robot>, enemies);

 public:
  CTOR(Frame, ball, allies, enemies);
};

#endif // VSS_BASE_SOFTWARE_FRAME_H
