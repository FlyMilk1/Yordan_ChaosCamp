#pragma once
#include <QtWidgets/QMainWindow>
#include <QObject>
#include "ui_MainWindow.h"
class MainWindow  : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	Ui::MainWindow ui;
public://Public functions
	/// <summary>
	/// Returns a pointer to the QLabel that is the render frame viewport
	/// </summary>
	/// <returns></returns>
	const QLabel* getRenderFrame()const;
	
	/// <summary>
	/// Updates the render frame viewport to a QImage
	/// </summary>
	/// <param name="frameImage">The QImage that the viewport will display</param>
	void updateViewport(const QImage& frameImage);

	/// <summary>
	/// Sets the Frames Per Second counter to an unsigned int
	/// </summary>
	/// <param name="fps">The value that will be displayed on the FPS counter</param>
	void setFPSCounter(const unsigned int& fps);
private://Variables

};

