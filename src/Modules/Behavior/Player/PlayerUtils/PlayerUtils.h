#ifndef VSS_UNIFICATION_PLAYERUTILS_H
#define VSS_UNIFICATION_PLAYERUTILS_H

#include "Packages/Packages.h"

namespace Behavior {
  class PlayerExtension : public ModuleExtension {
   public:
    template <class E, class P>
    inline PlayerExtension(E* e, P* player, const PlayerWorld& world) :
        ModuleExtension(e, player),
        world(world),
        m_index(player->index()) {
    }

   protected:
    [[nodiscard]] int index() const {
      return m_index;
    }

    const PlayerWorld& world;

   private:
    int m_index;
  };
} // namespace Behavior

#endif // VSS_UNIFICATION_PLAYERUTILS_H
