//
// Created by jose-cruz on 04/08/2021.
//

#ifndef VSS_BASE_SOFTWARE_DECISIONOUTPUT_H
#define VSS_BASE_SOFTWARE_DECISIONOUTPUT_H

#include <soccer-common/soccer-common.h>
#include "Packages/Tactic/Tactic.h"

namespace Decision {
  class Play {
    int m_id{};
    Tactic m_tactic;

   public:
    explicit Play(int id, const Tactic& tactic) : m_id(id), m_tactic(tactic) {
    }

    [[nodiscard]] int id() const {
      return m_id;
    }

    [[nodiscard]] inline Role role() const {
      return std::visit(
          [](auto&& t) -> Role {
            return t.role();
          },
          m_tactic);
    }

    [[nodiscard]] const auto& tactic() const {
      return m_tactic;
    }
  };

  using Output = QVector<Play>;
} // namespace Decision

#endif // VSS_BASE_SOFTWARE_DECISIONOUTPUT_H
