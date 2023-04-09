#include "Coach3x3.h"

VSSCOACH3x3::VSSCOACH3x3(QThreadPool* threadPool) :
    DecisionBase(threadPool),
    Extensions(this, world) {
}

void VSSCOACH3x3::buildParameters(Parameters::Handler& parameters) {
  using namespace Parameters;

  auto inets = []() {
    QStringList names;
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto& interface : interfaces) {
      names += interface.name();
    }
    return names;
  };

  parameters["Teleport"] = CheckBox(args.teleport);
  parameters["Debug"] = CheckBox(args.debug);
  parameters["Teleport"]["true"]["IP"] = Text(args.teleporter.ip);
  parameters["Teleport"]["true"]["Port"] = Text(args.teleporter.port);
  parameters["Teleport"]["true"]["INet"] = ComboBox(args.teleporter.inet, inets());
}

void VSSCOACH3x3::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &VisionBase::sendIsYellow,
          this,
          &VSSCOACH3x3::receiveIsYellow,
          Qt::DirectConnection);
  connect(modules->vision(),
          &VisionBase::sendField,
          this,
          &VSSCOACH3x3::receiveField,
          Qt::DirectConnection);
  connect(modules->processedVision(),
          &ProcessedVisionBase::sendFrame,
          this,
          &VSSCOACH3x3::receiveFrame,
          Qt::DirectConnection);
  connect(modules->referee(),
          &RefereeBase::sendReferee,
          this,
          &VSSCOACH3x3::receiveReferee,
          Qt::DirectConnection);
}

void VSSCOACH3x3::init(const Modules* modules) {
  paintKey.setup(modules->gui()->gameVisualizer(), Painting::Layers::Top);
}

void VSSCOACH3x3::update() {

  if (args.teleport.updated()) {
    setupTeleporter(args.teleport);
  }

  if (auto isYellow = shared->hasIsYellow.get_optional_and_reset()) {
    world.emplace_isYellow(*isYellow);
  }
  if (auto frame = shared->frame.get_optional_and_reset()) {
    world.emplace_frame(*frame);
  }
  if (auto field = shared->field.get_optional_and_reset()) {
    world.emplace_field(*field);
    const float attackDirection = static_cast<float>(world.field().attackDirection().x());

    area0 = {
        field->allyPenaltyAreaCornerTop(),
        field->allyPenaltyAreaGoalCornerTop(),
        QPointF((field->min().x() * attackDirection), -field->min().y()),
        field->topCenter(),
        field->allyPenaltyAreaCornerTop(),
    };
    area1 = {
        field->allyPenaltyAreaCornerBottom(),
        field->bottomCenter(),
        field->isAttackingToRight() ?
            field->min() :
            QPointF(field->max().x(), (field->max().y() * attackDirection)),
        field->allyPenaltyAreaGoalCornerBottom(),
        field->allyPenaltyAreaCornerBottom(),
    };
    area2 = {field->allyPenaltyAreaCornerTop(),
             field->topCenter(),
             field->enemyPenaltyAreaCornerTop(),
             field->enemyPenaltyAreaCornerBottom(),
             field->bottomCenter(),
             field->allyPenaltyAreaCornerBottom(),
             field->allyPenaltyAreaCornerTop()};

    area3 = {field->enemyPenaltyAreaCornerTop(),
             field->topCenter(),
             field->isAttackingToRight() ?
                 field->max() :
                 QPointF((field->min().x() * -attackDirection), -field->min().y()),
             field->enemyPenaltyAreaGoalCornerTop(),
             field->enemyPenaltyAreaCornerTop()};

    area4 = {field->enemyPenaltyAreaCornerBottom(),
             field->bottomCenter(),
             QPointF((field->max().x() * attackDirection), -field->max().y()),
             field->enemyPenaltyAreaGoalCornerBottom(),
             field->enemyPenaltyAreaCornerBottom()};

    areas.push_back(area0);
    areas.push_back(area1);
    areas.push_back(area2);
    areas.push_back(area3);
    areas.push_back(area4);
  }
  if (auto referee = shared->referee.get_optional_and_reset()) {
    world.emplace_referee(*referee);
  }
}

void VSSCOACH3x3::exec() {
  if (!world.has_world()) {
    return;
  }

  if (args.debug) {
    debug();
  }

  auto output = std::visit(*this, world.referee().state());

  for (auto play : output) {
    auto plays = output;
    plays.removeIf([&](auto& other) {
      return play.id() == other.id();
    });
    emit ithSender(play.id())->sendDecision(play, plays);
  }
}

void VSSCOACH3x3::debug() {
  std::vector<Qt::GlobalColor> colors;

  colors.push_back(Qt::red);
  colors.push_back(Qt::green);
  colors.push_back(Qt::blue);
  colors.push_back(Qt::black);
  colors.push_back(Qt::yellow);

  for (int i = 0; i < (int) areas.size(); i++) {
    // verify if the ball is in the area
    if (areas[i].containsPoint(world.frame().ball().position(), Qt::OddEvenFill)) {
      colors[i] = Qt::green;
      break;
    }
  }

  paintKey.draw([this, colors = colors](GameVisualizerPainter2D* f) {
    f->drawPolygon(area0, colors[0], 5);
    f->drawPolygon(area1, colors[1], 5);
    f->drawPolygon(area3, colors[3], 5);
    f->drawPolygon(area4, colors[4], 5);
    f->drawPolygon(area2, colors[2], 5);
  });
}
void VSSCOACH3x3::receiveIsYellow(bool isYellow) {
  shared->hasIsYellow = isYellow;
}

void VSSCOACH3x3::receiveFrame(const Frame& frame) {
  shared->frame = frame;
  runInParallel();
}

void VSSCOACH3x3::receiveField(const Field& field) {
  shared->field = field;
}

void VSSCOACH3x3::receiveReferee(const Referee::Output& referee) {
  shared->referee = referee;
}

void VSSCOACH3x3::setupTeleporter(bool enabled) {
  if (enabled) {
    QMetaObject::invokeMethod(
        this,
        [this]() {
          teleporter.reset(new QUdpSocket(this));
          qDebug() << "VSSCOACH3x3::teleporter:" << teleporter.get() << "was created";
          connect(
              this,
              &VSSCOACH3x3::sendTeleport,
              teleporter.get(),
              [this,
               ip = QHostAddress(args.teleporter.ip.value()),
               port = args.teleporter.port.value()](const QByteArray& datagram) {
                teleporter->writeDatagram(datagram, ip, port);
              },
              Qt::QueuedConnection); // must be queued.
        },
        Qt::QueuedConnection); // must be queued.
  } else {
    QMetaObject::invokeMethod(
        this,
        [this]() {
          teleporter.reset();
          qDebug() << "VSSCOACH3x3::teleporter is disabled";
        },
        Qt::QueuedConnection);
  }
}

static_block {
  Factory::decision.insert<VSSCOACH3x3>();
};