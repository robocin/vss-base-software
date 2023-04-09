//
// Created by jose-cruz on 08/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_PROTOBUFS_H
#define VSS_BASE_SOFTWARE_PROTOBUFS_H

#if __has_include("pb/command.pb.h")
  #include "pb/command.pb.h"
#else
  #warning "pb/command.pb.h" was not found.
#endif

#if __has_include("pb/common.pb.h")
  #include "pb/common.pb.h"
#else
  #warning "pb/common.pb.h" was not found.
#endif

#if __has_include("pb/packet.pb.h")
  #include "pb/packet.pb.h"
#else
  #warning "pb/packet.pb.h" was not found.
#endif

#if __has_include("pb/replacement.pb.h")
  #include "pb/replacement.pb.h"
#else
  #warning "pb/replacement.pb.h" was not found.
#endif

#if __has_include("pb/vssref_command.pb.h")
  #include "pb/vssref_command.pb.h"
#else
  #warning "pb/vssref_command.pb.h" was not found.
#endif

#if __has_include("pb/vssref_common.pb.h")
  #include "pb/vssref_common.pb.h"
#else
  #warning "pb/vssref_common.pb.h" was not found.
#endif

#if __has_include("pb/vssref_placement.pb.h")
  #include "pb/vssref_placement.pb.h"
#else
  #warning "pb/vssref_placement.pb.h" was not found.
#endif

#if __has_include("pb/ssl_vision_detection.pb.h")
  #include "pb/ssl_vision_detection.pb.h"
#else
  #warning "pb/ssl_vision_detection.pb.h" was not found.
#endif

#if __has_include("pb/ssl_vision_geometry.pb.h")
  #include "pb/ssl_vision_geometry.pb.h"
#else
  #warning "pb/ssl_vision_geometry.pb.h" was not found.
#endif

#if __has_include("pb/ssl_vision_wrapper.pb.h")
  #include "pb/ssl_vision_wrapper.pb.h"
#else
  #warning "pb/ssl_vision_wrapper.pb.h" was not found.
#endif

#if __has_include("pb/ssl_vision_wrapper_tracked.pb.h")
  #include "pb/ssl_vision_wrapper_tracked.pb.h"
#else
  #warning "pb/ssl_vision_wrapper_tracked.pb.h" was not found.
#endif

#endif // VSS_BASE_SOFTWARE_PROTOBUFS_H
