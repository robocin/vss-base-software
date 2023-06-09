//
// Created by jose-cruz on 30/07/2021.
//

#ifndef VSS_BASE_SOFTWARE_LIBTORCH_H
#define VSS_BASE_SOFTWARE_LIBTORCH_H

// fixes the including error between LibTorch and Qt libraries
#pragma push_macro("slots")
#undef slots
#if __has_include(<torch/script.h>)
  #include <torch/script.h>
#endif
#pragma pop_macro("slots")

/***************************************************************************************************
 You can verify if libtorch exists using HAS_LIBTORCH macro. Example:

 #ifdef HAS_LIBTORCH
   #include <libtorch/libtorch.h>
   ...
 #endif

***************************************************************************************************/

#endif // VSS_BASE_SOFTWARE_LIBTORCH_H
