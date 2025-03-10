//
// Created by jose-cruz on 17/07/2021.
//

#ifndef VSS_BASE_SOFTWARE_BASESTATIONUTILS_H
#define VSS_BASE_SOFTWARE_BASESTATIONUTILS_H

#include "Packages/Packages.h"
#include "communication/commbst.h"

namespace BaseStation {
  inline QByteArray toRobotControl(const Navigation::Output& command) {
    auto packet = CommBst::makeVSSSpeed(command.id(), command.leftWheel(), command.rightWheel());

    return QByteArray{packet.c_str(), CommBst::VSSSpeedPacket::size()};
  }
} // namespace BaseStation

#endif // VSS_BASE_SOFTWARE_BASESTATIONUTILS_H
