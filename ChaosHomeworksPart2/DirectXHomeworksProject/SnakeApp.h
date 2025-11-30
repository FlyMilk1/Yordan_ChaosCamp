#pragma once
#include <QtWidgets/QApplication>
#include <QTimer>
#include "MainWindow.h"
#include "CustomStopwatch.h"
#include "DXRenderer.h"
#include "FPSPresets.h"
static const FLOAT orange[] = {1.0f,0.5f,0.0f,1.0f};
class SnakeApp : public QObject
{
	Q_OBJECT
public://Public functions
	SnakeApp();

	/// <summary>
	/// Executes the first frame
	/// </summary>
	bool init();
private://Private functions
	/// <summary>
	/// Fills array with a color based on the frame index
	/// </summary>
	/// <param name="i">Frame index</param>
	/// <param name="out">Color array</param>
	void getFrameColor(int i, float out[3]);

	bool initWindow();

private slots:
	void updateRenderStats();

	/// <summary>
	/// Executes every frame
	/// </summary>
	void onIdleTick();

	
private://Variables
	UINT frameIndex=0; // Index of current frame
	MainWindow* mainWindow = nullptr; //Pointer to the Main Window
	CustomStopwatch secondsPassedFromLastFrame = {}; //Stopwatch that keeps the time from previous frame
	DXRenderer renderer = {}; //The DirectX Renderer
	QApplication* app = nullptr; //Pointer to the Qt Application
	QTimer* idleTimer = nullptr;
	QTimer* fpsTimer = nullptr;
};

