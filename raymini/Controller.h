/**
 * Handle any model modification, via the SLOTS, to ensure coherent behaviour
 */

#pragma once

#include <QApplication>
#include <QObject>

#include "Window.h"
#include "WindowModel.h"
#include "RayTracer.h"
#include "Scene.h"
#include "GLViewer.h"
#include "PBGI.h"

/*class Controller: public QApplication {*/
class Controller : public QObject {
    Q_OBJECT
public:
    Controller(QApplication *r);
    virtual ~Controller();

    /** Start the whole procedure */
    void initAll();

    inline Window *getWindow() {return window;}
    inline GLViewer *getViewer() {return viewer;}

    inline Scene *getScene() {return scene;}
    inline RayTracer *getRayTracer() {return rayTracer;}
    inline PBGI *getPBGI() {return pbgi;}
    inline WindowModel *getWindowModel() {return windowModel;}

public slots :
    void windowRenderRayImage();
    void windowSetShadowMode(int);
    void windowSetShadowNbRays(int);
    void windowSetBGColor();
    void windowShowRayImage();
    void windowExportGLImage();
    void windowExportRayImage();
    void windowAbout();
    void windowChangeAntiAliasingType(int index);
    void windowSetNbRayAntiAliasing(int);
    void windowChangeAmbientOcclusionNbRays(int index);
    void windowSetAmbientOcclusionMaxAngle(int);
    void windowSetAmbientOcclusionRadius(double);
    void windowSetAmbientOcclusionIntensity(int);
    void windowSetOnlyAO(bool);
    void windowEnableFocal(bool);
    void windowSetFocal();
    void windowSetDepthPathTracing(int);
    void windowSetNbRayPathTracing(int);
    void windowSetMaxAnglePathTracing(int);
    void windowSetIntensityPathTracing(int);
    void windowSetOnlyPT(bool);
    void windowSetNbImagesSpinBox(int);
    void windowSelectLight(int);
    void windowEnableLight(bool);
    void windowSetLightRadius(double);
    void windowSetLightIntensity(double);
    void windowSetLightPos();
    void windowSelectObject(int);
    void windowEnableObject(bool);

    void viewerSetWireframe(bool b);
    void viewerSetRenderingMode(WindowModel::RenderingMode m);
    void viewerSetRenderingMode(int m);
    void viewerSetDisplayMode(WindowModel::DisplayMode m);
    void viewerSetDisplayMode(int m);
    void viewerSetRayImage(const QImage & image);
    void viewerSetFocusPoint(Vertex point);

private:
    // Views
    Window *window;
    GLViewer *viewer;

    // Models
    Scene *scene;
    RayTracer *rayTracer;
    WindowModel *windowModel;
    PBGI * pbgi;

    // QApplication
    QApplication *raymini;
};
