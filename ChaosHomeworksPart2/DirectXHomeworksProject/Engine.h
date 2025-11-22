#pragma once
#include <QtWidgets/QApplication>
#include <QTimer>
#include "MainWindow.h"
#include "CustomStopwatch.h"
#include "DXRenderer.h"
static const FLOAT orange[] = {1.0f,0.5f,0.0f,1.0f};
class Engine
{
public://Public functions
	Engine(MainWindow* mainWindow, const DXRenderer& renderer);
	void update();
	void start();
private://Private functions
	void getFrameColor(int i, float out[3]);
private://Variables
	UINT frameIndex=0;
	MainWindow* window;
	CustomStopwatch secondsPassedFromLastFrame;
	DXRenderer renderer;

};

