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

	/// <summary>
	/// Overrides QObject::eventFilter to intercept events sent to watched objects before they reach their normal handlers.
	/// </summary>
	/// <param name="obj">Pointer to the object that the event is being sent to (the watched object).</param>
	/// <param name="event">Pointer to the event being delivered.</param>
	/// <returns>true if the event was handled and should not be propagated to the target object; false to allow normal processing.</returns>
	bool eventFilter(QObject* obj, QEvent* event) override;

	/// <summary>
	/// Executed when mouse dragging
	/// </summary>
	void onViewportDrag(const QPoint& deltaFromStart,
		const QPoint& deltaFromLast);

	/// <summary>
	/// Called when the rendering mode changes to perform any necessary updates.
	/// </summary>
	void onRenderingModeChanged();
signals:
	void viewportDrag(const QPoint& deltaFromStart,
		const QPoint& deltaFromLast);
    void switchRenderingModeSignal();
private:
	bool dragging = false;
	QPoint startPos;
	QPoint lastPos;
	QPoint deltaFromStart;
	QPoint deltaFromLast;
};

