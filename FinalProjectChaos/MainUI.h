#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainUI.h"
#include "vec3.h"
#include "AnimationSegment.h"
#include <vector>
#include "Renderer.h"

class MainUI : public QMainWindow
{
    Q_OBJECT

public:
    MainUI(QWidget *parent = nullptr);
    ~MainUI();
    

private:
    Ui::MainUIClass ui;
    void addFrames();
    std::vector<AnimationSegment> animationFrames;
    vec3 zeroVector = { 0.0f,0.0f,0.0f };
    AnimationSegment previousSegment;
    bool firstTime = true;
    AnimationSegment getAnimationSegment();
    void previewFrame();
    bool standartResolution;
    void setToStandartResolution();
    int standardResolutions[4][2] = { { 1920, 1080}, {1280, 720}, {960, 480}, {720, 360 } };

    void generateAnimation();
    void generateImage();
};


