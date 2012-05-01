#include "WindowModel.h"
#include "Controller.h"

WindowModel::WindowModel(Controller *c):
    controller(c),
    selectedLightIndex(-1),
    selectedObjectIndex(-1),
    wireframe(false),
    focusMode(false),
    renderingMode(SMOOTH),
    displayMode(OpenGLDisplayMode),
    focusPoint(Vec3Df(), Vec3Df(0, 0, 1))
{}

WindowModel::~WindowModel() {
}

void WindowModel::setFocusMode(bool f) {
    focusMode = (f && controller->getRayTracer()->isFocus());
}
