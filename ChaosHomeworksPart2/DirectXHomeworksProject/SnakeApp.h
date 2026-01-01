//Made by Yordan Yonchev for Chaos Raytracing Course 2025
#pragma once
#include <QtWidgets/QApplication>
#include <QTimer>
#include "MainWindow.h"
#include "CustomStopwatch.h"
#include "DXRenderer.h"
#include "FPSPresets.h"
#include "FrameData.h"
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

	/// <summary>
	/// Executes when the mouse is dragged with left click
	/// </summary>
	void onCameraPan(const QPoint& deltaFromStart, const QPoint& deltaFromLast);
	/// <summary>
	/// Switches the current rendering mode.
	/// </summary>
	void switchRenderingMode();
private://Variables
	UINT frameIndex=0; // Index of current frame
	MainWindow* mainWindow = nullptr; //Pointer to the Main Window
	CustomStopwatch secondsPassedFromLastFrame = {}; //Stopwatch that keeps the time from previous frame
	DXRenderer renderer = {}; //The DirectX Renderer
	QApplication* app = nullptr; //Pointer to the Qt Application
	QTimer* idleTimer = nullptr; //Pointer to the idle timer
	QTimer* fpsTimer = nullptr; //Pointer to the FPS timer
	FrameData frameData; //Frame data passed to the renderer for shaders
	bool isUsingRayTracing = true; //Whether ray tracing is used or not
};

