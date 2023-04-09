//
// Created by jose-cruz on 12/06/2021.
//

#include "DefaultProcessing.h"

DefaultProcessing::DefaultProcessing(QThreadPool* threadPool) : ProcessedVisionBase(threadPool) {
}

void DefaultProcessing::buildParameters(Parameters::Handler&) {
}

void DefaultProcessing::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &VisionBase::sendVision,
          this,
          &DefaultProcessing::receiveVision,
          Qt::DirectConnection);

  // TODO: Set timeout.
  // modules->timers()->hertz<500>()->callOnTimeout(this, &DefaultProcessing::runInParallel);
}

void DefaultProcessing::init(const Modules* modules) {
}

void DefaultProcessing::update() {
  shared->vision.extract_to(vision);
}

void DefaultProcessing::exec() {
  if (!vision) {
    return;
  }
  if (auto frame = std::visit(*this, vision.value())) {
    /* send for each robot */ {
      auto allAllies = frame->allies();
      for (const Robot& ally : allAllies) {
        frame->set_allies(allAllies.removed(ally));
        emit ithSender(ally.id())->sendFrameAndRobot(frame.value(), ally);
      }
      frame->emplace_allies(std::move(allAllies));
    }
    emit sendFrame(frame.value());
  }
}

std::optional<Frame> DefaultProcessing::operator()(const Vision::OutputType::Raw& rawFrames) {
  // TODO: Add vision processing.
  auto& latestRaw = rawFrames.last();
  std::optional<Ball> ball;
  if (!latestRaw.balls().empty()) {
    ball = Ball(latestRaw.balls().first(), {}, {});
  }
  if (!ball && !lastFrame) {
    return std::nullopt;
  }
  ball = ball.value_or(lastFrame->ball());
  Robots<Robot> allies, enemies;

  for (const auto& ally : latestRaw.allies()) {
    allies += Robot(ally, {}, {}, {});
  }
  for (const auto& enemy : latestRaw.enemies()) {
    enemies += Robot(enemy, {}, {}, {});
  }

  return lastFrame.emplace(ball.value(), allies, enemies);
}

std::optional<Frame> DefaultProcessing::operator()(const Vision::OutputType::Tracked& tracked) {
  std::optional<Ball> ball = tracked.optional_ball();
  if (!ball && !lastFrame) {
    return std::nullopt;
  }
  ball = ball.value_or(lastFrame->ball());
  return lastFrame.emplace(ball.value(), tracked.allies(), tracked.enemies());
}

void DefaultProcessing::receiveVision(const Vision::Output& rcvVision) {
  shared->vision.apply([&](std::optional<Vision::Output>& p) {
    if (p.has_value()) {
      if (const auto* rhs = std::get_if<Vision::OutputType::Raw>(&rcvVision)) {
        // try to append if exists an array of raw packets.
        if (auto* lhs = std::get_if<Vision::OutputType::Raw>(&p.value())) {
          lhs->append(*rhs);
          return;
        }
      }
    }
    p = rcvVision;
  });
  // TODO: Set timeout if needed.
  runInParallel();
}

static_block {
  Factory::processedVision.insert<DefaultProcessing>();
};