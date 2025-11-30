#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//setAttribute(Qt::WA_PaintOnScreen);
	//setAttribute(Qt::WA_NoSystemBackground);

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

