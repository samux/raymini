/**
 * A model handling any Window view state
 */

#pragma once

#include <QImage>

#include "Vertex.h"
#include "Observable.h"

class Controller;
class Object;

class WindowModel: public Observable {
public:
    WindowModel(Controller *);
    ~WindowModel();

    typedef enum {SMOOTH=0, FLAT=1} RenderingMode;
    typedef enum {OpenGLDisplayMode=0, RayDisplayMode=1} DisplayMode;

    inline int getSelectedLightIndex() const {return selectedLightIndex;}
    inline void setSelectedLightIndex(int index) {selectedLightIndex = index;}

    inline int getSelectedObjectIndex() const {return selectedObjectIndex;}
    inline void setSelectedObjectIndex(int index) {selectedObjectIndex = index;}

    inline int getSelectedMaterialIndex() const {return selectedMaterialIndex;}
    inline void setSelectedMaterialIndex(int index) {selectedMaterialIndex = index;}

    inline int getSelectedTextureIndex() const {return selectedTextureIndex;}
    inline void setSelectedTextureIndex(int index) {selectedTextureIndex = index;}

    inline bool isWireframe() const {return wireframe;}
    inline void setWireframe(bool w) {wireframe = w;}
    inline bool isShowSurfel() const {return showSurfels;}
    inline void setShowSurfels(bool s) {showSurfels = s;}
    inline bool isShowKDTree() const {return showKDTree;}
    inline void setShowKDTree(bool s) {showKDTree = s;}
    inline int getRenderingMode() const {return renderingMode;}
    inline void setRenderingMode(RenderingMode r) {renderingMode = r;}
    inline int getDisplayMode() const {return displayMode;}
    inline void setDisplayMode(DisplayMode d) {displayMode = d;}
    inline const QImage & getRayImage() const {return rayImage;}
    inline void setRayImage(QImage i) {rayImage = i;}
    inline bool isFocusMode() const {return focusMode;}
    inline Vertex getFocusPoint() { return focusPoint; }
    inline void setFocusPoint(Vertex f) {focusPoint = f;}

    void setFocusMode(bool f);

    inline void setRealTime(bool r) {realTime = r;}
    inline bool isRealTime() const {return realTime;}

    void handleRealTime();

    inline void setElapsedTime(int e) {elapsedTime = e;}
    inline int getElapsedTime() const {return elapsedTime;}

    inline void setDraggedObject(Object *o) {draggedObject = o;}
    inline void setInitialDraggedObjectPosition(Vec3Df p) {initialDraggedObjectPosition=p;}
    inline void setStartedDraggingPoint(QPoint p) {startedDraggingPoint = p;}
    inline void setMovingRatio(float r) {movingRatio = r;}
    inline Object * getDraggedObject() {return draggedObject;}
    inline QPoint getStartedDraggingPoint() const {return startedDraggingPoint;}
    inline float getMovingRatio() const {return movingRatio;}
    inline Vec3Df getInitialDraggedObjectPosition() const {return initialDraggedObjectPosition;}

    inline void setDragEnabled(bool e) {dragEnabled = e;}
    inline bool isDragEnabled() const {return dragEnabled;}

private:
    Controller *controller;

    int selectedLightIndex;
    int selectedObjectIndex;
    int selectedMaterialIndex;
    int selectedTextureIndex;
    bool wireframe;
    bool focusMode;
    bool showSurfels;
    bool showKDTree;
    RenderingMode renderingMode;
    DisplayMode displayMode;
    QImage rayImage;
    Vertex focusPoint;
    bool realTime;
    int elapsedTime;

    // Viewer drag and drop
    bool dragEnabled;
    Object *draggedObject;
    Vec3Df initialDraggedObjectPosition;
    QPoint startedDraggingPoint;
    float movingRatio;
};
