//
// Created by jose-cruz on 09/02/2021.
//

#ifndef VSS_BASE_SOFTWARE_DRAWFIELD_H
#define VSS_BASE_SOFTWARE_DRAWFIELD_H

#include <soccer-common/soccer-common.h>
#include "Field/Field.h"

class DrawField : public Painting {
  Field field;
  bool drawFieldMarkings;
  bool drawCIninho;

 public:
  explicit DrawField(const Field& field, bool drawFieldMarkings, bool drawCIninho = false);
  void run(GameVisualizerPainter2D* f) override;
  std::unique_ptr<Painting> clone() const override;
};

#endif // VSS_BASE_SOFTWARE_DRAWFIELD_H
