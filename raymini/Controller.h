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

class Controller : public QObject {
    Q_OBJECT
public:
    Controller(QApplication *r);
    virtual ~Controller();

    /** Start the whole procedure */
    void initAll(int argc, char **argv);

    inline Window *getWindow() {return window;}
    inline GLViewer *getViewer() {return viewer;}

    inline Scene *getScene() {return scene;}
    inline RayTracer *getRayTracer() {return rayTracer;}
    inline PBGI *getPBGI() {return pbgi;}
    inline WindowModel *getWindowModel() {return windowModel;}
    inline RenderThread *getRenderThread() {return renderThread;}

    /** To use with caution */
    inline void forceThreadUpdate() {
        if (renderThread->isReallyWorking()) {
            ensureThreadStopped();
        }
    }

public slots :
    void windowRenderRayImage();
    void windowStopRendering();
    void windowSetRayTracerMode(bool);
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
    void windowSetFocusType(int);
    void windowSetFocusNbRays(int);
    void windowSetFocusAperture(double);
    void windowSetFocalFixing(bool);
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
    void windowSetLightColor();
    void windowSelectObject(int);
    void windowEnableObject(bool);
    void windowSetObjectPos();
    void windowSetObjectMobile();
    void windowSetObjectMaterial(int);
    void windowSelectMaterial(int);
    void windowSetMaterialDiffuse(double);
    void windowSetMaterialSpecular(double);
    void windowSetMaterialColor();
    void windowSetMaterialGlossyRatio(double);
    void windowSetRealTime(bool);
    void windowSetDurtiestQuality(int);
    void windowSetQualityDivider(int);

    void viewerSetWireframe(bool b);
    void viewerSetRenderingMode(WindowModel::RenderingMode m);
    void viewerSetRenderingMode(int m);
    void viewerSetDisplayMode(WindowModel::DisplayMode m);
    void viewerSetDisplayMode(int m);
    void viewerSetRayImage(const QImage & image);
    void viewerSetFocusPoint(Vertex point);

    void threadRenderRayImage();
    void threadSetElapsed(int);

    void renderProgressed(float);

    void quitProgram();

public:
    void threadSetBestRenderingQuality();
    void threadSetDurtiestRenderingQuality();
    // Return true iff quality was already optimal
    bool threadImproveRenderingQuality();

private:
    /** Stop thread if running */
    void ensureThreadStopped();

    // Views
    Window *window;
    GLViewer *viewer;

    // Models
    Scene *scene;
    RayTracer *rayTracer;
    WindowModel *windowModel;
    PBGI * pbgi;
    RenderThread *renderThread;

    // QApplication
    QApplication *raymini;
};
