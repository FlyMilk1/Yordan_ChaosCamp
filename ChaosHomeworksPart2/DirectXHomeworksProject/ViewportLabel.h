#pragma once
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>

class ViewportLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ViewportLabel(QWidget* parent = nullptr);

signals:
    void drag(const QPoint& deltaFromStart, const QPoint& deltaFromLast);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    bool dragging = false;
    QPoint startPos;
    QPoint lastPos;
};
