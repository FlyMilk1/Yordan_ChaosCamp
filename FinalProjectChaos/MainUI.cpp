#include "MainUI.h"

MainUI::MainUI(QWidget *parent)
    : QMainWindow(parent), previousSegment({0,0,0},0,0,0)
{
    ui.setupUi(this);
    connect(ui.applyButton, &QPushButton::clicked, this, &MainUI::addFrames);
    connect(ui.previewButton, &QPushButton::clicked, this, &MainUI::previewFrame);
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
    }
    else {
        newSegment.calculateFrames(previousSegment);
        animationFrames.push_back(newSegment);
        previousSegment = newSegment;
    }
}

void MainUI::previewFrame()
{
    QString filePath = ui.filePath->toPlainText();
    QImage previewFrame;

    int width = ui.imageLabel->width();
    int height = ui.imageLabel->height();

    if (width % 2 != 0)width -= 1;
    if (height % 2 != 0)height -= 1;

    AnimationSegment newSegment = getAnimationSegment();

    Renderer renderer;
    renderer.generateImage(filePath.toStdString(), previewFrame, width, height, newSegment);
    ui.imageLabel->setPixmap(QPixmap::fromImage(previewFrame));
}
