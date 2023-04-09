//
// Created by jose-cruz on 31/07/2021.
//

#ifndef VSS_BASE_SOFTWARE_REFEREEPARSER_H
#define VSS_BASE_SOFTWARE_REFEREEPARSER_H

#include <protobufs/protobufs.h>
#include "Packages/RefereeOutput/RefereeOutput.h"

namespace Referee {
  class Parser {
    using Foul = VSSRef::Foul;
    using Color = VSSRef::Color;
    using Command = VSSRef::ref_to_team::VSSRef_Command;
    using FoulToStateFunction = std::function<State(const Command&, bool)>;

    inline static State freeKickFoul(const Command&, bool) {
      // TODO: This foul was not implemented by the referee.
      return StateType::Halt{};
    }

    inline static State stopFoul(const Command&, bool) {
      return StateType::Halt{};
    }

    inline static State penaltyKickFoul(const Command& command, bool isYellow) {
      if ((command.teamcolor() == Color::YELLOW) == isYellow) {
        return StateType::PrepareToKickPenalty{};
      } else {
        return StateType::PrepareToDefendPenalty{};
      }
    }

    inline static State goalKickFoul(const Command& command, bool isYellow) {
      if ((command.teamcolor() == Color::YELLOW) == isYellow) {
        return StateType::PrepareToKickGoalKick{};
      } else {
        return StateType::PrepareToDefendGoalKick{};
      }
    }

    inline static State freeBallFoul(const Command& command, bool) {
      return StateType::PrepareToFreeBall{static_cast<Field::Quadrant>(command.foulquadrant())};
    }

    inline static State kickoffFoul(const Command& command, bool isYellow) {
      if ((command.teamcolor() == Color::YELLOW) == isYellow) {
        return StateType::PrepareToKickKickoff{};
      } else {
        return StateType::PrepareToDefendKickoff{};
      }
    }

    inline static State gameOn(const Command&, bool) {
      return StateType::GameRunning{};
    }

    inline static std::unordered_map<Foul, FoulToStateFunction> foulToState = {
        {Foul::FREE_KICK, &freeKickFoul},
        {Foul::PENALTY_KICK, &penaltyKickFoul},
        {Foul::GOAL_KICK, &goalKickFoul},
        {Foul::FREE_BALL, &freeBallFoul},
        {Foul::KICKOFF, &kickoffFoul},
        {Foul::STOP, &stopFoul},
        {Foul::GAME_ON, &gameOn}};

   public:
    inline static Output parse(const Command& command, bool isYellow) {
      auto stage = static_cast<Stage>(command.gamehalf());
      if (auto it = foulToState.find(command.foul()); it != foulToState.end()) {
        const FoulToStateFunction& foulHandler = it->second;
        return Output{stage, foulHandler(command, isYellow), command.timestamp()};
      } else {
        return Output{stage, StateType::Halt{}, command.timestamp()};
      }
    }
  }; // namespace Referee
} // namespace Referee

#endif // VSS_BASE_SOFTWARE_REFEREEPARSER_H
