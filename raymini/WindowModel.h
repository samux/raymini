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
    static const unsigned long SELECTED_LIGHT_CHANGED    = 1<<0;
    static const unsigned long SELECTED_MATERIAL_CHANGED = 1<<1;
    static const unsigned long SELECTED_OBJECT_CHANGED   = 1<<2;
    static const unsigned long SELECTED_TEXTURE_CHANGED  = 1<<3;
    static const unsigned long WIREFRAME_CHANGED         = 1<<4;
    static const unsigned long SHOW_SURFELS_CHANGED      = 1<<5;
    static const unsigned long SHOW_KDTREE_CHANGED       = 1<<6;
    static const unsigned long RENDERING_MODE_CHANGED    = 1<<7;
    static const unsigned long DISPLAY_MODE_CHANGED      = 1<<8;
    static const unsigned long RAY_IMAGE_CHANGED         = 1<<9;
    static const unsigned long FOCUS_POINT_CHANGED       = 1<<10;
    static const unsigned long FOCUS_MODE_CHANGED        = 1<<11;
    static const unsigned long REAL_TIME_CHANGED         = 1<<12;
    static const unsigned long ELAPSED_TIME_CHANGED      = 1<<13;
    static const unsigned long DRAGGED_OBJECT_CHANGED    = 1<<14;
    static const unsigned long DRAG_ENABLED_CHANGED      = 1<<14;

    WindowModel(Controller *);
    ~WindowModel();

    typedef enum {SMOOTH=0, FLAT=1} RenderingMode;
    typedef enum {OpenGLDisplayMode=0, RayDisplayMode=1} DisplayMode;

    inline int getSelectedLightIndex() const {return selectedLightIndex;}
    /** Change SELECTED_LIGHT_CHANGED */
    inline void setSelectedLightIndex(int index) {
        selectedLightIndex = index;
        setChanged(SELECTED_LIGHT_CHANGED);
    }

    inline int getSelectedObjectIndex() const {return selectedObjectIndex;}
    /** Change SELECTED_OBJECT_CHANGED */
    inline void setSelectedObjectIndex(int index) {
        selectedObjectIndex = index;
        setChanged(SELECTED_OBJECT_CHANGED);
    }
    /** Change SELECTED_OBJECT_CHANGED */
    void setSelectedObject(Object *o);

    inline int getSelectedMaterialIndex() const {return selectedMaterialIndex;}
    /** Change SELECTED_MATERIAL_CHANGED */
    inline void setSelectedMaterialIndex(int index) {
        selectedMaterialIndex = index;
        setChanged(SELECTED_MATERIAL_CHANGED);
    }

    inline int getSelectedTextureIndex() const {return selectedTextureIndex;}
    /** Change SELECTED_TEXTURE_CHANGED */
    inline void setSelectedTextureIndex(int index) {
        selectedTextureIndex = index;
        setChanged(SELECTED_TEXTURE_CHANGED);
    }

    inline bool isWireframe() const {return wireframe;}
    /** Change WIREFRAME_CHANGED */
    inline void setWireframe(bool w) {
        wireframe = w;
        setChanged(WIREFRAME_CHANGED);
    }
    inline bool isShowSurfel() const {return showSurfels;}
    /** Change SHOW_SURFELS_CHANGED */
    inline void setShowSurfels(bool s) {
        showSurfels = s;
        setChanged(SHOW_SURFELS_CHANGED);
    }
    inline bool isShowKDTree() const {return showKDTree;}
    /** Change SHOW_KDTREE_CHANGED */
    inline void setShowKDTree(bool s) {
        showKDTree = s;
        setChanged(SHOW_KDTREE_CHANGED);
    }
    inline int getRenderingMode() const {return renderingMode;}
    /** Change RENDERING_MODE_CHANGED */
    inline void setRenderingMode(RenderingMode r) {
        renderingMode = r;
        setChanged(RENDERING_MODE_CHANGED);
    }
    inline int getDisplayMode() const {return displayMode;}
    /** Change DISPLAY_MODE_CHANGED */
    inline void setDisplayMode(DisplayMode d) {
        displayMode = d;
        setChanged(DISPLAY_MODE_CHANGED);
    }
    inline const QImage & getRayImage() const {return rayImage;}
    /** Change RAY_IMAGE_CHANGED */
    inline void setRayImage(QImage i) {
        rayImage = i;
        setChanged(RAY_IMAGE_CHANGED);
    }
    inline bool isFocusMode() const {return focusMode;}
    inline Vertex getFocusPoint() const { return focusPoint; }
    /** Change FOCUS_POINT_CHANGED */
    inline void setFocusPoint(Vertex f) {
        focusPoint = f;
        setChanged(FOCUS_POINT_CHANGED);
    }

    /** Change FOCUS_MODE_CHANGED */
    void setFocusMode(bool f);

    /** Change REAL_TIME_CHANGED */
    inline void setRealTime(bool r) {
        realTime = r;
        setChanged(REAL_TIME_CHANGED);
    }
    inline bool isRealTime() const {return realTime;}

    /** Change ELAPSED_TIME_CHANGED */
    inline void setElapsedTime(int e) {
        elapsedTime = e;
        setChanged(ELAPSED_TIME_CHANGED);
    }
    inline int getElapsedTime() const {return elapsedTime;}

    /** Change DRAGGED_OBJECT_CHANGED */
    inline void setDraggedObject(Object *o, Vec3Df initialPosition, QPoint startedDragging, float ratio) {
        draggedObject = o;
        initialDraggedObjectPosition=initialPosition;
        startedDraggingPoint = startedDragging;
        movingRatio = ratio;
        setChanged(DRAGGED_OBJECT_CHANGED);
    }
    inline Object * getDraggedObject() {return draggedObject;}
    inline QPoint getStartedDraggingPoint() const {return startedDraggingPoint;}
    inline float getMovingRatio() const {return movingRatio;}
    inline Vec3Df getInitialDraggedObjectPosition() const {return initialDraggedObjectPosition;}
    inline bool isDragging() const {
        return draggedObject != nullptr;
    }
    /** Change DRAGGED_OBJECT_CHANGED */
    inline void stopDragging() {
        draggedObject = nullptr;
        setChanged(DRAGGED_OBJECT_CHANGED);
    }

    /** Change DRAG_ENABLED_CHANGED */
    inline void setDragEnabled(bool e) {
        dragEnabled = e;
        setChanged(DRAG_ENABLED_CHANGED);
        if (!e) {
            draggedObject = nullptr;
            setChanged(DRAGGED_OBJECT_CHANGED);
        }
    }
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
