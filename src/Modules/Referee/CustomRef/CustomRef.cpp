//
// Created by jose-cruz on 28/08/2021.
//

#include "CustomRef.h"

CustomRef::CustomRef(QThreadPool* threadPool) : RefereeBase(threadPool) {
}

void CustomRef::buildParameters(Parameters::Handler& parameters) {
  using namespace Parameters;
  parameters["Foul"] = ComboBox(args.foul, MagicEnum::values<VSSRef::Foul>());
  parameters["Color"] = ComboBox(args.color, MagicEnum::values<VSSRef::Color>());
  parameters["Quadrant"] = ComboBox(args.quadrant, MagicEnum::values<VSSRef::Quadrant>());
  parameters["Half"] = ComboBox(args.half, MagicEnum::values<VSSRef::Half>());
}

void CustomRef::connectModules(const Modules* modules) {
  connect(modules->vision(),
          &VisionBase::sendIsYellow,
          this,
          &CustomRef::receiveIsYellow,
          Qt::DirectConnection);
}

void CustomRef::init(const Modules*) {
}

void CustomRef::update() {
  shared->hasIsYellow.extract_to(hasIsYellow);
}

void CustomRef::exec() {
  if (!hasIsYellow) {
    return;
  }
  bool isYellow = hasIsYellow.value();
  VSSRef::ref_to_team::VSSRef_Command packet;
  packet.set_foul(args.foul);
  packet.set_teamcolor(args.color);
  packet.set_foulquadrant(args.quadrant);
  packet.set_timestamp(0); // TODO: simulate time considering fouls
  packet.set_gamehalf(args.half);
  emit sendReferee(Referee::Parser::parse(packet, isYellow));
}

void CustomRef::receiveIsYellow(bool isYellow) {
  shared->hasIsYellow = isYellow;
  runInParallel();
}

static_block {
  Factory::referee.insert<CustomRef>();
};