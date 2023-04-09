//
// Created by jose-cruz on 04/07/2021.
//

#ifndef VSS_UNIFICATION_WORLD_H
#define VSS_UNIFICATION_WORLD_H

#include "Field/Field.h"
#include "Packages/Frame/Frame.h"
#include "Packages/RefereeOutput/RefereeOutput.h"
#include "Packages/DecisionOutput/DecisionOutput.h"

class World {
  PROPERTY(bool, isYellow);
  PROPERTY(Field, field);
  PROPERTY(Frame, frame);
  PROPERTY(Referee::Output, referee);

 public:
  CTOR(World);

  [[nodiscard]] virtual inline bool has_world() const {
    return has_isYellow() && has_field() && has_frame() && has_referee();
  }
};

class PlayerWorld : public World {
  PROPERTY(Robot, robot);
  PROPERTY(Decision::Play, play);
  PROPERTY(QVector<Decision::Play>, plays, {});

 public:
  CTOR(PlayerWorld);

  [[nodiscard]] inline bool has_world() const override {
    return World::has_world() && has_robot() && has_play();
  }
};

#endif // VSS_UNIFICATION_WORLD_H
