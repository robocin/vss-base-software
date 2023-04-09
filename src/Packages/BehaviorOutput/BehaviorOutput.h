//
// Created by jose-cruz on 04/08/2021.
//

#ifndef VSS_UNIFICATION_BEHAVIOROUTPUT_H
#define VSS_UNIFICATION_BEHAVIOROUTPUT_H

#include "Packages/RobotCommand/RobotCommand.h"
#include "Packages/PlanningOutput/PlanningOutput.h"
#include "Packages/NavigationOutput/NavigationOutput.h"

namespace Behavior {
  namespace OutputType {
    using Command = RobotCommand;
    using Planning = Planning::Output;
    using Navigation = Navigation::Output;
  } // namespace OutputType

  using Output = std::variant<OutputType::Command, OutputType::Planning, OutputType::Navigation>;
} // namespace Behavior

#endif // VSS_UNIFICATION_BEHAVIOROUTPUT_H
