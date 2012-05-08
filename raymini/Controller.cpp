#include "Controller.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>

using namespace std;

Controller::Controller(QApplication *r):
    raymini(r)
{}

Controller::~Controller()
{
    delete scene;
    delete rayTracer;
    delete windowModel;
}

void Controller::initAll(int argc, char **argv) {
    scene = new Scene(this, argc, argv);
    rayTracer = new RayTracer(this);
    windowModel = new WindowModel(this);
    renderThread = new RenderThread(this);

    // do this after Scene and RayTracer
    pbgi = new PBGI(this);

    window = new Window(this);
    window->setWindowTitle("RayMini: A minimal raytracer.");
    connect(raymini, SIGNAL(lastWindowClosed()), this, SLOT(quitProgram()));

    viewer = new GLViewer(this);
    window->setCentralWidget(viewer);

    scene->addObserver(window);
    scene->addObserver(viewer);
    rayTracer->addObserver(window);
    rayTracer->addObserver(viewer);
    windowModel->addObserver(window);
    windowModel->addObserver(viewer);
    renderThread->addObserver(window);

    window->show();

    // First notification
    scene->notifyAll();
    rayTracer->notifyAll();
    windowModel->notifyAll();
}

void Controller::ensureThreadStopped() {
    if (renderThread->isRendering()) {
        renderThread->stopRendering();
    }
}

/******************************************
 ***************** SLOTS ******************
 ******************************************/

void Controller::quitProgram() {
    ensureThreadStopped();
    renderThread->wait();
    raymini->quit();
}

