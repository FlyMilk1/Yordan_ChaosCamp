//Made by Yordan Yonchev for Chaos Raytracing Course 2025
#pragma once
#include <QtWidgets/QApplication>
#include <QTimer>
#include "MainWindow.h"
#include "CustomStopwatch.h"
#include "DXRenderer.h"
#include "FPSPresets.h"

class SnakeApp : public QObject
{
	Q_OBJECT
public://Public functions
	SnakeApp();

	/// <summary>
	/// Initializes the application and prepares for rendering
	/// </summary>
	bool init();
private://Private functions
	/// <summary>
	/// Fills array with a color based on the frame index
	/// </summary>
	/// <param name="i">Frame index</param>
	/// <param name="out">Color array</param>
	void getFrameColor(int i, float out[3]);

	/// <summary>
	/// Initializes the application window
	/// </summary>
	/// <returns></returns>
	bool initWindow();

private slots:

	/// <summary>
	/// Updates the rendering stats
	/// </summary>
	void updateRenderStats();

	/// <summary>
	/// Executes on every frame
	/// </summary>
	void onIdleTick();

	
private://Variables
	UINT frameIndex=0; // Index of current frame
	MainWindow* mainWindow = nullptr; //Pointer to the Main Window
	CustomStopwatch secondsPassedFromLastFrame = {}; //Stopwatch that keeps the time from previous frame
	DXRenderer renderer = {}; //The DirectX Renderer
	QApplication* app = nullptr; //Pointer to the Qt Application
	QTimer* idleTimer = nullptr; //Pointer to the idle timer
	QTimer* fpsTimer = nullptr; //Pointer to the FPS timer
};

