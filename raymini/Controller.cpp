#include "Controller.h"

#include <QPlastiqueStyle>

#include "QTUtils.h"

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
    viewer->setRayImage(rayTracer->render (camPos, viewDirection, upVector, rightVector,
                                           fieldOfView, aspectRatio, screenWidth, screenHeight));
    statusBar()->showMessage(QString ("Raytracing performed in ") +
                             QString::number (timer.elapsed ()) +
                             QString ("ms at ") +
                             QString::number (screenWidth) + QString ("x") + QString::number (screenHeight) +
                             QString (" screen resolution"));
    viewerSetDisplayMode(GLViewer::RayDisplayMode);
}

void Controller::windowSetBGColor () {
    Vec3Df bg = 255*rayTracer->getBackgroundColor();
    QColor c = QColorDialog::getColor (QColor (bg[0], bg[1], bg[2]), this);
    if (c.isValid () == true) {
        rayTracer->setBackgroundColor (Vec3Df (c.red ()/255.f, c.green ()/255.f, c.blue ()/255.f));
        viewer->setBackgroundColor (c);
        viewer->updateGL ();
    }
}

void Controller::windowShowRayImage () {
    viewerSetDisplayMode(GLViewer::RayDisplayMode);
}

void Controller::windowExportGLImage () {
    viewer->saveSnapshot (false, false);
}

void Controller::windowExportRayImage () {
    QString filename = QFileDialog::getSaveFileName (this,
                                                     "Save ray-traced image",
                                                     ".",
                                                     "*.jpg *.bmp *.png");
    if (!filename.isNull () && !filename.isEmpty ()) {
        // HACK: for some reason, saved image is fliped
        QImage fliped(viewer->getRayImage().mirrored(false, true));
        fliped.save (filename);
    }
}

void Controller::windowAbout () {
    QMessageBox::about (this,
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
    AANbRaySpinBox->setVisible(type != AntiAliasing::NONE);
}

void Controller::windowSetNbRayAntiAliasing(int i) {
    rayTracer->nbRayAntiAliasing = i;
}

void Controller::windowChangeAmbientOcclusionNbRays(int index) {
    rayTracer->nbRayAmbientOcclusion = index;
    AORadiusSpinBox->setVisible(index);
    AOMaxAngleSpinBox->setVisible(index);
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
    selecFocusedObject->setText("Choose focused point");
    selecFocusedObject->setVisible(isFocal);
    viewer->focusMode = isFocal;
    if(!isFocal)
        rayTracer->noFocus();
    viewer->updateGL();
}

void Controller::windowSetFocal() {
    if(viewer->focusMode) {
        viewer->focusMode = false;
        rayTracer->setFocus(viewer->getFocusPoint());
        selecFocusedObject->setText("Change focus point");
    }
    else {
        enableFocal(true);
    }
}

void Controller::windowSetDepthPathTracing(int i) {
    rayTracer->depthPathTracing = i;
    PTNbRaySpinBox->setVisible(i != 0);
    PTMaxAngleSpinBox->setVisible(i != 0);
    PTIntensitySpinBox->setVisible(i != 0);
    PTOnlyCheckBox->setVisible(i !=0);
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

int Controller::windowGetSelectedLightIndex() {
    return lightsList->currentIndex()-1;
}

void Controller::windowSelectLight(int l) {
    lightEnableCheckBox->setVisible(l != 0);
    if (l != 0) {
        bool enabled = scene->getLights()[l-1].isEnabled();
        lightEnableCheckBox->setChecked(enabled);
        enableLight(enabled);
    }
}

void Controller::windowEnableLight(bool enabled) {
    int l = getSelectedLightIndex();
    if (l == -1) {
        return;
    }
    for (int i=0; i<3; i++) {
        lightPosSpinBoxes[i]->setVisible(enabled);
    }
    lightRadiusSpinBox->setVisible(enabled);
    lightIntensitySpinBox->setVisible(enabled);
    Light &light = scene->getLights()[l];
    light.setEnabled(enabled);
    viewer->updateLights();
    if (!enabled) {
        return;
    }
    Vec3Df pos = light.getPos();
    float intensity = light.getIntensity();
    float radius = light.getRadius();
    for (int i=0; i<3; i++) {
        lightPosSpinBoxes[i]->setValue(pos[i]);
    }
    lightIntensitySpinBox->setValue(intensity);
    lightRadiusSpinBox->setValue(radius);
}

void Controller::windowSetLightRadius(double r) {
    int l = getSelectedLightIndex();
    if (l == -1) {
        return;
    }
    scene->getLights()[l].setRadius(r);
    viewer->updateLights();
}

void Controller::windowSetLightIntensity(double i) {
    int l = getSelectedLightIndex();
    if (l == -1) {
        return;
    }
    scene->getLights()[l].setIntensity(i);
    viewer->updateLights();
}

void Controller::windowSetLightPos() {
    int l = getSelectedLightIndex();
    if (l == -1) {
        return;
    }
    Vec3Df newPos;
    Light &light = scene->getLights()[l];
    for (int i=0; i<3; i++) {
        newPos[i] = lightPosSpinBoxes[i]->value();
    }
    light.setPos(newPos);
    viewer->updateLights();
}

void Controller::viewerSetWireframe(bool b) {
    wireframe = b;
    if (wireframe)
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    updateGL ();
}

void Controller::viewerSetRenderingMode(RenderingMode m) {
    renderingMode = m;
    updateGL ();
}

void Controller::viewerSetRenderingMode(int m) {
    setRenderingMode (static_cast<RenderingMode>(m));
}

void Controller::viewerSetDisplayMode(DisplayMode m) {
    displayMode = m;
    updateGL ();
}

void Controller::viewerSetDisplayMode(int m) {
    setRenderingMode (static_cast<DisplayMode>(m));
}

void Controller::viewerSetRayImage(const QImage & image) {
    rayImage = image;
}
