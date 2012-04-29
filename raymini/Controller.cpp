#include "Controller.h"

#include <QPlastiqueStyle>
#include <QStatusBar>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "QTUtils.h"

using namespace std;

// TODO update corresponding model

Controller::Controller(int argc, char *argv[]):
    QApplication(argc, argv)
{}

Controller::~Controller() {
}

void Controller::initAll() {
    setBoubekQTStyle(*this);

    setStyle(new QPlastiqueStyle);

    window = new Window(this);
    window->setWindowTitle("RayMini: A minimal raytracer.");
    window->show();
    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quit()));

    viewer = new GLViewer(this);
    window->setCentralWidget(viewer);

    scene = new Scene(this);
    scene->addObserver(window);
    scene->addObserver(viewer);

    rayTracer = new RayTracer(this);
    rayTracer->addObserver(window);
    rayTracer->addObserver(viewer);

    windowModel = new WindowModel(this);
    windowModel->addObserver(window);
    windowModel->addObserver(viewer);
}

void Controller::windowSetShadowMode(int i) {
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
}

void Controller::windowSetShadowNbRays (int i) {
    rayTracer->setShadowNbImpule(i);
}

void Controller::windowRenderRayImage () {
    qglviewer::Camera * cam = viewer->camera ();
    qglviewer::Vec p = cam->position ();
    qglviewer::Vec d = cam->viewDirection ();
    qglviewer::Vec u = cam->upVector ();
    qglviewer::Vec r = cam->rightVector ();
    Vec3Df camPos (p[0], p[1], p[2]);
    Vec3Df viewDirection (d[0], d[1], d[2]);
    Vec3Df upVector (u[0], u[1], u[2]);
    Vec3Df rightVector (r[0], r[1], r[2]);
    float fieldOfView = cam->fieldOfView ();
    float aspectRatio = cam->aspectRatio ();
    unsigned int screenWidth = cam->screenWidth ();
    unsigned int screenHeight = cam->screenHeight ();
    QTime timer;
    timer.start ();
    viewerSetRayImage(rayTracer->render (camPos, viewDirection, upVector, rightVector,
                                           fieldOfView, aspectRatio, screenWidth, screenHeight));
    window->statusBar()->showMessage(QString ("Raytracing performed in ") +
                             QString::number (timer.elapsed ()) +
                             QString ("ms at ") +
                             QString::number (screenWidth) + QString ("x") + QString::number (screenHeight) +
                             QString (" screen resolution"));
    viewerSetDisplayMode(WindowModel::RayDisplayMode);
}

void Controller::windowSetBGColor () {
    Vec3Df bg = 255*rayTracer->getBackgroundColor();
    QColor c = QColorDialog::getColor (QColor (bg[0], bg[1], bg[2]), window);
    if (c.isValid () == true) {
        rayTracer->setBackgroundColor (Vec3Df (c.red ()/255.f, c.green ()/255.f, c.blue ()/255.f));
        viewer->setBackgroundColor (c);
    }
}

void Controller::windowShowRayImage () {
    viewerSetDisplayMode(WindowModel::RayDisplayMode);
}

void Controller::windowExportGLImage () {
    viewer->saveSnapshot (false, false);
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
}

void Controller::windowAbout () {
    QMessageBox::about (window,
                        "About This Program",
                        "<b>RayMini</b> by: <br> <i>Tamy Boubekeur <br> Axel Schumacher <br> Bertrand Chazot <br> Samuel Mokrani</i>.");
}

void Controller::windowChangeAntiAliasingType(int index) {
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
}

void Controller::windowSetNbRayAntiAliasing(int i) {
    rayTracer->nbRayAntiAliasing = i;
}

void Controller::windowChangeAmbientOcclusionNbRays(int index) {
    rayTracer->nbRayAmbientOcclusion = index;
}

void Controller::windowSetAmbientOcclusionMaxAngle(int i) {
    rayTracer->maxAngleAmbientOcclusion = (float)i*2.0*M_PI/360.0;
}

void Controller::windowSetAmbientOcclusionRadius(double f) {
    rayTracer->radiusAmbientOcclusion = f;
}

void Controller::windowSetAmbientOcclusionIntensity(int i) {
    rayTracer->intensityAmbientOcclusion = float(i)/100;
}

void Controller::windowSetOnlyAO(bool b) {
    rayTracer->onlyAmbientOcclusion = b;
}

void Controller::windowEnableFocal(bool isFocal) {
    windowModel->setFocusMode(isFocal);
    if(!isFocal) {
        rayTracer->noFocus();
    }
}

void Controller::windowSetFocal() {
    if (windowModel->isFocusMode()) {
        windowModel->setFocusMode(false);
        rayTracer->setFocus(windowModel->getFocusPoint());
    }
    else {
        windowEnableFocal(true);
    }
}

void Controller::windowSetDepthPathTracing(int i) {
    rayTracer->depthPathTracing = i;
}

void Controller::windowSetNbRayPathTracing(int i) {
    rayTracer->nbRayPathTracing = i;
}
void Controller::windowSetMaxAnglePathTracing(int i) {
    rayTracer->maxAnglePathTracing = (float)i*2.0*M_PI/360.0;
}

void Controller::windowSetIntensityPathTracing(int i) {
    rayTracer->intensityPathTracing = float(i);
}

void Controller::windowSetNbImagesSpinBox(int i) {
    rayTracer->nbPictures = i;
}

void Controller::windowSetOnlyPT(bool b) {
    rayTracer->onlyPathTracing = b;
}

void Controller::windowSelectLight(int l) {
    if (l != 0) {
        bool enabled = scene->getLights()[l-1].isEnabled();
        windowEnableLight(enabled);
    }
}

void Controller::windowEnableLight(bool enabled) {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    scene->getLights()[l].setEnabled(enabled);
}

void Controller::windowSetLightRadius(double r) {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    scene->getLights()[l].setRadius(r);
}

void Controller::windowSetLightIntensity(double i) {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    scene->getLights()[l].setIntensity(i);
}

void Controller::windowSetLightPos() {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    scene->getLights()[l].setPos(window->getLightPos());
}

void Controller::viewerSetWireframe(bool b) {
    windowModel->setWireframe(b);
}

void Controller::viewerSetRenderingMode(WindowModel::RenderingMode m) {
    windowModel->setRenderingMode(m);
}

void Controller::viewerSetRenderingMode(int m) {
    windowModel->setRenderingMode(static_cast<WindowModel::RenderingMode>(m));
}

void Controller::viewerSetDisplayMode(WindowModel::DisplayMode m) {
    windowModel->setDisplayMode(m);
}

void Controller::viewerSetDisplayMode(int m) {
    windowModel->setDisplayMode(static_cast<WindowModel::DisplayMode>(m));
}

void Controller::viewerSetRayImage(const QImage & image) {
    windowModel->setRayImage(image);
}

void Controller::viewerSetFocusPoint(Vertex point) {
    windowModel->setFocusPoint(point);
}
