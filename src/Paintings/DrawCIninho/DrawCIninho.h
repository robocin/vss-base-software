#ifndef DRAWCININHO_H
#define DRAWCININHO_H

#include <soccer-common/soccer-common.h>

class DrawCIninho : public Painting {
  QPointF m_origin;
  qreal m_scale;
  qreal m_angle;
  QColor m_color;

 public:
  static const QVector<QPolygonF> CIninhoBody;
  static const QVector<QPolygonF> CIninhoEyes;
  static const QVector<QPolygonF> CIninhoCircuit;

  static const QVector<QPolygonF> CIninho;

  DrawCIninho(const QPointF& origin,
              qreal scale,
              qreal angle = 0.0,
              const QColor& color = Color::White);

  void run(GameVisualizerPainter2D* f) override;
  std::unique_ptr<Painting> clone() const override;
};

#endif // DRAWCININHO_H
