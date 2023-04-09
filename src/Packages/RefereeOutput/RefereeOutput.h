//
// Created by jose-cruz on 03/08/2021.
//

#ifndef VSS_UNIFICATION_REFEREEOUTPUT_H
#define VSS_UNIFICATION_REFEREEOUTPUT_H

#include <soccer-common/soccer-common.h>
#include "Field/Field.h"

namespace Referee {
  namespace StateType {
    class Halt {};

    class GameRunning {};

    class PrepareToKickFreeKick {};
    class PrepareToDefendFreeKick {};

    class PrepareToKickPenalty {};
    class PrepareToDefendPenalty {};

    class PrepareToKickGoalKick {};
    class PrepareToDefendGoalKick {};

    class PrepareToFreeBall {
      REQUIRED(Field::Quadrant, quadrant);

     public:
      CTOR(PrepareToFreeBall, quadrant);
    };

    class PrepareToKickKickoff {};
    class PrepareToDefendKickoff {};
  } // namespace StateType

  enum class Stage : uint8_t {
    Inactive,  //
    FirstHalf, //
    SecondHalf //
  };

  using State = std::variant<StateType::Halt,
                             StateType::GameRunning,
                             // StateType::PrepareToKickFreeKick,
                             // StateType::PrepareToDefendFreeKick,
                             StateType::PrepareToKickPenalty,
                             StateType::PrepareToDefendPenalty,
                             StateType::PrepareToKickGoalKick,
                             StateType::PrepareToDefendGoalKick,
                             StateType::PrepareToFreeBall,
                             StateType::PrepareToKickKickoff,
                             StateType::PrepareToDefendKickoff>;

  class Output {
    REQUIRED(Stage, stage);
    REQUIRED(State, state);
    REQUIRED(real_t, timestamp);

   public:
    CTOR(Output, stage, state, timestamp);
  };
} // namespace Referee

#endif // VSS_UNIFICATION_REFEREEOUTPUT_H
