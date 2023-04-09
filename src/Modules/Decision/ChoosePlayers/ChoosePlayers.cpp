#include "ChoosePlayers.h"

void ChoosePlayers::extendedInit(const Modules* modules) {
}

std::optional<Robot> ChoosePlayers::takeGoalkeeper(Robots<Robot>& allies) const {
  if (auto it = allies.closestTo(world.field().allyGoalInsideCenter()); it != allies.end()) {
    auto goalkeeper = std::move(*it);
    allies.erase(it);
    return goalkeeper;
  }
  return std::nullopt;
}

std::optional<Robot> ChoosePlayers::takeDefender(Robots<Robot>& allies, Point allyPoint) const {
  if (auto it = allies.closestTo(allyPoint); it != allies.end()) {
    auto defender = std::move(*it);
    allies.erase(it);
    return defender;
  }
  return std::nullopt;
}

std::optional<Robot>
ChoosePlayers::takeAttacker(Robots<Robot>& allies, Robots<Robot>& enemies, Ball ball) const {
  // TODO: Implement this function.
  return std::nullopt;
}