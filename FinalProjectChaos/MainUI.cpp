#include "MainUI.h"

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent), previousSegment({0,0,0},0,0,0)
{
    ui.setupUi(this);
    connect(ui.applyButton, &QPushButton::clicked, this, &MainUI::addFrames);
    connect(ui.previewButton, &QPushButton::clicked, this, &MainUI::previewFrame);
    connect(ui.standardResCheck, &QCheckBox::checkStateChanged, this, &MainUI::setToStandartResolution);
    connect(ui.animateButton, &QPushButton::clicked, this, &MainUI::generateAnimation);
    connect(ui.singleButton, &QPushButton::clicked, this, &MainUI::generateImage);
    connect(ui.resetButton, &QPushButton::clicked, this, &MainUI::resetAnimation);
}

MainUI::~MainUI()
{
}

AnimationSegment MainUI::getAnimationSegment() {
    QString xString = ui.xPosComp->toPlainText();
    QString yString = ui.yPosComp->toPlainText();
    QString zString = ui.zPosComp->toPlainText();

    vec3 newPosition = { xString.toFloat(),yString.toFloat() ,zString.toFloat() };

    QString panString = ui.panValue->toPlainText();
    QString tiltString = ui.tiltValue->toPlainText();
    QString frameString = ui.frameValue->toPlainText();

    AnimationSegment newSegment(newPosition, panString.toFloat(), tiltString.toFloat(), frameString.toInt());
    return newSegment;
}
void MainUI::addFrames() {
    AnimationSegment newSegment = getAnimationSegment();
    if (firstTime) {
        animationFrames.push_back(newSegment);
        previousSegment = newSegment;
        firstTime = false;
    }
    else {
        newSegment.calculateFrames(previousSegment);
        animationFrames.push_back(newSegment);
        previousSegment = newSegment;
    }
}

void MainUI::previewFrame()
{
    ui.loadingLabel->setText(loadingStr);
    QCoreApplication::processEvents();
    QString filePath = ui.filePath->toPlainText();
    QImage previewFrame;

    int width = ui.imageLabel->width();
    int height = ui.imageLabel->height();

    if (width % 2 != 0)width -= 1;
    if (height % 2 != 0)height -= 1;
    if (standartResolution) {
        int pixels = width * height;
        int max = 0;
        int newWidth, newHeight;
        for (int resIdx = 0; resIdx < (sizeof(standardResolutions) / sizeof(standardResolutions[0])); resIdx++) {
            if (standardResolutions[resIdx][0] * standardResolutions[resIdx][1] < pixels && standardResolutions[resIdx][0] * standardResolutions[resIdx][1] > max) {
                max = standardResolutions[resIdx][0] * standardResolutions[resIdx][1];
                newWidth = standardResolutions[resIdx][0];
                newHeight = standardResolutions[resIdx][1];
            }
        }
        if (max != 0) {
            width = newWidth;
            height = newHeight;
        }
    }
    AnimationSegment newSegment = getAnimationSegment();

    Renderer renderer;
    
    renderer.generateImage(filePath.toStdString(), previewFrame, width, height, false, newSegment);
    ui.imageLabel->setPixmap(QPixmap::fromImage(previewFrame));
    ui.loadingLabel->setText(doneStr);
}

void MainUI::setToStandartResolution()
{
    standartResolution = ui.standardResCheck->checkState();
}

void MainUI::generateAnimation()
{
    QImage previewFrame;
    AnimationSegment newSegment = getAnimationSegment();
    QString filePath = ui.filePath->toPlainText();
    Renderer renderer;
    renderer.generateImage(filePath.toStdString(), previewFrame, -1, -1, true, newSegment, animationFrames);
}

void MainUI::generateImage()
{
    QImage previewFrame;
    AnimationSegment newSegment = getAnimationSegment();
    QString filePath = ui.filePath->toPlainText();
    Renderer renderer;
    renderer.generateImage(filePath.toStdString(), previewFrame, -1, -1, false, newSegment, animationFrames);
}

void MainUI::resetAnimation()
{
    animationFrames.clear();
}
