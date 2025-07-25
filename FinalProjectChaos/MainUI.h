#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainUI.h"
#include "vec3.h"
#include "AnimationSegment.h"
#include <vector>
#include "Renderer.h"
static const char LOADING[1] = { 54 }, DONE[1] = { 32 };
static QString loadingStr = QString::fromUtf8(LOADING), doneStr = QString::fromUtf8(DONE);
static const int standardResolutions[4][2] = { { 1920, 1080}, {1280, 720}, {960, 480}, {720, 360 } };
static const vec3 zeroVector = { 0.0f,0.0f,0.0f };
class MainUI : public QMainWindow
{
    Q_OBJECT

public:  
    MainUI(QWidget *parent = nullptr);
    ~MainUI();
    Ui::MainUIClass ui;
   
private:
    
    void addFrames();
    std::vector<AnimationSegment> animationFrames;
    AnimationSegment previousSegment;
    bool firstTime = true;
    AnimationSegment getAnimationSegment();
    void previewFrame();
    bool standartResolution = false;
    void setToStandartResolution(); 
    void generateAnimation();
    void generateImage();
    void resetAnimation();
};


