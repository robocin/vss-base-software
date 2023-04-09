#ifndef VSS_BASE_SOFTWARE_DEBUG_H
#define VSS_BASE_SOFTWARE_DEBUG_H

#include "Modules/Modules.h"

class Debug : public BehaviorBase {
 public:
  Debug(int index, QThreadPool* threadPool);
  enum class DebugMode { ManualPackage, Halt };

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 private:
  struct Args {
    Parameters::Arg<DebugMode> debugMode = DebugMode::ManualPackage;
    struct ManualPackage {
      Parameters::Arg<double> wLeftWheel = 0;
      Parameters::Arg<double> wRightWheel = 0;
    };
    ManualPackage manualPackage;
  };
  Args args;

  struct Shared {
    SharedOptional<bool> hasIsYellow;
    SharedOptional<Field> field;
    SharedOptional<Referee::Output> referee;
    SharedOptional<std::tuple<Frame, Robot>> frameAndRobot;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::optional<Field> field;
  std::optional<Frame> frame;
  std::optional<Robot> robot;

  Behavior::Output manualPackage();

 private slots:
  void receiveIsYellow(bool rcvdIsYellow);
  void receiveField(const Field& rcvdField);
  void receiveReferee(const Referee::Output& rcvdReferee);
  void receiveFrameAndRobot(const Frame& rcvdFrame, const Robot& rcvdRobot);
};

#endif // VSS_BASE_SOFTWARE_DEBUG_H
