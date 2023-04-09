#ifndef VSS_BASE_SOFTWARE_PREPARETOFREEBALLDECISION_H
#define VSS_BASE_SOFTWARE_PREPARETOFREEBALLDECISION_H

#include "Modules/Decision/DecisionUtils/DecisionUtils.h"
#include "Utils/Utils.h"

namespace {
  using namespace Decision;
}

class PrepareToFreeBallDecision : public Extension {
  static constexpr double FREE_BALL_OFFSET = 200; // mm
  static constexpr Field field = Field::field(Field::Size::Small, true);

  struct Quadrant {
    ArgRobot firstRobot; // GK
    ArgRobot secondRobot;
    ArgRobot thirdRobot;
  };

  struct Args {

    Quadrant qTR{{-675.0, 0.0, 0.0}, // GK
                 {field.freeBallMark(Field::Quadrant::TopRight).x() - FREE_BALL_OFFSET,
                  field.freeBallMark(Field::Quadrant::TopRight).y(),
                  0.0},
                 {100.0, -200.0, 0.0}};

    Quadrant qTL{{-675.0, 0.0, 0.0}, // GK
                 {field.freeBallMark(Field::Quadrant::TopLeft).x() - FREE_BALL_OFFSET,
                  field.freeBallMark(Field::Quadrant::TopLeft).y(),
                  0.0},
                 {-200.0, -100.0, 0.0}};
    Quadrant qBL{{-675.0, 0.0, 0.0}, // GK
                 {field.freeBallMark(Field::Quadrant::BottomLeft).x() - FREE_BALL_OFFSET,
                  field.freeBallMark(Field::Quadrant::BottomLeft).y(),
                  0.0},
                 {-200.0, 100.0, 0.0}};
    Quadrant qBR{{-675.0, 0.0, 0.0}, // GK
                 {field.freeBallMark(Field::Quadrant::BottomRight).x() - FREE_BALL_OFFSET,
                  field.freeBallMark(Field::Quadrant::BottomRight).y(),
                  0.0},
                 {100.0, 200.0, 0.0}};
  };
  Args args;

 public:
  template <class M>
  inline PrepareToFreeBallDecision(M* m, const World& world) : Extension(this, m, world) {
  }

  void extendedBuildParameters(Parameters::Handler& parameters) final {
    static constexpr real_t LIMIT = 5000.0;

    parameters["Top Right Quadrant"]["1st Robot"] = args.qTR.firstRobot.handler(-LIMIT, +LIMIT);
    parameters["Top Right Quadrant"]["2nd Robot"] = args.qTR.secondRobot.handler(-LIMIT, +LIMIT);
    parameters["Top Right Quadrant"]["3rd Robot"] = args.qTR.thirdRobot.handler(-LIMIT, +LIMIT);

    parameters["Top Left Quadrant"]["1st Robot"] = args.qTL.firstRobot.handler(-LIMIT, +LIMIT);
    parameters["Top Left Quadrant"]["2nd Robot"] = args.qTL.secondRobot.handler(-LIMIT, +LIMIT);
    parameters["Top Left Quadrant"]["3rd Robot"] = args.qTL.thirdRobot.handler(-LIMIT, +LIMIT);

    parameters["Bottom Left Quadrant"]["1st Robot"] = args.qBL.firstRobot.handler(-LIMIT, +LIMIT);
    parameters["Bottom Left Quadrant"]["2nd Robot"] = args.qBL.secondRobot.handler(-LIMIT, +LIMIT);
    parameters["Bottom Left Quadrant"]["3rd Robot"] = args.qBL.thirdRobot.handler(-LIMIT, +LIMIT);

    parameters["Bottom Right Quadrant"]["1st Robot"] = args.qBR.firstRobot.handler(-LIMIT, +LIMIT);
    parameters["Bottom Right Quadrant"]["2nd Robot"] = args.qBR.secondRobot.handler(-LIMIT, +LIMIT);
    parameters["Bottom Right Quadrant"]["3rd Robot"] = args.qBR.thirdRobot.handler(-LIMIT, +LIMIT);
  }

  Output operator()(const Referee::StateType::PrepareToFreeBall& prepareToFreeBall) {
    Output output;
    QVector<RawRobot> formation;
    Quadrant* q;
    bool isAttackingRight = world.field().isAttackingToRight();

    if (prepareToFreeBall.quadrant() == Field::Quadrant::TopRight) {
      q = isAttackingRight ? &args.qTR : &args.qBL;
    } else if (prepareToFreeBall.quadrant() == Field::Quadrant::TopLeft) {
      q = isAttackingRight ? &args.qTL : &args.qBR;
    } else if (prepareToFreeBall.quadrant() == Field::Quadrant::BottomLeft) {
      q = isAttackingRight ? &args.qBL : &args.qTR;
    } else if (prepareToFreeBall.quadrant() == Field::Quadrant::BottomRight) {
      q = isAttackingRight ? &args.qBR : &args.qTL;
    } else {
      q = &args.qTR;
    }

    formation = getFormation(world, q->firstRobot, q->secondRobot, q->thirdRobot);
    emit sendTeleport(toTeleportDatagram(world, formation));

    for (const RawRobot& robot : formation) {
      output += Play(robot.id(), TacticType::GoToPosition(robot.position()));
    }

    return output;
  }
};

#endif // VSS_BASE_SOFTWARE_PREPARETOFREEBALLDECISION_H
