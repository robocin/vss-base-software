#ifndef VSS_BASE_SOFTWARE_ROLE_H
#define VSS_BASE_SOFTWARE_ROLE_H

#include <cstdint>

enum class Role : uint8_t { Goalkeeper, Dynamic, Forward };

namespace TacticType {
  template <Role R>
  class TacticOf {
   public:
    [[nodiscard]] inline static constexpr Role role() {
      return R;
    }
  };
} // namespace TacticType

#endif // VSS_BASE_SOFTWARE_ROLE_H
