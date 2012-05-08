#include "WindowModel.h"
#include "Controller.h"

WindowModel::WindowModel(Controller *c):
    controller(c),
    selectedLightIndex(-1),
    selectedObjectIndex(-1),
    selectedMaterialIndex(-1),
    selectedTextureIndex(-1),
    wireframe(false),
    focusMode(false),
    showSurfels(false),
    showKDTree(false),
    renderingMode(SMOOTH),
    displayMode(OpenGLDisplayMode),
    focusPoint(Vec3Df(), Vec3Df(0, 0, 1)),
    realTime(false),
    elapsedTime(0),
    dragEnabled(false),
    draggedObject(nullptr)
{}

WindowModel::~WindowModel() {
}

void WindowModel::setFocusMode(bool f) {
    focusMode = (f && controller->getRayTracer()->typeFocus != Focus::NONE);
}

void WindowModel::handleRealTime() {
    RenderThread *renderThread = controller->getRenderThread();
    RayTracer *rayTracer = controller->getRayTracer();
    bool isRendering = renderThread->isRendering();
    if (realTime && !isRendering) {
        controller->windowRenderRayImage();
    }
    if (!realTime) {
        rayTracer->quality = RayTracer::Quality::OPTIMAL;
    }
}