void Controller::windowSetShadowMode(int i) {
    ensureThreadStopped();
    switch(i) {
    case 0:
        rayTracer->setShadowMode(Shadow::NONE);
        break;
    case 1:
        rayTracer->setShadowMode(Shadow::HARD);
        break;
    case 2:
        rayTracer->setShadowMode(Shadow::SOFT);
        break;
    }
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetShadowNbRays (int i) {
    ensureThreadStopped();
    rayTracer->setShadowNbImpulse(i);
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetRayTracerMode (bool b) {
    ensureThreadStopped();
    rayTracer->mode = (b) ? RayTracer::Mode::PBGI_MODE : RayTracer::RAY_TRACING_MODE;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::threadRenderRayImage() {
    // To avoid dark bands
    if (!renderThread->isEmergencyStop()) {
        viewerSetRayImage(renderThread->getLastRendered());
        viewerSetDisplayMode(WindowModel::RayDisplayMode);
    }
    windowModel->handleRealTime();
    renderThread->notifyAll();
}

void Controller::threadSetElapsed(int e)  {
    windowModel->setElapsedTime(e);
}

void Controller::threadSetBestRenderingQuality() {
    rayTracer->quality = RayTracer::Quality::OPTIMAL;
    rayTracer->qualityDivider = 1;
}

void Controller::threadSetDurtiestRenderingQuality() {
    rayTracer->quality = rayTracer->durtiestQuality;
    rayTracer->qualityDivider = rayTracer->durtiestQualityDivider;
}

bool Controller::threadImproveRenderingQuality() {
    if (rayTracer->quality == RayTracer::Quality::OPTIMAL) {
        return true;
    }
    if (rayTracer->quality == RayTracer::Quality::BASIC) {
        rayTracer->quality = RayTracer::Quality::OPTIMAL;
        rayTracer->qualityDivider = 1;
    }
    // Logarithmic progression
    else if (rayTracer->quality == RayTracer::Quality::ONE_OVER_X) {
        rayTracer->qualityDivider /= 2;
        if (rayTracer->qualityDivider <= 1) {
            rayTracer->qualityDivider = 1;
            rayTracer->quality = RayTracer::Quality::BASIC;
        }
    }
    return false;
}

void Controller::windowStopRendering() {
    ensureThreadStopped();
    windowSetRealTime(false);
    renderThread->hasToRedraw();
    renderThread->notifyAll();
}

void Controller::renderProgressed(float percent) {
    renderThread->setPercent(percent);
    renderThread->notifyAll();
}

void Controller::windowRenderRayImage () {
    ensureThreadStopped();
    Vec3Df camPos, viewDirection, upVector, rightVector;
    float fieldOfView, aspectRatio, screenWidth, screenHeight;
    viewer->getCameraInformation(fieldOfView, aspectRatio, screenWidth, screenHeight, camPos, viewDirection, upVector, rightVector);
    renderThread->startRendering(camPos, viewDirection, upVector, rightVector,
            fieldOfView, aspectRatio, screenWidth, screenHeight);
}

Vec3Df Controller::userSelectsColor(Vec3Df programColor) {
    Vec3Df bg = 255*programColor;
    QColor c = QColorDialog::getColor(QColor(bg[0], bg[1], bg[2]), window);
    if (!c.isValid()) {
        return Vec3Df(-1, -1, -1);
    }
    return Vec3Df(c.red()/255.f, c.green()/255.f, c.blue()/255.f);
}


void Controller::windowSetBGColor () {
    Vec3Df c = userSelectsColor(rayTracer->getBackgroundColor());
    if (c[0] != -1) {
        ensureThreadStopped();
        rayTracer->setBackgroundColor(c);
        renderThread->hasToRedraw();
        rayTracer->notifyAll();
    }
}

void Controller::windowShowRayImage () {
    viewerSetDisplayMode(WindowModel::RayDisplayMode);
    windowModel->notifyAll();
}

void Controller::windowExportGLImage () {
    viewer->saveSnapshot(false, false);
    // Nothing modified
}

void Controller::windowExportRayImage () {
    QString filename = QFileDialog::getSaveFileName (window,
                                                     "Save ray-traced image",
                                                     ".",
                                                     "*.jpg *.bmp *.png");
    if (!filename.isNull () && !filename.isEmpty ()) {
        // HACK: for some reason, saved image is fliped
        QImage fliped(windowModel->getRayImage().mirrored(false, true));
        fliped.save(filename);
    }
    // Nothing modified
}

void Controller::windowAbout () {
    QMessageBox::about (window,
                        "About This Program",
                        "<b>RayMini</b> by: <br> <i>Tamy Boubekeur <br> Axel Schumacher <br> Bertrand Chazot <br> Samuel Mokrani</i>.");
    // Nothing modified
}

void Controller::windowChangeAntiAliasingType(int index) {
    ensureThreadStopped();
    AntiAliasing::Type type;

    switch (index)
        {
        default:
        case 0:
            type = AntiAliasing::NONE;
            break;
        case 1:
            type = AntiAliasing::UNIFORM;
            break;
        case 2:
            type = AntiAliasing::POLYGONAL;
            break;
        case 3:
            type = AntiAliasing::STOCHASTIC;
            break;
        }
    rayTracer->typeAntiAliasing = type;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetNbRayAntiAliasing(int i) {
    ensureThreadStopped();
    rayTracer->nbRayAntiAliasing = i;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowChangeAmbientOcclusionNbRays(int index) {
    ensureThreadStopped();
    rayTracer->nbRayAmbientOcclusion = index;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetAmbientOcclusionMaxAngle(int i) {
    ensureThreadStopped();
    rayTracer->maxAngleAmbientOcclusion = (float)i*2.0*M_PI/360.0;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetAmbientOcclusionRadius(double f) {
    ensureThreadStopped();
    rayTracer->radiusAmbientOcclusion = f;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetAmbientOcclusionIntensity(int i) {
    ensureThreadStopped();
    rayTracer->intensityAmbientOcclusion = float(i)/100;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetAmbientOcclusionNbRays(int i) {
    ensureThreadStopped();
    rayTracer->nbRayAmbientOcclusion = i;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetOnlyAO(bool b) {
    ensureThreadStopped();
    rayTracer->onlyAmbientOcclusion = b;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetFocusType(int type) {
    ensureThreadStopped();
    rayTracer->typeFocus = static_cast<Focus::Type>(type);
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetFocusNbRays(int n) {
    ensureThreadStopped();
    rayTracer->nbRayFocus = n;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetFocusAperture(double a) {
    ensureThreadStopped();
    rayTracer->apertureFocus = a;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetFocalFixing(bool isFocusMode) {
    if (rayTracer->typeFocus == Focus::NONE) {
        cerr <<__FUNCTION__<< ": There is no point to change WindowModel focus mode !"<<endl;
        return;
    }
    windowModel->setFocusMode(!isFocusMode);
    windowModel->notifyAll();
}

void Controller::viewerSetFocusPoint(Vertex point) {
    ensureThreadStopped();
    if (rayTracer->typeFocus == Focus::NONE || !windowModel->isFocusMode()) {
        cerr <<__FUNCTION__<< ": There is no point to define a focal !"<<endl;
        return;
    }
    windowModel->setFocusPoint(point);
    renderThread->hasToRedraw();
    windowModel->notifyAll();
}

void Controller::viewerSetShowSurfel(bool s) {
    windowModel->setShowSurfels(s);
    windowModel->notifyAll();
}

void Controller::viewerSetShowKDTree(bool s) {
    windowModel->setShowKDTree(s);
    windowModel->notifyAll();
}

void Controller::windowSetDepthPathTracing(int i) {
    ensureThreadStopped();
    rayTracer->depthPathTracing = i;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetNbRayPathTracing(int i) {
    ensureThreadStopped();
    rayTracer->nbRayPathTracing = i;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}
void Controller::windowSetMaxAnglePathTracing(int i) {
    ensureThreadStopped();
    rayTracer->maxAnglePathTracing = (float)i*2.0*M_PI/360.0;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetIntensityPathTracing(int i) {
    ensureThreadStopped();
    rayTracer->intensityPathTracing = float(i);
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetNbImagesSpinBox(int i) {
    ensureThreadStopped();
    rayTracer->nbPictures = i;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSetOnlyPT(bool b) {
    ensureThreadStopped();
    rayTracer->onlyPathTracing = b;
    renderThread->hasToRedraw();
    rayTracer->notifyAll();
}

void Controller::windowSelectMaterial(int m) {
    windowModel->setSelectedMaterialIndex(m-1);
    windowModel->notifyAll();
}

void Controller::windowSetMaterialDiffuse(double d) {
    ensureThreadStopped();
    int m = windowModel->getSelectedMaterialIndex();
    if (m == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[m]->setDiffuse(d);
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetMaterialSpecular(double s) {
    ensureThreadStopped();
    int m = windowModel->getSelectedMaterialIndex();
    if (m == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[m]->setSpecular(s);
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetMaterialGlossyRatio(double g) {
    ensureThreadStopped();
    int m = windowModel->getSelectedMaterialIndex();
    if (m == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[m]->setGlossyRatio(g);
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetMaterialTexture(int index) {
    ensureThreadStopped();
    int o = windowModel->getSelectedMaterialIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[o]->setTexture(scene->getTextures()[index]);
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSelectTexture(int t) {
    windowModel->setSelectedTextureIndex(t-1);
    windowModel->notifyAll();
}

void Controller::windowSetTextureColor() {
    int t = windowModel->getSelectedTextureIndex();
    if (t == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    Texture *texture = scene->getTextures()[t];
    Vec3Df c = userSelectsColor(texture->getRepresentativeColor());
    if (c[0] != -1) {
        ensureThreadStopped();
        texture->setRepresentativeColor(c);
        renderThread->hasToRedraw();
        scene->notifyAll();
    }
}

void Controller::windowSelectObject(int o) {
    windowModel->setSelectedObjectIndex(o-1);
    windowModel->notifyAll();
}

void Controller::windowEnableObject(bool enabled) {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    if (!windowModel->isRealTime()) {
        viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    }
    scene->getObjects()[o]->setEnabled(enabled);
    scene->updateBoundingBox();
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetObjectPos() {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->setTrans(window->getObjectPos());
    scene->updateBoundingBox();
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetObjectMobile() {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->setMobile(window->getObjectMobile());
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetObjectMaterial(int index) {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->setMaterial(scene->getMaterials()[index]);
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSelectLight(int l) {
    windowModel->setSelectedLightIndex(l-1);
    windowModel->notifyAll();
}

void Controller::windowEnableLight(bool enabled) {
    ensureThreadStopped();
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    if (!windowModel->isRealTime()) {
        viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    }
    scene->getLights()[l]->setEnabled(enabled);
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetLightRadius(double r) {
    ensureThreadStopped();
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    if (!windowModel->isRealTime()) {
        viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    }
    scene->getLights()[l]->setRadius(r);
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetLightIntensity(double i) {
    ensureThreadStopped();
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    if (!windowModel->isRealTime()) {
        viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    }
    scene->getLights()[l]->setIntensity(i);
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetLightPos() {
    ensureThreadStopped();
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    scene->getLights()[l]->setPos(window->getLightPos());
    renderThread->hasToRedraw();
    scene->notifyAll();
}

void Controller::windowSetLightColor() {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    Light *light = scene->getLights()[l];
    Vec3Df c = userSelectsColor(light->getColor());
    if (c[0] != -1) {
        ensureThreadStopped();
        light->setColor(c);
        renderThread->hasToRedraw();
        scene->notifyAll();
    }
}

void Controller::windowSetRealTime(bool r) {
    windowModel->setRealTime(r);
    if (r) {
        renderThread->hasToRedraw();
        windowRenderRayImage();
    }
    else {
        ensureThreadStopped();
    }

    windowModel->notifyAll();
}

void Controller::windowSetDurtiestQuality(int quality) {
    rayTracer->durtiestQuality = static_cast<RayTracer::Quality>(quality);
    rayTracer->notifyAll();
}

void Controller::windowSetQualityDivider(int divider) {
    rayTracer->durtiestQualityDivider = divider;
    rayTracer->notifyAll();
}

void Controller::windowUpdatePBGI() {
    pbgi->update();
    windowModel->notifyAll();
    scene->notifyAll();
}

void Controller::viewerSetWireframe(bool b) {
    windowModel->setWireframe(b);
    windowModel->notifyAll();
}

void Controller::viewerSetRenderingMode(WindowModel::RenderingMode m) {
    windowModel->setRenderingMode(m);
    windowModel->notifyAll();
}

void Controller::viewerSetRenderingMode(int m) {
    windowModel->setRenderingMode(static_cast<WindowModel::RenderingMode>(m));
    windowModel->notifyAll();
}

void Controller::viewerSetDisplayMode(WindowModel::DisplayMode m) {
    windowModel->setDisplayMode(m);
    windowModel->notifyAll();
}

void Controller::viewerSetDisplayMode(int m) {
    windowModel->setDisplayMode(static_cast<WindowModel::DisplayMode>(m));
    windowModel->notifyAll();
}

void Controller::viewerSetRayImage(const QImage & image) {
    windowModel->setRayImage(image);
    windowModel->notifyAll();
}

void Controller::windowSetDragEnabled(bool e) {
    windowModel->setDragEnabled(e);
    if (!e) {
        windowModel->setDraggedObject(nullptr);
    }
    windowModel->notifyAll();
}

void Controller::viewerStartsDragging(Object *o, Vec3Df i, QPoint p, float r) {
    windowModel->setDraggedObject(o);
    windowModel->setInitialDraggedObjectPosition(i);
    windowModel->setStartedDraggingPoint(p);
    windowModel->setMovingRatio(r);
    windowModel->notifyAll();
}

void Controller::viewerMovesWhileDragging(QPoint p) {
    float fov, ar, screenWidth, screenHeight;
    Vec3Df camPos;
    Vec3Df viewDirection;
    Vec3Df upVector;
    Vec3Df rightVector;
    viewer->getCameraInformation(fov, ar, screenWidth, screenHeight, camPos, viewDirection, upVector, rightVector);
    QPoint lastPos = windowModel->getStartedDraggingPoint();
    Vec3Df oPos = windowModel->getInitialDraggedObjectPosition();
    float ratio = windowModel->getMovingRatio();
    float xMove = (float)(p.x()-lastPos.x())/(float)screenWidth/ratio;
    float yMove = (float)(lastPos.y()-p.y())/(float)screenHeight/ratio;
    Object *o = windowModel->getDraggedObject();
    o->setTrans(oPos+rightVector*xMove+upVector*yMove);
    scene->notifyAll();
}

void Controller::viewerStopsDragging() {
    windowModel->setDraggedObject(nullptr);
    windowModel->notifyAll();
}
