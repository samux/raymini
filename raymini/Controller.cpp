#include "Controller.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "NoiseUser.h"

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
    models.push_back(scene);
    rayTracer = new RayTracer(this);
    models.push_back(rayTracer);
    windowModel = new WindowModel(this);
    models.push_back(windowModel);
    renderThread = new RenderThread(this);
    models.push_back(renderThread);

    // do this after Scene and RayTracer
    pbgi = new PBGI(this);
    models.push_back(pbgi);

    window = new Window(this);
    window->setWindowTitle("RayMini: A minimal raytracer.");
    connect(raymini, SIGNAL(lastWindowClosed()), this, SLOT(quitProgram()));
    views.push_back(window);

    viewer = new GLViewer(this);
    window->setCentralWidget(viewer);
    views.push_back(viewer);

    for (Observable *m: models) {
        for (Observer *v: views) {
            m->addObserver(v);
        }
    }

    window->show();

    // First notification
    notifyAll();
}

void Controller::notifyAll() {
    for (Observable *m : models) {
        m->notifyAll();
    }
}

void Controller::ensureThreadStopped() {
    if (renderThread->isRendering()) {
        renderThread->stopRendering();
        renderThread->wait();
    }
}

/******************************************
 ***************** SLOTS ******************
 ******************************************/

void Controller::quitProgram() {
    ensureThreadStopped();
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
    notifyAll();
}

