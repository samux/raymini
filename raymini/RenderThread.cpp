#include "RenderThread.h"

#include "Controller.h"

using namespace std;

RenderThread::RenderThread(Controller *c): controller(c), emergencyStop(false) {
    connect(this, SIGNAL(finished()), controller, SLOT(threadRenderRayImage()));
}

void RenderThread::run() {
    hasToRedrawMutex.lock();
    if (hasToRedraw) {
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
        controller->threadSetElapsed(time.elapsed());
    }
    hasToRedraw = false;
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
    hasToRedraw |= this->camPos != camPos;
    prepare(camPos, viewDirection, upVector, rightVector, fieldOfView, aspectRatio, screenWidth, screenHeight);
    hasToRedrawMutex.unlock();
    start();
}
bool RenderThread::isRendering() {
    return isRunning();
}
bool RenderThread::hasRendered() {
    return isFinished();
}
void RenderThread::stopRendering() {
    emergencyStop = true;
    quit();
}
const QImage &RenderThread::getLastRendered() {
    return resultImage;
}
