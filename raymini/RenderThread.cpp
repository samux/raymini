#include "RenderThread.h"

#include "Controller.h"

using namespace std;

RenderThread::RenderThread(Controller *c): controller(c), emergencyStop(false) {
    connect(this, SIGNAL(finished()), controller, SLOT(threadRenderRayImage()));
}

void RenderThread::run() {
    hasToRedrawMutex.lock();
    if (haveToRedraw || drawingIterations < controller->getRayTracer()->durtiesQuality+1) {
        reallyWorkingMutex.lock();
        reallyWorking = true;
        reallyWorkingMutex.unlock();
        time.restart();
        time.start();
        controller->threadSetsRenderQuality(drawingIterations++);
        resultImage = controller->getRayTracer()->render(
                camPos,
                viewDirection,
                upVector,
                rightVector,
                fieldOfView,
                aspectRatio,
                screenWidth,
                screenHeight);
        controller->threadSetElapsed(time.elapsed());
    }
    haveToRedraw = false;
    reallyWorkingMutex.lock();
    reallyWorking = false;
    reallyWorkingMutex.unlock();
    hasToRedrawMutex.unlock();
}

void RenderThread::startRendering(const Vec3Df & camPos,
                                  const Vec3Df & viewDirection,
                                  const Vec3Df & upVector,
                                  const Vec3Df & rightVector,
                                  float fieldOfView,
                                  float aspectRatio,
                                  unsigned int screenWidth,
                                  unsigned int screenHeight) {
    emergencyStop = false;
    hasToRedrawMutex.lock();
    haveToRedraw |= this->camPos != camPos;
    if (haveToRedraw) {
        if (controller->getWindowModel()->isRealTime()) {
            drawingIterations = 0;
        } else {
            drawingIterations = 10; // more than qualities
        }
    }
    prepare(camPos, viewDirection, upVector, rightVector, fieldOfView, aspectRatio, screenWidth, screenHeight);
    hasToRedrawMutex.unlock();
    start();
}

bool RenderThread::isRendering() {
    bool result = isRunning();
    if (result) {
        reallyWorkingMutex.lock();
        result = reallyWorking;
        reallyWorkingMutex.unlock();
    }
    return result;
}

bool RenderThread::hasRendered() {
    return isFinished();
}

void RenderThread::stopRendering() {
    emergencyStop = true;
    haveToRedraw = true;
    drawingIterations = 0;
    quit();
}

const QImage &RenderThread::getLastRendered() {
    return resultImage;
}

void RenderThread::hasToRedraw() {
    hasToRedrawMutex.lock();
    haveToRedraw = true;
    hasToRedrawMutex.unlock();
}