void Controller::windowSetShadowNbRays (int i) {
    ensureThreadStopped();
    rayTracer->setShadowNbImpulse(i);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetRayTracerMode(bool b) {
    ensureThreadStopped();
    rayTracer->setMode(b ? RayTracer::Mode::PBGI_MODE : RayTracer::PATH_TRACING_MODE);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::threadRenderRayImage() {
    // To avoid dark bands
    if (!renderThread->isEmergencyStop()) {
        windowModel->setRayImage(renderThread->getLastRendered());
        windowModel->setDisplayMode(WindowModel::RayDisplayMode);
    }
    bool isRendering = renderThread->isRendering();
    bool realTime = windowModel->isRealTime();
    if (realTime && !isRendering) {
        windowRenderRayImage();
    }
    if (!realTime) {
        setRayTracerQuality(RayTracer::Quality::OPTIMAL);
    }
    renderThread->setChanged(RenderThread::RENDER_CHANGED);
    notifyAll();
}

void Controller::threadSetElapsed(int e)  {
    windowModel->setElapsedTime(e);
}

void Controller::threadSetBestRenderingQuality() {
    rayTracer->setQuality(RayTracer::Quality::OPTIMAL);
    rayTracer->setQualityDivider(1);
}

void Controller::threadSetDurtiestRenderingQuality() {
    rayTracer->setQuality(rayTracer->getDurtiestQuality());
    rayTracer->setQualityDivider(rayTracer->getDurtiestQualityDivider());
}

bool Controller::threadImproveRenderingQuality() {
    if (rayTracer->getQuality() == RayTracer::Quality::OPTIMAL) {
        return true;
    }
    if (rayTracer->getQuality() == RayTracer::Quality::BASIC) {
        rayTracer->setQuality(RayTracer::Quality::OPTIMAL);
        rayTracer->setQualityDivider(1);
    }
    // Logarithmic progression
    else if (rayTracer->getQuality() == RayTracer::Quality::ONE_OVER_X) {
        int divider = rayTracer->getQualityDivider();
        divider /= 2;
        if (divider <= 1) {
            divider = 1;
            rayTracer->setQuality(RayTracer::Quality::BASIC);
        }
        rayTracer->setQualityDivider(divider);
    }
    return false;
}

void Controller::windowStopRendering() {
    ensureThreadStopped();
    windowSetRealTime(false);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::renderProgressed(float percent) {
    renderThread->setPercent(percent);
    notifyAll();
}

void Controller::windowRenderRayImage() {
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
        notifyAll();
    }
}

void Controller::windowShowRayImage () {
    windowModel->setDisplayMode(WindowModel::RayDisplayMode);
    notifyAll();
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
    rayTracer->setTypeAntiAliasing(type);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetNbRayAntiAliasing(int i) {
    ensureThreadStopped();
    rayTracer->setNbRayAntiAliasing(i);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowChangeAmbientOcclusionNbRays(int index) {
    ensureThreadStopped();
    rayTracer->setNbRayAmbientOcclusion(index);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetAmbientOcclusionMaxAngle(int i) {
    ensureThreadStopped();
    rayTracer->setMaxAngleAmbientOcclusion((float)i*2.0*M_PI/360.0);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetAmbientOcclusionRadius(double f) {
    ensureThreadStopped();
    rayTracer->setRadiusAmbientOcclusion(f);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetAmbientOcclusionIntensity(int i) {
    ensureThreadStopped();
    rayTracer->setIntensityAmbientOcclusion(float(i)/100);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetAmbientOcclusionNbRays(int i) {
    ensureThreadStopped();
    rayTracer->setNbRayAmbientOcclusion(i);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetOnlyAO(bool b) {
    ensureThreadStopped();
    rayTracer->setOnlyAmbientOcclusion(b);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetFocusType(int type) {
    ensureThreadStopped();
    rayTracer->setTypeFocus(static_cast<Focus::Type>(type));
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetFocusNbRays(int n) {
    ensureThreadStopped();
    rayTracer->setNbRayFocus(n);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetFocusAperture(double a) {
    ensureThreadStopped();
    rayTracer->setApertureFocus(a);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetFocalFixing(bool isFocusMode) {
    if (rayTracer->getTypeFocus() == Focus::NONE) {
        cerr <<__FUNCTION__<< ": There is no point to change WindowModel focus mode !"<<endl;
        return;
    }
    windowModel->setFocusMode(!isFocusMode);
    notifyAll();
}

void Controller::viewerSetFocusPoint(Vertex point) {
    ensureThreadStopped();
    if (rayTracer->getTypeFocus() == Focus::NONE || !windowModel->isFocusMode()) {
        cerr <<__FUNCTION__<< ": There is no point to define a focal !"<<endl;
        return;
    }
    windowModel->setFocusPoint(point);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::viewerSetShowSurfel(bool s) {
    windowModel->setShowSurfels(s);
    notifyAll();
}

void Controller::viewerSetShowKDTree(bool s) {
    windowModel->setShowKDTree(s);
    notifyAll();
}

void Controller::viewerMovesMouse() {
    if (rayTracer->getQuality() != rayTracer->getDurtiestQuality() ||
            (rayTracer->getQuality() == RayTracer::Quality::ONE_OVER_X &&
             rayTracer->getQualityDivider() != rayTracer->getDurtiestQualityDivider())) {
        ensureThreadStopped();
    }
}

void Controller::windowSetDepthPathTracing(int i) {
    ensureThreadStopped();
    rayTracer->setDepthPathTracing(i);
    rayTracer->setTypeAntiAliasing(AntiAliasing::UNIFORM);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetNbRayPathTracing(int i) {
    ensureThreadStopped();
    rayTracer->setNbRayPathTracing(i);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetIntensityPathTracing(double i) {
    ensureThreadStopped();
    rayTracer->setIntensityPathTracing(i);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetNbImagesSpinBox(int i) {
    ensureThreadStopped();
    rayTracer->setNbPictures(i);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetOnlyPT(bool b) {
    ensureThreadStopped();
    rayTracer->setOnlyPathTracing(b);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSelectMaterial(int m) {
    windowModel->setSelectedMaterialIndex(m-1);
    notifyAll();
}

void Controller::windowSetMaterialName(const QString &n) {
    int o = windowModel->getSelectedMaterialIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[o]->setName(n.toStdString());
    scene->setChanged(Scene::MATERIAL_CHANGED);
    notifyAll();
}


void Controller::windowSetMaterialDiffuse(double d) {
    ensureThreadStopped();
    int m = windowModel->getSelectedMaterialIndex();
    if (m == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[m]->setDiffuse(d);
    scene->setChanged(Scene::MATERIAL_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetMaterialSpecular(double s) {
    ensureThreadStopped();
    int m = windowModel->getSelectedMaterialIndex();
    if (m == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[m]->setSpecular(s);
    scene->setChanged(Scene::MATERIAL_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetMaterialGlossyRatio(double g) {
    ensureThreadStopped();
    int m = windowModel->getSelectedMaterialIndex();
    if (m == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[m]->setGlossyRatio(g);
    scene->setChanged(Scene::MATERIAL_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetMaterialColorTexture(int index) {
    ensureThreadStopped();
    int o = windowModel->getSelectedMaterialIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[o]->setColorTexture(scene->getColorTextures()[index]);
    scene->setChanged(Scene::MATERIAL_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetMaterialNormalTexture(int index) {
    ensureThreadStopped();
    int o = windowModel->getSelectedMaterialIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    scene->getMaterials()[o]->setNormalTexture(scene->getNormalTextures()[index]);
    scene->setChanged(Scene::MATERIAL_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetMaterialGlassAlpha(double a) {
    ensureThreadStopped();
    int o = windowModel->getSelectedMaterialIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though a material hasn't been selected!\n";
        return;
    }
    Glass *glass = dynamic_cast<Glass*>(scene->getMaterials()[o]);
    if (!glass) {
        cerr << __FUNCTION__ << " called even though selected material isn't a Glass!\n";
        return;
    }
    glass->setAlpha(a);;
    scene->setChanged(Scene::MATERIAL_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSelectColorTexture(int t) {
    windowModel->setSelectedColorTextureIndex(t-1);
    notifyAll();
}

void Controller::windowSetColorTextureColor() {
    int t = windowModel->getSelectedColorTextureIndex();
    if (t == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    ColorTexture *texture = scene->getColorTextures()[t];
    Vec3Df c = userSelectsColor(texture->getRepresentativeColor());
    if (c[0] != -1) {
        ensureThreadStopped();
        texture->setRepresentativeColor(c);
        scene->setChanged(Scene::COLOR_TEXTURE_CHANGED);
        renderThread->hasToRedraw();
        notifyAll();
    }
}

void Controller::windowSetColorTextureName(const QString &n) {
    int o = windowModel->getSelectedColorTextureIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though a color texture hasn't been selected!\n";
        return;
    }
    scene->getColorTextures()[o]->setName(n.toStdString());
    scene->setChanged(Scene::COLOR_TEXTURE_CHANGED);
    notifyAll();
}

void Controller::windowChangeColorTextureType(int t) {
    int it = windowModel->getSelectedColorTextureIndex();
    if (it == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    ColorTexture *oldTexture = scene->getColorTextures()[it];
    if (oldTexture->getType() == static_cast<ColorTexture::Type>(t)) {
        return;
    }
    ensureThreadStopped();
    Vec3Df color = oldTexture->getRepresentativeColor();
    string name = oldTexture->getName();
    ColorTexture *texture = nullptr;
    auto type = static_cast<ColorTexture::Type>(t);
    switch (type) {
        case ColorTexture::Type::SingleColor:
            texture = new SingleColorTexture(color, name);
            break;
        case ColorTexture::Type::Debug:
            texture = new DebugColorTexture(name);
            break;
        case ColorTexture::Type::Noise:
            texture = new NoiseColorTexture(
                    color,
                    NoiseUser::Predefined::PERLIN_MARBLE,
                    name);
            break;
        case ColorTexture::Type::Image:
            texture = new ImageColorTexture(
                    "textures/grass.jpg",
                    name);
            break;
    }
    scene->getColorTextures()[it] = texture;
    scene->updateMaterialsColorTexture(oldTexture, texture);
    delete oldTexture;
    scene->setChanged(Scene::COLOR_TEXTURE_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowChangeColorImageTextureFile() {
    int it = windowModel->getSelectedColorTextureIndex();
    if (it == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    ImageColorTexture *texture = dynamic_cast<ImageColorTexture*>(scene->getColorTextures()[it]);
    if (!texture) {
        cerr << __FUNCTION__ << " called even though selected texture is not an ImageColorTexture!\n";
        return;
    }
    QString filename = QFileDialog::getOpenFileName(window,
                                                    "Open a texture image",
                                                    "./textures",
                                                    "*.jpg *.bmp *.png");
    if (!filename.isNull()) {
        ensureThreadStopped();
        texture->loadImage(filename.toStdString().c_str());
        scene->setChanged(Scene::COLOR_TEXTURE_CHANGED);
        renderThread->hasToRedraw();
        notifyAll();
    }
}

void Controller::windowSetNoiseColorTextureFunction(int ip) {
    int it = windowModel->getSelectedColorTextureIndex();
    if (it == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    NoiseColorTexture *texture = dynamic_cast<NoiseColorTexture*>(scene->getColorTextures()[it]);
    if (!texture) {
        cerr << __FUNCTION__ << " called even though selected texture is not a NoiseColorTexture!\n";
        return;
    }
    ensureThreadStopped();
    texture->loadPredefined(static_cast<NoiseUser::Predefined>(ip));
    scene->setChanged(Scene::COLOR_TEXTURE_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSelectNormalTexture(int t) {
    windowModel->setSelectedNormalTextureIndex(t-1);
    notifyAll();
}

void Controller::windowSetNormalTextureName(const QString &n) {
    int o = windowModel->getSelectedNormalTextureIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though a color texture hasn't been selected!\n";
        return;
    }
    scene->getNormalTextures()[o]->setName(n.toStdString());
    scene->setChanged(Scene::NORMAL_TEXTURE_CHANGED);
    notifyAll();
}

void Controller::windowChangeNormalTextureType(int t) {
    int it = windowModel->getSelectedNormalTextureIndex();
    if (it == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    NormalTexture *oldTexture = scene->getNormalTextures()[it];
    if (oldTexture->getType() == static_cast<NormalTexture::Type>(t)) {
        return;
    }
    ensureThreadStopped();
    string name = oldTexture->getName();
    NormalTexture *texture = nullptr;
    auto type = static_cast<NormalTexture::Type>(t);
    switch (type) {
        case NormalTexture::Type::Mesh:
            texture = new MeshNormalTexture(name);
            break;
        case NormalTexture::Type::Noise:
            texture = new NoiseNormalTexture(
                    NoiseUser::Predefined::PERLIN_MARBLE,
                    Vec3Df(0.6, 0.6, 0.7),
                    name);
            break;
        case NormalTexture::Type::Image:
            texture = new ImageNormalTexture(
                    "normals/swarm.jpg",
                    name);
            break;
    }
    scene->getNormalTextures()[it] = texture;
    scene->updateMaterialsNormalTexture(oldTexture, texture);
    delete oldTexture;
    scene->setChanged(Scene::NORMAL_TEXTURE_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowChangeNormalImageTextureFile() {
    int it = windowModel->getSelectedNormalTextureIndex();
    if (it == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    ImageNormalTexture *texture = dynamic_cast<ImageNormalTexture*>(scene->getNormalTextures()[it]);
    if (!texture) {
        cerr << __FUNCTION__ << " called even though selected texture is not an ImageNormalTexture!\n";
        return;
    }
    QString filename = QFileDialog::getOpenFileName(window,
                                                    "Open a texture image",
                                                    "./normals",
                                                    "*.jpg *.bmp *.png");
    if (!filename.isNull()) {
        ensureThreadStopped();
        texture->loadImage(filename.toStdString().c_str());
        scene->setChanged(Scene::NORMAL_TEXTURE_CHANGED);
        renderThread->hasToRedraw();
        notifyAll();
    }
}

void Controller::windowSetNoiseNormalTextureFunction(int ip) {
    int it = windowModel->getSelectedNormalTextureIndex();
    if (it == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    NoiseNormalTexture *texture = dynamic_cast<NoiseNormalTexture*>(scene->getNormalTextures()[it]);
    if (!texture) {
        cerr << __FUNCTION__ << " called even though selected texture is not a NoiseNormalTexture!\n";
        return;
    }
    ensureThreadStopped();
    texture->loadPredefined(static_cast<NoiseUser::Predefined>(ip));
    scene->setChanged(Scene::NORMAL_TEXTURE_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetNoiseNormalTextureOffset() {
    int it = windowModel->getSelectedNormalTextureIndex();
    if (it == -1) {
        cerr << __FUNCTION__ << " called even though a texture hasn't been selected!\n";
        return;
    }
    NoiseNormalTexture *texture = dynamic_cast<NoiseNormalTexture*>(scene->getNormalTextures()[it]);
    if (!texture) {
        cerr << __FUNCTION__ << " called even though selected texture is not a NoiseNormalTexture!\n";
        return;
    }
    ensureThreadStopped();
    Vec3Df offset = window->getNoiseTextureOffset();
    texture->setOffset(offset);
    scene->setChanged(Scene::NORMAL_TEXTURE_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSelectObject(int o) {
    windowModel->setSelectedObjectIndex(o-1);
    notifyAll();
}

void Controller::windowEnableObject(bool enabled) {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    if (!windowModel->isRealTime()) {
        windowModel->setDisplayMode(WindowModel::OpenGLDisplayMode);
    }
    scene->getObjects()[o]->setEnabled(enabled);
    scene->setChanged(Scene::OBJECT_CHANGED);
    scene->updateBoundingBox();
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetObjectName(const QString &n) {
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->setName(n.toStdString());
    scene->setChanged(Scene::OBJECT_CHANGED);
    notifyAll();
}

void Controller::windowSetObjectPos() {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->setTrans(window->getObjectPos());
    scene->setChanged(Scene::OBJECT_CHANGED);
    scene->updateBoundingBox();
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetObjectMobile() {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->setMobile(window->getObjectMobile());
    scene->setChanged(Scene::OBJECT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetObjectMaterial(int index) {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->setMaterial(scene->getMaterials()[index]);
    scene->setChanged(Scene::OBJECT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSelectLight(int l) {
    windowModel->setSelectedLightIndex(l-1);
    notifyAll();
}

void Controller::windowEnableLight(bool enabled) {
    ensureThreadStopped();
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    if (!windowModel->isRealTime()) {
        windowModel->setDisplayMode(WindowModel::OpenGLDisplayMode);
    }
    scene->getLights()[l]->setEnabled(enabled);
    scene->setChanged(Scene::LIGHT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetLightRadius(double r) {
    ensureThreadStopped();
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    if (!windowModel->isRealTime()) {
        windowModel->setDisplayMode(WindowModel::OpenGLDisplayMode);
    }
    scene->getLights()[l]->setRadius(r);
    scene->setChanged(Scene::LIGHT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetLightIntensity(double i) {
    ensureThreadStopped();
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    if (!windowModel->isRealTime()) {
        windowModel->setDisplayMode(WindowModel::OpenGLDisplayMode);
    }
    scene->getLights()[l]->setIntensity(i);
    scene->setChanged(Scene::LIGHT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetLightPos() {
    ensureThreadStopped();
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    scene->getLights()[l]->setPos(window->getLightPos());
    scene->setChanged(Scene::LIGHT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
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
        scene->setChanged(Scene::LIGHT_CHANGED);
        renderThread->hasToRedraw();
        notifyAll();
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
        windowModel->setDragEnabled(false);
    }

    notifyAll();
}

void Controller::windowSetDurtiestQuality(int quality) {
    rayTracer->setDurtiestQuality(static_cast<RayTracer::Quality>(quality));
    notifyAll();
}

void Controller::windowSetQualityDivider(int divider) {
    rayTracer->setDurtiestQualityDivider(divider);
    notifyAll();
}

void Controller::windowUpdatePBGI() {
    pbgi->update();
    notifyAll();
}

void Controller::viewerSetWireframe(bool b) {
    windowModel->setWireframe(b);
    notifyAll();
}

void Controller::viewerSetRenderingMode(WindowModel::RenderingMode m) {
    windowModel->setRenderingMode(m);
    notifyAll();
}

void Controller::viewerSetRenderingMode(int m) {
    windowModel->setRenderingMode(static_cast<WindowModel::RenderingMode>(m));
    notifyAll();
}

void Controller::viewerSetDisplayMode(WindowModel::DisplayMode m) {
    windowModel->setDisplayMode(m);
    notifyAll();
}

void Controller::viewerSetDisplayMode(int m) {
    windowModel->setDisplayMode(static_cast<WindowModel::DisplayMode>(m));
    notifyAll();
}

void Controller::viewerSetRayImage(const QImage & image) {
    windowModel->setRayImage(image);
    notifyAll();
}

void Controller::windowSetDragEnabled(bool e) {
    windowModel->setDragEnabled(e);
    notifyAll();
}

void Controller::windowSetUScale(double u) {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->getMesh().setUScale(u);
    scene->setChanged(Scene::OBJECT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetVScale(double v) {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->getMesh().setVScale(v);
    scene->setChanged(Scene::OBJECT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetSquareMapping() {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->getMesh().setSquareTextureMapping();
    scene->setChanged(Scene::OBJECT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetSphericalMapping() {
    ensureThreadStopped();
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    scene->getObjects()[o]->getMesh().setDefaultTextureMapping();
    scene->setChanged(Scene::OBJECT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::windowSetCubicMapping() {
    ensureThreadStopped();
    int io = windowModel->getSelectedObjectIndex();
    if (io == -1) {
        cerr << __FUNCTION__ << " called even though an object hasn't been selected!\n";
        return;
    }
    Object *o = scene->getObjects()[io];
    o->getMesh().setCubeTextureMapping(&o->getMaterial(), 3, 3);
    scene->setChanged(Scene::OBJECT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::viewerStartsDragging(Object *o, Vec3Df i, QPoint p, float r) {
    windowModel->setDraggedObject(o, i, p, r);
    windowModel->setSelectedObject(o);
    notifyAll();
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
    scene->setChanged(Scene::OBJECT_CHANGED);
    renderThread->hasToRedraw();
    notifyAll();
}

void Controller::viewerStopsDragging() {
    windowModel->stopDragging();
    notifyAll();
}

void Controller::setRayTracerQuality(RayTracer::Quality quality) {
    rayTracer->setQuality(quality);
}

void Controller::setSceneMove(int nbPictures) {
    scene->move(nbPictures);
}

void Controller::setSceneReset() {
    scene->reset();
}
