#include "MainWindow.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.frameLabel->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    ui.frameLabel->setMouseTracking(true);
    ui.frameLabel->installEventFilter(this);
    ui.frameLabel->setCursor(Qt::OpenHandCursor);
    ui.frameLabel->setFocusPolicy(Qt::StrongFocus);
    ui.pushButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));



    ui.lineEdit->installEventFilter(this);

    qApp->installEventFilter(this);

    movementEnabled = true;
}

MainWindow::~MainWindow()
{
}

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
    ui.fpsLabel->setText(QString::fromStdString("FPS: " + std::to_string(fps)));
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui.lineEdit) {
        if (event->type() == QEvent::FocusIn) {
            movementEnabled = false;
        }
        if (event->type() == QEvent::FocusOut) {
            movementEnabled = true;
        }
        return false;
    }

    if (!movementEnabled) {
        if (event->type() == QEvent::KeyPress ||
            event->type() == QEvent::KeyRelease) {
            return false;
        }
    }

    if (obj == ui.frameLabel) {

        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* e = static_cast<QMouseEvent*>(event);
            if (e->button() == Qt::LeftButton) {
                ui.frameLabel->setFocus();
                dragging = true;
                ui.frameLabel->setCursor(Qt::ClosedHandCursor);
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
                ui.frameLabel->setCursor(Qt::OpenHandCursor);
                return true;
            }
        }
    }

    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent* e = static_cast<QKeyEvent*>(event);
        if (e->key() == Qt::Key_R) {
            onRenderingModeChanged();
            return true;
        }
    }

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* e = static_cast<QKeyEvent*>(event);
        pressedKeys.insert(e->key());
    }

    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent* e = static_cast<QKeyEvent*>(event);
        pressedKeys.remove(e->key());
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::checkMoveInput()
{
    if (!movementEnabled)
        return;

    MovementInput input;
    input.moveForward = pressedKeys.contains(Qt::Key_W);
    input.moveBackward = pressedKeys.contains(Qt::Key_S);
    input.moveLeft = pressedKeys.contains(Qt::Key_A);
    input.moveRight = pressedKeys.contains(Qt::Key_D);
    input.moveUp = pressedKeys.contains(Qt::Key_E);
    input.moveDown = pressedKeys.contains(Qt::Key_Q);
    onMoveInput(input);
}

void MainWindow::onViewportDrag(const QPoint& deltaFromStart, const QPoint& deltaFromLast)
{
    emit viewportDrag(deltaFromStart, deltaFromLast);
}

void MainWindow::onRenderingModeChanged()
{
    //Disabled because its obsolete and can cause crashes
    //emit switchRenderingModeSignal();
}

void MainWindow::onMoveInput(const MovementInput& input)
{
    emit moveSignal(input);
}

void MainWindow::onSceneLoaded() {
    emit loadScene((ui.lineEdit->text().toStdString()));
}
