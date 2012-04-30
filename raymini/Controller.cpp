#include "Controller.h"

#include <QStatusBar>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>

using namespace std;

Controller::Controller(QApplication *r):
    raymini(r)
{}

Controller::~Controller()
{}

void Controller::initAll() {
    scene = new Scene(this);
    rayTracer = new RayTracer(this);
    windowModel = new WindowModel(this);

    window = new Window(this);
    window->setWindowTitle("RayMini: A minimal raytracer.");
    connect(raymini, SIGNAL(lastWindowClosed()), raymini, SLOT(quit()));

    viewer = new GLViewer(this);
    window->setCentralWidget(viewer);

    scene->addObserver(window);
    scene->addObserver(viewer);
    rayTracer->addObserver(window);
    rayTracer->addObserver(viewer);
    windowModel->addObserver(window);
    windowModel->addObserver(viewer);

    window->show();

    // First notification
    scene->notifyAll();
    rayTracer->notifyAll();
    windowModel->notifyAll();
}

/******************************************
 ***************** SLOTS ******************
 ******************************************/

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
    rayTracer->notifyAll();
}

void Controller::windowSetShadowNbRays (int i) {
    rayTracer->setShadowNbImpule(i);
    rayTracer->notifyAll();
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
    // TODO move to view
    QTime timer;
    timer.start ();

    // Will notify
    viewerSetRayImage(rayTracer->render (camPos, viewDirection, upVector, rightVector,
                                           fieldOfView, aspectRatio, screenWidth, screenHeight));

    window->statusBar()->showMessage(QString ("Raytracing performed in ") +
                             QString::number (timer.elapsed ()) +
                             QString ("ms at ") +
                             QString::number (screenWidth) + QString ("x") + QString::number (screenHeight) +
                             QString (" screen resolution"));

    // Will notify
    viewerSetDisplayMode(WindowModel::RayDisplayMode);

    windowModel->notifyAll();
    rayTracer->notifyAll();
}

void Controller::windowSetBGColor () {
    Vec3Df bg = 255*rayTracer->getBackgroundColor();
    QColor c = QColorDialog::getColor (QColor (bg[0], bg[1], bg[2]), window);
    if (c.isValid () == true) {
        rayTracer->setBackgroundColor(Vec3Df (c.red ()/255.f, c.green ()/255.f, c.blue ()/255.f));
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
    rayTracer->notifyAll();
}

void Controller::windowSetNbRayAntiAliasing(int i) {
    rayTracer->nbRayAntiAliasing = i;
    rayTracer->notifyAll();
}

void Controller::windowChangeAmbientOcclusionNbRays(int index) {
    rayTracer->nbRayAmbientOcclusion = index;
    rayTracer->notifyAll();
}

void Controller::windowSetAmbientOcclusionMaxAngle(int i) {
    rayTracer->maxAngleAmbientOcclusion = (float)i*2.0*M_PI/360.0;
    rayTracer->notifyAll();
}

void Controller::windowSetAmbientOcclusionRadius(double f) {
    rayTracer->radiusAmbientOcclusion = f;
    rayTracer->notifyAll();
}

void Controller::windowSetAmbientOcclusionIntensity(int i) {
    rayTracer->intensityAmbientOcclusion = float(i)/100;
    rayTracer->notifyAll();
}

void Controller::windowSetOnlyAO(bool b) {
    rayTracer->onlyAmbientOcclusion = b;
    rayTracer->notifyAll();
}

void Controller::windowEnableFocal(bool isFocal) {
    // TODO ENHANCE
    windowModel->setFocusMode(isFocal);
    windowModel->notifyAll();
    if(!isFocal) {
        rayTracer->noFocus();
        rayTracer->notifyAll();
    }
}

void Controller::windowSetFocal() {
    // TODO ENHANCE
    if (windowModel->isFocusMode()) {
        windowModel->setFocusMode(false);
        windowModel->notifyAll();
        rayTracer->setFocus(windowModel->getFocusPoint());
        rayTracer->notifyAll();
    }
    else {
        // Will notify
        windowEnableFocal(true);
    }
}

void Controller::windowSetDepthPathTracing(int i) {
    rayTracer->depthPathTracing = i;
    rayTracer->notifyAll();
}

void Controller::windowSetNbRayPathTracing(int i) {
    rayTracer->nbRayPathTracing = i;
    rayTracer->notifyAll();
}
void Controller::windowSetMaxAnglePathTracing(int i) {
    rayTracer->maxAnglePathTracing = (float)i*2.0*M_PI/360.0;
    rayTracer->notifyAll();
}

void Controller::windowSetIntensityPathTracing(int i) {
    rayTracer->intensityPathTracing = float(i);
    rayTracer->notifyAll();
}

void Controller::windowSetNbImagesSpinBox(int i) {
    rayTracer->nbPictures = i;
    rayTracer->notifyAll();
}

void Controller::windowSetOnlyPT(bool b) {
    rayTracer->onlyPathTracing = b;
    rayTracer->notifyAll();
}

void Controller::windowSelectObject(int o) {
    windowModel->setSelectedObjectIndex(o-1);
    windowModel->notifyAll();
}

void Controller::windowEnableObject(bool enabled) {
    int o = windowModel->getSelectedObjectIndex();
    if (o == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    scene->getObjects()[o].setEnabled(enabled);
    scene->notifyAll();
}

void Controller::windowSelectLight(int l) {
    windowModel->setSelectedLightIndex(l-1);
    windowModel->notifyAll();
}

void Controller::windowEnableLight(bool enabled) {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    scene->getLights()[l].setEnabled(enabled);
    scene->notifyAll();
}

void Controller::windowSetLightRadius(double r) {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    scene->getLights()[l].setRadius(r);
    scene->notifyAll();
}

void Controller::windowSetLightIntensity(double i) {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    scene->getLights()[l].setIntensity(i);
    scene->notifyAll();
}

void Controller::windowSetLightPos() {
    int l = windowModel->getSelectedLightIndex();
    if (l == -1) {
        cerr << __FUNCTION__ << " called even though a light hasn't been selected!\n";
        return;
    }
    viewerSetDisplayMode(WindowModel::OpenGLDisplayMode);
    scene->getLights()[l].setPos(window->getLightPos());
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

void Controller::viewerSetFocusPoint(Vertex point) {
    windowModel->setFocusPoint(point);
    windowModel->notifyAll();
}
