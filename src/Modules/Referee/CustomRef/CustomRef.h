//
// Created by jose-cruz on 28/08/2021.
//

#ifndef VSS_UNIFICATION_CUSTOMREF_H
#define VSS_UNIFICATION_CUSTOMREF_H

#include "Modules/Modules.h"
#include "Modules/Referee/RefereeUtils/RefereeUtils.h"

class CustomRef : public RefereeBase {
 public:
  explicit CustomRef(QThreadPool* threadPool);
  ~CustomRef() override = default;

  struct Args {
    Parameters::Arg<VSSRef::Foul> foul = VSSRef::Foul::GAME_ON;
    Parameters::Arg<VSSRef::Color> color = VSSRef::Color::BLUE;
    Parameters::Arg<VSSRef::Quadrant> quadrant = VSSRef::Quadrant::NO_QUADRANT;
    Parameters::Arg<VSSRef::Half> half = VSSRef::Half::FIRST_HALF;
  };
  Args args;

  struct Shared {
    SharedOptional<bool> hasIsYellow;
  };
  SharedWrapper<Shared, std::mutex> shared;

  std::optional<bool> hasIsYellow;

 protected:
  void buildParameters(Parameters::Handler& parameters) override;
  void connectModules(const Modules* modules) override;
  void init(const Modules* modules) override;
  void update() override;
  void exec() override;

 protected slots:
  void receiveIsYellow(bool isYellow);
};

#endif // VSS_UNIFICATION_CUSTOMREF_H
