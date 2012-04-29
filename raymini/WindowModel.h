/**
 * A model handling any Window view state
 */

#pragma once

#include <QImage>

#include "Vertex.h"
#include "Observable.h"

class Controller;

class WindowModel: public Observable {
public:
    WindowModel(Controller *);
    ~WindowModel();

    typedef enum {SMOOTH=0, FLAT=1} RenderingMode;
    typedef enum {OpenGLDisplayMode=0, RayDisplayMode=1} DisplayMode;

    inline int getSelectedLightIndex() const {return selectedLightIndex;}
    inline bool isWireframe() const {return wireframe;}
    inline void setWireframe(bool w) {wireframe = w;}
    inline int getRenderingMode() const {return renderingMode;}
    inline void setRenderingMode(RenderingMode r) {renderingMode = r;}
    inline int getDisplayMode() const {return displayMode;}
    inline void setDisplayMode(DisplayMode d) {displayMode = d;}
    inline const QImage & getRayImage() const {return rayImage;}
    inline void setRayImage(QImage i) {rayImage = i;}
    inline bool isFocusMode() const {return focusMode;}
    inline void setFocusMode(bool f) {focusMode = f;}
    inline Vec3Df getFocusPoint() { return focusPoint.getPos(); }
    inline void setFocusPoint(Vertex f) {focusPoint = f;}

private:
    Controller *controller;

    int selectedLightIndex;
    bool wireframe;
    bool focusMode;
    RenderingMode renderingMode;
    DisplayMode displayMode;
    QImage rayImage;
    Vertex focusPoint;
};
