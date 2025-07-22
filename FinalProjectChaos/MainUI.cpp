#include "MainUI.h"

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent), previousSegment({0,0,0},0,0,0)
{
    ui.setupUi(this);
    connect(ui.applyButton, &QPushButton::clicked, this, &MainUI::addFrames);
}

MainUI::~MainUI()
{
}


void MainUI::addFrames() {
    QString xString = ui.xPosComp->toPlainText();
    QString yString = ui.yPosComp->toPlainText();
    QString zString = ui.zPosComp->toPlainText();

    vec3 newPosition = { xString.toFloat(),yString.toFloat() ,zString.toFloat() };

    QString panString = ui.panValue->toPlainText();
    QString tiltString = ui.tiltValue->toPlainText();
    QString frameString = ui.frameValue->toPlainText();
    
    AnimationSegment newSegment(newPosition, panString.toFloat(), tiltString.toFloat(), frameString.toInt());
    if (firstTime) {
        animationFrames.push_back(newSegment);
        previousSegment = newSegment;
    }
    else {
        newSegment.calculateFrames(previousSegment);
        animationFrames.push_back(newSegment);
        previousSegment = newSegment;
    }
}