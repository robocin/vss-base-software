//
// Created by jose-cruz on 17/07/2021.
//

#include "CommunicationBSt.h"

CommunicationBSt::CommunicationBSt(QThreadPool* threadPool) : CommunicationBase(threadPool) {
}

void CommunicationBSt::buildParameters(Parameters::Handler& parameters) {
  using namespace Parameters;
}

void CommunicationBSt::connectModules(const Modules* modules) {
  for (NavigationBase* navigation : modules->navigation()) {
    connect(navigation,
            &NavigationBase::sendNavigation,
            this,
            &CommunicationBSt::receiveNavigation,
            Qt::DirectConnection);
  }
  connect(modules->processedVision(),
          &ProcessedVisionBase::sendFrame,
          this,
          &CommunicationBSt::receiveFrame,
          Qt::DirectConnection);
  connect(modules->vision(),
          &VisionBase::sendIsYellow,
          this,
          &CommunicationBSt::receiveIsYellow,
          Qt::DirectConnection);
}

void CommunicationBSt::init(const Modules* modules) {
  // Increase hertz not decrease receive time between packages in robot, frequency optimized for
  // capability of base-station sending, require reduce package size to try this.
  modules->timers()->hertz<120>()->callOnTimeout(this,
                                                 &CommunicationBSt::runInParallel,
                                                 Qt::DirectConnection);

  commandsKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Top);

  batteryKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Top);
}

void CommunicationBSt::update() {
  if (!bstConfigured) {
    BstService::instance().setup();
    bstConfigured = true;
  }

  commands = shared.apply([](Shared& obj) -> QVector<Navigation::Output> {
    QVector<Navigation::Output> result;
    for (SharedOptional<Navigation::Output>& command : obj.commands) {
      if (command.has_value()) {
        result.emplace_back(command.value());
      }
    }
    return result;
  });

  shared->frame.extract_to(frame);
  shared->hasIsYellow.extract_to(hasIsYellow);
}

void CommunicationBSt::exec() {
  if (!frame)
    return;

  for (auto&& command : commands) {
    // direct velocity control
    BstService::instance().send(BaseStation::toRobotControl(command));
  }
  parseTelemetry(BstService::instance().read());

  batteryKey.draw([telemetries = telemetries,
                   isYellow = hasIsYellow.value()](GameVisualizerPainter2D* f) {
    auto offsetY = (qreal) 580.0;
    auto offsetX = 920;
    auto distX = 4 * Robot::halfAxis();
    auto totalDistX = 3 * distX * 1.25;

    int telemetryCount = 0;

    for (auto&& telemetry : telemetries) {
      if (telemetry.has_value() && telemetry->second.secsTo(QTime::currentTime()) < 30) {
        auto batteryColorStatus = [&](real_t volts) {
          if (volts <= BATTERY_CRITICAL_VOLTAGE)
            return Color::Red;
          else if (volts <= BATTERY_WARNING_VOLTAGE)
            return Color::Orange;
          else
            return Color::LightGreen;
        };

        f->drawPolygon(
            QPolygonF(QRectF(offsetX, offsetY - distX / 2 - telemetryCount * distX, distX, distX)),
            Color::White,
            10);

        // Robot
        f->drawFilledRectangle(
            QPointF(offsetX + distX * 0.15, (offsetY + distX * 0.40) - telemetryCount * distX),
            QPointF(offsetX + distX * 0.85, (offsetY - distX * 0.15) - telemetryCount * distX),
            isYellow ? Color::Yellow : Color::Blue);

        auto drawCapacity = [&](real_t volts) {
          auto capacity = (volts * 0.23) / 2;
          // Battery

          f->drawFilledRectangle(
              QPointF(offsetX + 5, (offsetY - 40) + distX / 2 * 0.15 - (telemetryCount) *distX),
              QPointF(offsetX + (distX * capacity),
                      (offsetY - 50) - distX / 2 * 0.15 - (telemetryCount) *distX),
              batteryColorStatus(volts));
          f->drawText(
              QString::number(volts, 'f', 2) + "V",
              QPointF(offsetX + 72, ((offsetY - 55) + distX / 2 * 0.15 - (telemetryCount) *distX)),
              36.0,
              Color::Black);
        };
        // ID
        f->drawText(QString::number(telemetry->first.id),
                    Point(offsetX + distX * 0.45, offsetY + 20 - (telemetryCount) *distX),
                    80,
                    isYellow ? Color::Black : Color::White);
        drawCapacity(telemetry->first.battery);
      }
      if (telemetry.has_value())
        telemetryCount++;
    }
  });
}

void CommunicationBSt::parseTelemetry(const QVector<robocin::comm::GenericPacket>& packages) {
  for (auto&& packet : packages) {
    if (auto telemetry = CommBst::readVSSTelemetry(packet)) {
      telemetries[telemetry->id] = Telemetry(telemetry.value(), QTime::currentTime());
    }
  }
}

void CommunicationBSt::receiveNavigation(const Navigation::Output& navigation) {
  shared->commands[navigation.id()] = navigation;
}

void CommunicationBSt::receiveFrame(const Frame& frame) {
  shared->frame = frame;
}

void CommunicationBSt::receiveIsYellow(bool rcvdIsYellow) {
  shared->hasIsYellow = rcvdIsYellow;
}

static_block {
  Factory::communication.insert<CommunicationBSt>();
};