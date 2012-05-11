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

    // Const to ensure that views use controller to modify models
    inline const Scene *getScene() {return scene;}
    inline const RayTracer *getRayTracer() {return rayTracer;}
    inline const PBGI *getPBGI() {return pbgi;}
    inline const WindowModel *getWindowModel() {return windowModel;}
    inline const RenderThread *getRenderThread() {return renderThread;}

    /** To use with caution */
    inline void forceThreadUpdate() {
        if (renderThread->isReallyWorking()) {
            ensureThreadStopped();
        }
    }

public slots :
    void windowRenderRayImage(); // Won't notify
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
    void windowSetAmbientOcclusionNbRays(int);
    void windowSetOnlyAO(bool);
    void windowSetFocusType(int);
    void windowSetFocusNbRays(int);
    void windowSetFocusAperture(double);
    void windowSetFocalFixing(bool);
    void windowSetDepthPathTracing(int);
    void windowSetNbRayPathTracing(int);
    void windowSetIntensityPathTracing(double);
    void windowSetOnlyPT(bool);
    void windowSetNbImagesSpinBox(int);
    void windowSelectLight(int);
    void windowAddLight();
    void windowEnableLight(bool);
    void windowSetLightRadius(double);
    void windowSetLightIntensity(double);
    void windowSetLightPos();
    void windowSetLightColor();
    void windowSelectObject(int);
    void windowEnableObject(bool);
    void windowSetObjectName(const QString &);
    void windowSetObjectPos();
    void windowSetObjectMobile();
    void windowSetObjectMaterial(int);
    void windowSelectMaterial(int);
    void windowSetMaterialName(const QString &);
    void windowSetMaterialDiffuse(double);
    void windowSetMaterialSpecular(double);
    void windowSetMaterialGlossyRatio(double);
    void windowSetMaterialColorTexture(int);
    void windowSetMaterialNormalTexture(int);
    void windowSetMaterialGlassAlpha(double);
    void windowSelectColorTexture(int);
    void windowSetColorTextureColor();
    void windowSetColorTextureName(const QString &);
    void windowChangeColorTextureType(int);
    void windowChangeColorImageTextureFile();
    void windowSetNoiseColorTextureFunction(int);
    void windowSelectNormalTexture(int);
    void windowSetNormalTextureName(const QString &);
    void windowChangeNormalTextureType(int);
    void windowChangeNormalImageTextureFile();
    void windowSetNoiseNormalTextureFunction(int);
    void windowSetNoiseNormalTextureOffset();
    void windowSetRealTime(bool);
    void windowSetDurtiestQuality(int);
    void windowSetQualityDivider(int);
    void windowUpdatePBGI();
    void windowSetDragEnabled(bool);
    void windowSetUScale(double);
    void windowSetVScale(double);
    void windowSetSquareMapping();
    void windowSetSphericalMapping();
    void windowSetCubicMapping();

    void viewerSetWireframe(bool b);
    void viewerSetRenderingMode(WindowModel::RenderingMode m);
    void viewerSetRenderingMode(int m);
    void viewerSetDisplayMode(WindowModel::DisplayMode m);
    void viewerSetDisplayMode(int m);
    void viewerSetRayImage(const QImage & image);
    void viewerSetFocusPoint(Vertex point);
    void viewerSetShowSurfel(bool);
    void viewerSetShowKDTree(bool);
    void viewerMovesMouse();

    void threadRenderRayImage();
    // Won't notify ****
    void threadSetElapsed(int);
    // *****************

    void renderProgressed(float);

    void quitProgram();

public:
    // Won't notify ****
    void threadSetBestRenderingQuality();
    void threadSetDurtiestRenderingQuality();
    /** Return true iff quality was already optimal */
    bool threadImproveRenderingQuality();
    // *****************

    void viewerStartsDragging(Object *o, Vec3Df i, QPoint p, float ratio);
    void viewerMovesWhileDragging(QPoint);
    void viewerStopsDragging();

    // Won't notify ****
    void setRayTracerQuality(RayTracer::Quality quality);
    void setSceneMove(int nbPictures);
    void setSceneReset();
    // *****************

    /**
     * Ask for a color and return a [0,1] color, or -1 on each field in not valid
     * programColor is a [0,1] color
     */
    Vec3Df userSelectsColor(Vec3Df programColor);

private:
    /** All models notify if necessary */
    void notifyAll();

    /** Stop thread if running */
    void ensureThreadStopped();

    // Views
    std::vector<Observer*> views;
    Window *window;
    GLViewer *viewer;

    // Models
    std::vector<Observable*> models;
    Scene *scene;
    RayTracer *rayTracer;
    WindowModel *windowModel;
    PBGI * pbgi;
    RenderThread *renderThread;

    // QApplication
    QApplication *raymini;
};
