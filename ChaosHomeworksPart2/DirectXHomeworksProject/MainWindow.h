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
	const QLabel* getRenderFrame()const;
	void updateViewport(const QImage& frameImage);
	void setFPSCounter(const unsigned int& fps);
private://Variables

};

