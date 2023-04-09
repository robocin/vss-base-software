//
// Created by jose-cruz on 19/07/2021.
//

#ifndef VSS_BASE_SOFTWARE_ENTITIES_H
#define VSS_BASE_SOFTWARE_ENTITIES_H

#include "Robot/Robot.h"

using RawBall = Common::RawBall<Point>;
using Ball = Common::Ball<Point>;

template <class R>
using Robots = Common::Robots<R>;

#endif // VSS_BASE_SOFTWARE_ENTITIES_H
