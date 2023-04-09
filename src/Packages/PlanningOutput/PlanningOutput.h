//
// Created by jose-cruz on 06/08/2021.
//

#ifndef VSS_UNIFICATION_PLANNINGOUTPUT_H
#define VSS_UNIFICATION_PLANNINGOUTPUT_H

#include "Packages/RobotCommand/RobotCommand.h"
#include "Packages/NavigationOutput/NavigationOutput.h"

namespace Planning {
  namespace OutputType {
    using Command = RobotCommand;
    using Navigation = Navigation::Output;
  } // namespace OutputType

  using Output = std::variant<OutputType::Command, OutputType::Navigation>;
} // namespace Planning

#endif // VSS_UNIFICATION_PLANNINGOUTPUT_H
