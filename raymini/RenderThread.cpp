#include "RenderThread.h"

#include "Controller.h"

RenderThread::RenderThread(Controller *c): controller(c), emergencyStop(false) {
    connect(this, SIGNAL(finished()), controller, SLOT(threadRenderRayImage()));
}

void RenderThread::run() {
    resultImage = controller->getRayTracer()->render(
            camPos,
            viewDirection,
            upVector,
            rightVector,
            fieldOfView,
            aspectRatio,
            screenWidth,
            screenHeight);
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
    prepare(camPos, viewDirection, upVector, rightVector, fieldOfView, aspectRatio, screenWidth, screenHeight);
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
