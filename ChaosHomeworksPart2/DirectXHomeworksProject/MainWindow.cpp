#include "MainWindow.h"
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    // Mouse events still handled by frameLabel
    ui.frameLabel->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    ui.frameLabel->setMouseTracking(true);
    ui.frameLabel->installEventFilter(this);

    // Keyboard events handled globally
    qApp->installEventFilter(this);
}


MainWindow::~MainWindow()
{}

const QLabel* MainWindow::getRenderFrame() const
{
	return ui.frameLabel;
}

void MainWindow::updateViewport(const QImage& frameImage)
{
	ui.frameLabel->setPixmap(QPixmap::fromImage(frameImage));
}

void MainWindow::setFPSCounter(const unsigned int& fps)
{
	ui.fpsLabel->setText(QString::fromStdString("FPS: "+std::to_string(fps)));
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
	/// Handle mouse events for dragging on the frameLabel
    if (obj == ui.frameLabel) {

        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* e = static_cast<QMouseEvent*>(event);

            if (e->button() == Qt::LeftButton) {
                dragging = true;
                startPos = e->pos();
                lastPos = startPos;
                return true;
            }
        }

        if (event->type() == QEvent::MouseMove && dragging) {
            QMouseEvent* e = static_cast<QMouseEvent*>(event);

            QPoint currentPos = e->pos();
            QPoint deltaFromStart = currentPos - startPos;
            QPoint deltaFromLast = currentPos - lastPos;

            onViewportDrag(deltaFromStart, deltaFromLast);

            lastPos = currentPos;
            return true;
        }

        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* e = static_cast<QMouseEvent*>(event);

            if (e->button() == Qt::LeftButton) {
                dragging = false;
                return true;
            }
        }
    }
    // Keyboard events
    if (event->type() == QEvent::KeyRelease) {
		//Switch rendering mode on space key press
        QKeyEvent* e = static_cast<QKeyEvent*>(event);
        if (e->key() == Qt::Key_Space) {
            onRenderingModeChanged();
            return true;
		}
    }
    
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onViewportDrag(const QPoint& deltaFromStart, const QPoint& deltaFromLast)
{
    emit viewportDrag(deltaFromStart, deltaFromLast);
}

void MainWindow::onRenderingModeChanged()
{
	emit switchRenderingModeSignal();
}
