//
// Created by jose-cruz on 12/08/2021.
//

#ifndef VSS_BASE_SOFTWARE_VISIONOUTPUT_H
#define VSS_BASE_SOFTWARE_VISIONOUTPUT_H

#include "Packages/RawFrame/RawFrame.h"
#include "Packages/Frame/Frame.h"

namespace Vision {
  namespace OutputType {
    using Raw = QVector<RawFrame>;
    class Tracked {
      PROPERTY(Ball, ball);
      PROPERTY(Robots<Robot>, allies, Robots<Robot>{});
      PROPERTY(Robots<Robot>, enemies, Robots<Robot>{});

     public:
      CTOR(Tracked);
    };
  } // namespace OutputType

  using Output = std::variant<OutputType::Raw, OutputType::Tracked>;
} // namespace Vision

#endif // VSS_BASE_SOFTWARE_VISIONOUTPUT_H
