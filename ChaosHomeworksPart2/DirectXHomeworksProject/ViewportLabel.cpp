#include "ViewportLabel.h"

ViewportLabel::ViewportLabel(QWidget* parent)
    : QLabel(parent)
{
    setMouseTracking(true);
}

void ViewportLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        startPos = event->pos();
        lastPos  = startPos;
    }
}

void ViewportLabel::mouseMoveEvent(QMouseEvent* event)
{
    if (!dragging || !(event->buttons() & Qt::LeftButton))
        return;

    QPoint currentPos = event->pos();
    QPoint deltaFromStart = currentPos - startPos;
    QPoint deltaFromLast  = currentPos - lastPos;

    emit drag(deltaFromStart, deltaFromLast);

    lastPos = currentPos;
}

void ViewportLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
}
