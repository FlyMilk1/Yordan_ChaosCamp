#include "CustomStopwatch.h"

void CustomStopwatch::start()
{
    startPoint = std::chrono::high_resolution_clock::now();
}

void CustomStopwatch::stop()
{
    endPoint = std::chrono::high_resolution_clock::now();
}

std::chrono::microseconds CustomStopwatch::getDurationMicro()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(endPoint-startPoint);
}

std::chrono::milliseconds CustomStopwatch::getDurationMilli()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(endPoint-startPoint);
}
