#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

MainWindow::~MainWindow()
{}

const QLabel* MainWindow::getRenderFrame() const
{
	return ui.frameLabel;
}

void MainWindow::fillFrame(const QImage& frameImage)
{
	ui.frameLabel->setPixmap(QPixmap::fromImage(frameImage));
}

void MainWindow::setFPSCounter(const unsigned int& fps)
{
	ui.fpsLabel->setText(QString::fromStdString("FPS: "+std::to_string(fps)));
}

