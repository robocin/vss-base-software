//
// Created by jose-cruz on 06/08/2021.
//

#include "Player.h"

Player::Player(int index, QThreadPool* threadPool) :
    BehaviorBase(index, threadPool),
    overloaded_visitor_t(this, world) {
}

void Player::buildParameters(Parameters::Handler&) {
}

void Player::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &VisionBase::sendIsYellow,
          this,
          &Player::receiveIsYellow,
          Qt::DirectConnection);
  connect(modules->vision(),
          &VisionBase::sendField,
          this,
          &Player::receiveField,
          Qt::DirectConnection);
  connect(modules->processedVision()->ithSender(index()),
          &ProcessedVisionBaseIthSender::sendFrameAndRobot,
          this,
          &Player::receiveFrameAndRobot,
          Qt::DirectConnection);
  connect(modules->referee(),
          &RefereeBase::sendReferee,
          this,
          &Player::receiveReferee,
          Qt::DirectConnection);
  connect(modules->decision()->ithSender(index()),
          &DecisionBaseIthSender::sendDecision,
          this,
          &Player::receiveDecision,
          Qt::DirectConnection);
}

void Player::init(const Modules* modules) {
}

void Player::update() {
  if (auto hasIsYellow = shared->hasIsYellow.get_optional_and_reset()) {
    world.set_isYellow(hasIsYellow);
  }

  if (auto field = shared->field.get_optional_and_reset()) {
    world.set_field(field);
  }

  if (auto frameAndRobot = shared->frameAndRobot.get_optional_and_reset()) {
    auto&& [frame, robot] = std::move(*frameAndRobot);
    world.set_frame(frame);
    world.set_robot(robot);
  }

  if (auto referee = shared->referee.get_optional_and_reset()) {
    world.set_referee(referee);
  }

  if (auto decision = shared->decision.get_optional_and_reset()) {
    auto&& [rcvdPlay, rcvdPlays] = std::move(*decision);
    world.set_play(rcvdPlay);
    world.set_plays(rcvdPlays);
  }
}

void Player::exec() {
  if (!world.has_world()) {
    return;
  }
  auto output = std::visit(*this, world.play().tactic());
  emit sendBehavior(output);
}

void Player::receiveIsYellow(bool rcvdIsYellow) {
  shared->hasIsYellow = rcvdIsYellow;
}

void Player::receiveField(const Field& rcvdField) {
  shared->field = rcvdField;
}

void Player::receiveReferee(const Referee::Output& rcvdReferee) {
  shared->referee = rcvdReferee;
}

void Player::receiveFrameAndRobot(const Frame& rcvdFrame, const Robot& rcvdRobot) {
  shared->frameAndRobot = std::tuple(rcvdFrame, rcvdRobot);
}

void Player::receiveDecision(const Decision::Play& rcvdPlay, const Decision::Output& rcvdPlays) {
  shared->decision.emplace(rcvdPlay, rcvdPlays);
  runInParallel();
}

static_block {
  Factory::behavior.insert<Player>();
};