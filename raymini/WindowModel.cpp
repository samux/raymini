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
    focusMode = (f && controller->getRayTracer()->getTypeFocus() != Focus::NONE);
    setChanged(FOCUS_MODE_CHANGED);
}

void WindowModel::setSelectedObject(Object *o) {
    unsigned int index = 0;
    const Scene *scene = controller->getScene();

    for (Object *object : scene->getObjects()) {
        if (object == o) {
            selectedObjectIndex = index;
            setChanged(SELECTED_OBJECT_CHANGED);
            return;
        }
        index++;
    }
}
