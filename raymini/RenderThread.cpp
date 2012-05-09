#include "RenderThread.h"

#include "Controller.h"

using namespace std;

RenderThread::RenderThread(Controller *c): controller(c), emergencyStop(false) {
    connect(this, SIGNAL(finished()), controller, SLOT(threadRenderRayImage()));
}

void RenderThread::run() {
    hasToRedrawMutex.lock();
    if (haveToRedraw || !optimalDone) {
        reallyWorkingMutex.lock();
        reallyWorking = true;
        reallyWorkingMutex.unlock();
        emergencyStopMutex.lock();
        emergencyStop = false;
        emergencyStopMutex.unlock();
        time.restart();
        time.start();
        resultImage = controller->getRayTracer()->render(
                camPos,
                viewDirection,
                upVector,
                rightVector,
                fieldOfView,
                aspectRatio,
                screenWidth,
                screenHeight);
        setChanged(RENDER_CHANGED);
        controller->threadSetElapsed(time.elapsed());
        optimalDone = controller->threadImproveRenderingQuality();
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
    hasToRedrawMutex.lock();
    haveToRedraw |= this->camPos != camPos;
    if (!haveToRedraw) {
        emergencyStopMutex.lock();
        haveToRedraw = emergencyStop;
        emergencyStopMutex.unlock();
    }
    if (haveToRedraw) {
        if (controller->getWindowModel()->isRealTime()) {
            controller->threadSetDurtiestRenderingQuality();
        } else {
            controller->threadSetBestRenderingQuality();
        }
    }
    prepare(camPos, viewDirection, upVector, rightVector, fieldOfView, aspectRatio, screenWidth, screenHeight);
    hasToRedrawMutex.unlock();
    start();
}

bool RenderThread::isRendering() const {
    bool result = isRunning();
    if (result) {
        //reallyWorkingMutex.lock();
        result = reallyWorking;
        //reallyWorkingMutex.unlock();
    }
    return result;
}

bool RenderThread::hasRendered() {
    return isFinished();
}

void RenderThread::setPercent(float p)
{
    percent = p;
    setChanged(RENDER_CHANGED);
}

void RenderThread::stopRendering() {
    setChanged(RENDER_CHANGED);
    // Don't use mutexes to be immediate
    emergencyStop = true;
    haveToRedraw = true;
    quit();
}

const QImage &RenderThread::getLastRendered() {
    return resultImage;
}

bool RenderThread::isEmergencyStop() const {
    bool result;
    //emergencyStopMutex.lock();
    result = emergencyStop;
    //emergencyStopMutex.unlock();

    return result;
}

void RenderThread::hasToRedraw() {
    hasToRedrawMutex.lock();
    haveToRedraw = true;
    hasToRedrawMutex.unlock();
    setChanged(RENDER_CHANGED);
}
