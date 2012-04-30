#include "WindowModel.h"

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
