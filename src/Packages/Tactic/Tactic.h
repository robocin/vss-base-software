#ifndef VSS_BASE_SOFTWARE_TACTIC_H
#define VSS_BASE_SOFTWARE_TACTIC_H

#include "Role.h"

#include "Packages/BehaviorOutput/BehaviorOutput.h"
#include "Packages/PlanningOutput/PlanningOutput.h"
#include "Packages/NavigationOutput/NavigationOutput.h"

namespace TacticType {
  class Halt : public TacticOf<Role::Dynamic>, public Navigation::Output {
   public:
    inline explicit Halt(int id) : Navigation::Output(Navigation::Output::halt(id)) {
    }
  };

  class GoToPosition : public TacticOf<Role::Dynamic> {
    REQUIRED(Point, position);
    PROPERTY(double, angle, 0.0);

   public:
    CTOR(GoToPosition, position);
    CTOR(GoToPosition, position, angle);
  };

  class Guard : public TacticOf<Role::Goalkeeper> {
   public:
    CTOR(Guard);
  };

} // namespace TacticType

using Tactic = std::variant<TacticType::Halt,         //
                            TacticType::GoToPosition, //
                            TacticType::Guard>;

#endif // VSS_BASE_SOFTWARE_TACTIC_H
