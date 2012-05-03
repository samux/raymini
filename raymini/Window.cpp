#include <GL/glew.h>
#include "Window.h"

#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <QDockWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMenuBar>
#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QColorDialog>
#include <QLCDNumber>
#include <QPixmap>
#include <QFrame>
#include <QSplitter>
#include <QMenu>
#include <QScrollArea>
#include <QCoreApplication>
#include <QFont>
#include <QSizePolicy>
#include <QImageReader>
#include <QStatusBar>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QComboBox>

#include "RayTracer.h"
#include "Scene.h"
#include "AntiAliasing.h"
#include "Controller.h"

const char * ICON = "textures/icon.png";

using namespace std;

Window::Window (Controller *c) : QMainWindow (NULL), controller(c) {
    QDockWidget * controlDockWidget = new QDockWidget (this);
    initControlWidget ();

    controlDockWidget->setFloating(true);
    controlDockWidget->setWidget (controlWidget);
    controlDockWidget->adjustSize ();
    addDockWidget (Qt::RightDockWidgetArea, controlDockWidget);
    controlDockWidget->setFeatures (QDockWidget::AllDockWidgetFeatures);
    statusBar()->showMessage("");
    setWindowIcon(QIcon(ICON));
    controlDockWidget->setWindowIcon(QIcon(ICON));
}

Window::~Window () {

}

Vec3Df Window::getLightPos() const {
    Vec3Df newPos;
    for (int i=0; i<3; i++) {
        newPos[i] = lightPosSpinBoxes[i]->value();
    }
    return newPos;
}

Vec3Df Window::getLightColor() const {
    Vec3Df newColor;
    for (int i=0; i<3; i++) {
        newColor[i] = lightColorSpinBoxes[i]->value();
    }
    return newColor;
}

void Window::update(Observable *observable) {
    if (observable == controller->getScene()) {
        updateFromScene();
    }
    else if (observable == controller->getRayTracer()) {
        updateFromRayTracer();
    }
    else if (observable == controller->getWindowModel()) {
        updateFromWindowModel();
    }
    else if (observable == controller->getRenderThread()) {
        updateProgressBar();
        updateStatus();
    }
    else {
        cerr << "Window::update(Observable*) has been called from an unknown source!" << endl;
    }
}

void Window::updateFromScene() {
    // Lights
    updateLights();

    // Objects
    updateObjects();
}

void Window::updateFromRayTracer() {
    RayTracer *rayTracer = controller->getRayTracer();

    // Shadows
    shadowSpinBox->setVisible(rayTracer->getShadowMode() == Shadow::SOFT);

    // Anti aliasing
    AANbRaySpinBox->setVisible(rayTracer->typeAntiAliasing != AntiAliasing::NONE);

    // Ambient occlusion
    bool isAO = rayTracer->nbRayAmbientOcclusion != 0;
    AORadiusSpinBox->setVisible(isAO);
    AOMaxAngleSpinBox->setVisible(isAO);

    // Focus
    updateFocus();

    // Path tracing
    bool isPT = rayTracer->depthPathTracing != 0;
    PTNbRaySpinBox->setVisible(isPT);
    PTMaxAngleSpinBox->setVisible(isPT);
    PTOnlyCheckBox->setVisible(isPT);
    PBGICheckBox->setVisible(!isPT);
}

void Window::updateFromWindowModel() {
    // Lights
    updateLights();

    // Focus
    updateFocus();

    // Objects
    updateObjects();

    // Real time
    updateRealTime();

    // Status
    updateStatus();
}

void Window::updateLights() {
    Scene *scene = controller->getScene();
    WindowModel *windowModel = controller->getWindowModel();

    int lightIndex = windowModel->getSelectedLightIndex();
    lightsList->setCurrentIndex(lightIndex+1);
    bool isLightSelected = lightIndex != -1;
    bool isLightEnabled = isLightSelected && scene->getLights()[lightIndex]->isEnabled();
    lightEnableCheckBox->setVisible(isLightSelected);
    for (int i=0; i<3; i++) {
        lightPosSpinBoxes[i]->setVisible(isLightEnabled);
        lightColorSpinBoxes[i]->setVisible(isLightEnabled);
    }
    lightRadiusSpinBox->setVisible(isLightEnabled);
    lightIntensitySpinBox->setVisible(isLightEnabled);

    if (isLightSelected) {
        Light * l = scene->getLights()[lightIndex];
        isLightEnabled = l->isEnabled();
        lightEnableCheckBox->setChecked(isLightEnabled);
        Vec3Df color = l->getColor();
        Vec3Df pos = l->getPos();
        float intensity = l->getIntensity();
        float radius = l->getRadius();
        for (int i=0; i<3; i++) {
            lightPosSpinBoxes[i]->setValue(pos[i]);
            lightColorSpinBoxes[i]->setValue(color[i]);
        }
        lightIntensitySpinBox->setValue(intensity);
        lightRadiusSpinBox->setValue(radius);
    }
}

void Window::updateFocus() {
    WindowModel *windowModel = controller->getWindowModel();
    RayTracer *rayTracer = controller->getRayTracer();
    Focus::Type type = rayTracer->typeFocus;
    focusTypeComboBox->setCurrentIndex(type);
    bool isFocus = type != Focus::NONE;
    changeFocusFixingCheckBox->setVisible(isFocus);
    focusNbRaysSpinBox->setVisible(isFocus);
    focusApertureSpinBox->setVisible(isFocus);
    bool isFocusMode = windowModel->isFocusMode();
    changeFocusFixingCheckBox->setChecked(!isFocusMode);
    focusNbRaysSpinBox->setValue(rayTracer->nbRayFocus);
    focusApertureSpinBox->setValue(rayTracer->apertureFocus);
}

void Window::updateObjects() {
    Scene *scene = controller->getScene();
    WindowModel *windowModel = controller->getWindowModel();

    int index = windowModel->getSelectedObjectIndex();
    objectsList->setCurrentIndex(index+1);
    bool isSelected = index != -1;
    objectEnableCheckBox->setVisible(isSelected);
    if (isSelected) {
        bool isEnabled = scene->getObjects()[index]->isEnabled();
        objectEnableCheckBox->setChecked(isEnabled);
    }
}

void Window::updateProgressBar() {
    RenderThread *renderThread = controller->getRenderThread();
    WindowModel *windowModel = controller->getWindowModel();
    bool isRendering = renderThread->isRendering();
    bool isRealTime = windowModel->isRealTime();
    stopRenderButton->setVisible(isRendering||isRealTime);
    renderButton->setVisible(!isRendering && !isRealTime);
    renderProgressBar->setVisible(isRendering || isRealTime);
    if (isRendering) {
        float percent = renderThread->getPercent();
        renderProgressBar->setValue(percent);
    }
    else {
        renderProgressBar->setValue(100);
    }
}

void Window::updateRealTime() {
    WindowModel *windowModel = controller->getWindowModel();
    bool isRealTime = windowModel->isRealTime();
    realTimeCheckBox->setChecked(isRealTime);
}

void Window::updateStatus() {
    WindowModel *windowModel = controller->getWindowModel();
    RayTracer *rayTracer = controller->getRayTracer();
    RenderThread *renderThread = controller->getRenderThread();
    int elapsed = windowModel->getElapsedTime();
    qglviewer::Camera * cam = controller->getViewer()->camera ();
    unsigned int screenWidth = cam->screenWidth ();
    unsigned int screenHeight = cam->screenHeight ();
    RayTracer::Quality quality = rayTracer->quality;
    if (elapsed != 0) {
        int FPS = 1000/elapsed;
        QString message = 
                QString("[")+
                QString::number(screenWidth) + QString ("x") + QString::number (screenHeight) +
                QString("] ")+
                QString::number(elapsed) +
                QString ("ms (") +
                QString::number(FPS)+
                QString(" fps)");
        if (renderThread->isRendering()) {
            message +=
                QString(" Rendering in ")+
                QString(RayTracer::qualityToString(quality))+
                QString(" quality...");
        }
        statusBar()->showMessage(message);
    }
}

void Window::initControlWidget () {
    Scene *scene = controller->getScene();
    RayTracer *rayTracer = controller->getRayTracer();

    // Control widget
    controlWidget = new QGroupBox ();
    QGridLayout * layout = new QGridLayout (controlWidget);

    // Preview
    QGroupBox * previewGroupBox = new QGroupBox ("Preview", controlWidget);
    QVBoxLayout * previewLayout = new QVBoxLayout (previewGroupBox);

    QCheckBox * wireframeCheckBox = new QCheckBox ("Wireframe", previewGroupBox);
    connect(wireframeCheckBox, SIGNAL(clicked(bool)), controller, SLOT(viewerSetWireframe(bool)));
    previewLayout->addWidget (wireframeCheckBox);

    QComboBox *modeList = new QComboBox(previewGroupBox);
    modeList->addItem("Smooth");
    modeList->addItem("Flat");
    previewLayout->addWidget(modeList);
    connect (modeList, SIGNAL(activated (int)), controller, SLOT(viewerSetRenderingMode(int)));

    QPushButton * snapshotButton  = new QPushButton ("Save preview", previewGroupBox);
    connect (snapshotButton, SIGNAL(clicked ()) ,controller, SLOT(windowExportGLImage()));
    previewLayout->addWidget (snapshotButton);

    layout->addWidget (previewGroupBox, 1, 0);




    // Ray tracing
    QGroupBox * rayGroupBox = new QGroupBox ("Ray Tracing", controlWidget);
    QHBoxLayout * rayLayout = new QHBoxLayout (rayGroupBox);

    //  RayGroup: Anti Aliasing
    QGroupBox * AAGroupBox = new QGroupBox ("Anti aliasing", rayGroupBox);
    QVBoxLayout * AALayout = new QVBoxLayout (AAGroupBox);

    QComboBox *antiAliasingList = new QComboBox(AAGroupBox);
    antiAliasingList->addItem("None");
    antiAliasingList->addItem("Uniform");
    antiAliasingList->addItem("Regular polygon");
    antiAliasingList->addItem("Stochastic");

    AALayout->addWidget(antiAliasingList);
    connect(antiAliasingList, SIGNAL(activated(int)), controller, SLOT(windowChangeAntiAliasingType(int)));

    AANbRaySpinBox = new QSpinBox(AAGroupBox);
    AANbRaySpinBox->setSuffix(" rays");
    AANbRaySpinBox->setMinimum(4);
    AANbRaySpinBox->setMaximum(10);
    AANbRaySpinBox->setVisible(false);
    AANbRaySpinBox->setValue(rayTracer->nbRayAntiAliasing);
    connect(AANbRaySpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetNbRayAntiAliasing(int)));
    AALayout->addWidget(AANbRaySpinBox);

    rayLayout->addWidget(AAGroupBox);

    //  RayGroup: Ambient occlusion
    QGroupBox * AOGroupBox = new QGroupBox ("Ambient Occlusion", rayGroupBox);
    QVBoxLayout * AOLayout = new QVBoxLayout (AOGroupBox);

    QSpinBox *AONbRaysSpinBox = new QSpinBox(AOGroupBox);
    AONbRaysSpinBox->setSuffix(" rays");
    AONbRaysSpinBox->setMinimum(0);
    AONbRaysSpinBox->setMaximum(1000);
    AONbRaysSpinBox->setValue(rayTracer->nbRayAmbientOcclusion);
    AOLayout->addWidget(AONbRaysSpinBox);
    connect(AONbRaysSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowChangeAmbientOcclusionNbRays(int)));

    AOMaxAngleSpinBox = new QSpinBox(AOGroupBox);
    AOMaxAngleSpinBox->setPrefix ("Max angle: ");
    AOMaxAngleSpinBox->setSuffix (" degrees");
    AOMaxAngleSpinBox->setMinimum (0);
    AOMaxAngleSpinBox->setMaximum (180);
    AOMaxAngleSpinBox->setVisible(false);
    AOMaxAngleSpinBox->setValue(rayTracer->maxAngleAmbientOcclusion*360.0/(2.0*M_PI));
    connect(AOMaxAngleSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetAmbientOcclusionMaxAngle(int)));
    AOLayout->addWidget(AOMaxAngleSpinBox);

    AORadiusSpinBox = new QDoubleSpinBox(AOGroupBox);
    AORadiusSpinBox->setPrefix("Radius: ");
    AORadiusSpinBox->setMinimum(0);
    AORadiusSpinBox->setSingleStep(0.1);
    AORadiusSpinBox->setValue(rayTracer->radiusAmbientOcclusion);
    AORadiusSpinBox->setVisible(false);
    connect(AORadiusSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetAmbientOcclusionRadius(double)));
    AOLayout->addWidget(AORadiusSpinBox);

    QCheckBox * AOOnlyCheckBox = new QCheckBox ("Only ambient coloring", AOGroupBox);
    connect (AOOnlyCheckBox, SIGNAL (toggled (bool)), controller, SLOT(windowSetOnlyAO (bool)));
    AOLayout->addWidget (AOOnlyCheckBox);

    rayLayout->addWidget(AOGroupBox);

    //  RayGroup: Shadows
    QGroupBox * shadowsGroupBox = new QGroupBox ("Shadows", rayGroupBox);
    QVBoxLayout * shadowsLayout = new QVBoxLayout (shadowsGroupBox);

    QComboBox *shadowTypeList = new QComboBox(shadowsGroupBox);
    shadowTypeList->addItem("None");
    shadowTypeList->addItem("Hard shadow");
    shadowTypeList->addItem("Soft shadow");
    connect (shadowTypeList, SIGNAL (activated (int)), controller, SLOT (windowSetShadowMode (int)));
    shadowsLayout->addWidget (shadowTypeList);

    shadowSpinBox = new QSpinBox(shadowsGroupBox);
    shadowSpinBox->setSuffix (" rays");
    shadowSpinBox->setMinimum (2);
    shadowSpinBox->setMaximum (1000);
    shadowSpinBox->setVisible (false);
    shadowSpinBox->setValue(rayTracer->getShadowMode());
    connect (shadowSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetShadowNbRays (int)));
    shadowsLayout->addWidget (shadowSpinBox);

    rayLayout->addWidget (shadowsGroupBox);

    //  RayGroup: Path Tracing
    QGroupBox * PTGroupBox = new QGroupBox ("Path tracing", rayGroupBox);
    QVBoxLayout * PTLayout = new QVBoxLayout (PTGroupBox);

    QSpinBox *PTDepthSpinBox = new QSpinBox(PTGroupBox);
    PTDepthSpinBox->setPrefix ("Depth: ");
    PTDepthSpinBox->setMinimum (0);
    PTDepthSpinBox->setMaximum (5);
    PTDepthSpinBox->setValue(rayTracer->depthPathTracing);
    connect (PTDepthSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetDepthPathTracing (int)));
    PTLayout->addWidget (PTDepthSpinBox);

    PTNbRaySpinBox = new QSpinBox(PTGroupBox);
    PTNbRaySpinBox->setSuffix (" rays");
    PTNbRaySpinBox->setMinimum (1);
    PTNbRaySpinBox->setMaximum (1000);
    PTNbRaySpinBox->setVisible(false);
    PTNbRaySpinBox->setValue(rayTracer->nbRayPathTracing);
    connect (PTNbRaySpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetNbRayPathTracing (int)));
    PTLayout->addWidget (PTNbRaySpinBox);

    PTMaxAngleSpinBox = new QSpinBox(PTGroupBox);
    PTMaxAngleSpinBox->setPrefix ("Max angle: ");
    PTMaxAngleSpinBox->setSuffix (" degrees");
    PTMaxAngleSpinBox->setMinimum (0);
    PTMaxAngleSpinBox->setMaximum (180);
    PTMaxAngleSpinBox->setVisible(false);
    PTMaxAngleSpinBox->setValue(rayTracer->maxAnglePathTracing*360.0/(2.0*M_PI));
    connect (PTMaxAngleSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetMaxAnglePathTracing (int)));
    PTLayout->addWidget (PTMaxAngleSpinBox);

    QSpinBox * PTIntensitySpinBox = new QSpinBox(PTGroupBox);
    PTIntensitySpinBox->setPrefix ("Intensity: ");
    PTIntensitySpinBox->setMinimum (1);
    PTIntensitySpinBox->setMaximum (1000);
    PTIntensitySpinBox->setValue(rayTracer->intensityPathTracing);
    connect (PTIntensitySpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetIntensityPathTracing (int)));
    PTLayout->addWidget (PTIntensitySpinBox);

    PTOnlyCheckBox = new QCheckBox ("Only path tracing coloring", PTGroupBox);
    connect (PTOnlyCheckBox, SIGNAL (clicked (bool)), controller, SLOT (windowSetOnlyPT (bool)));
    PTOnlyCheckBox->setVisible(false);
    PTLayout->addWidget (PTOnlyCheckBox);

    PBGICheckBox = new QCheckBox ("PBGI mode", PTGroupBox);
    connect (PBGICheckBox, SIGNAL (clicked (bool)), controller, SLOT (windowSetRayTracerMode (bool)));
    PBGICheckBox->setVisible(true);
    PTLayout->addWidget (PBGICheckBox);

    rayLayout->addWidget (PTGroupBox);

    //  RayGroup: Focal
    QGroupBox * focalGroupBox = new QGroupBox("Focal", rayGroupBox);
    QVBoxLayout * focalLayout = new QVBoxLayout(focalGroupBox);

    focusTypeComboBox = new QComboBox(focalGroupBox);
    focusTypeComboBox->addItem("No focus");
    focusTypeComboBox->addItem("Uniform");
    focusTypeComboBox->addItem("Stochastic");
    connect(focusTypeComboBox, SIGNAL(activated(int)), controller, SLOT(windowSetFocusType(int)));
    focalLayout->addWidget(focusTypeComboBox);

    changeFocusFixingCheckBox = new QCheckBox("Focal point is fixed", focalGroupBox);
    changeFocusFixingCheckBox->setVisible(false);
    connect(changeFocusFixingCheckBox, SIGNAL(clicked(bool)), controller, SLOT(windowSetFocalFixing(bool)));
    focalLayout->addWidget(changeFocusFixingCheckBox);

    focusNbRaysSpinBox = new QSpinBox(focalGroupBox);
    focusNbRaysSpinBox->setSuffix(" rays");
    focusNbRaysSpinBox->setMinimum(1);
    focusNbRaysSpinBox->setVisible(false);
    connect(focusNbRaysSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetFocusNbRays(int)));
    focalLayout->addWidget(focusNbRaysSpinBox);

    focusApertureSpinBox = new QDoubleSpinBox(focalGroupBox);
    focusApertureSpinBox->setPrefix("Aperture: ");
    focusApertureSpinBox->setMinimum(0.01);
    focusApertureSpinBox->setMaximum(0.3);
    focusApertureSpinBox->setSingleStep(0.01);
    focusApertureSpinBox->setVisible(false);
    connect(focusApertureSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetFocusAperture(double)));
    focalLayout->addWidget(focusApertureSpinBox);

    rayLayout->addWidget(focalGroupBox);

    // RayGroup: Motion Blur
    if(scene->hasMobile()) {
        QGroupBox * mBlurGroupBox = new QGroupBox ("Motion blur", rayGroupBox);
        QVBoxLayout * mBlurLayout = new QVBoxLayout (mBlurGroupBox);

        QSpinBox * mBlurNbImagesSpinBox = new QSpinBox(PTGroupBox);
        mBlurNbImagesSpinBox->setSuffix (" images");
        mBlurNbImagesSpinBox->setMinimum (1);
        mBlurNbImagesSpinBox->setMaximum (100);
        connect (mBlurNbImagesSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetNbImagesSpinBox (int)));
        mBlurLayout->addWidget (mBlurNbImagesSpinBox);

        rayLayout->addWidget (mBlurGroupBox);
    }


    layout->addWidget (rayGroupBox, 0, 1);



    // scene param
    QGroupBox * sceneGroupBox = new QGroupBox ("Scene", controlWidget);
    QHBoxLayout * sceneLayout = new QHBoxLayout (sceneGroupBox);

    //  SceneGroup: Objects
    QGroupBox *objectsGroupBox = new QGroupBox("Objects", sceneGroupBox);
    QVBoxLayout *objectsLayout = new QVBoxLayout(objectsGroupBox);

    objectsList = new QComboBox(objectsGroupBox);
    objectsList->addItem("No object selected");
    for (const Object * o : scene->getObjects()) {
        QString name = QString::fromStdString(o->getName());
        objectsList->addItem(name);
    }
    connect(objectsList, SIGNAL(activated(int)), controller, SLOT(windowSelectObject(int)));
    objectsLayout->addWidget(objectsList);

    objectEnableCheckBox = new QCheckBox("Enable");
    objectEnableCheckBox->setVisible(false);
    connect(objectEnableCheckBox, SIGNAL(clicked(bool)), controller, SLOT(windowEnableObject(bool)));
    objectsLayout->addWidget(objectEnableCheckBox);

    sceneLayout->addWidget(objectsGroupBox);


    //  SceneGroup: Lights
    QGroupBox *lightsGroupBox = new QGroupBox("Lights", sceneGroupBox);
    QVBoxLayout *lightsLayout = new QVBoxLayout(lightsGroupBox);

    lightsList = new QComboBox(lightsGroupBox);
    lightsList->addItem("No light selected");
    for (unsigned int i=0; i<scene->getLights().size(); i++) {
        QString name;
        name = QString("Light #%1").arg(i);
        lightsList->addItem(name);
    }
    connect(lightsList, SIGNAL(activated(int)), controller, SLOT(windowSelectLight(int)));
    lightsLayout->addWidget(lightsList);

    lightEnableCheckBox = new QCheckBox("Enable");
    lightEnableCheckBox->setVisible(false);
    connect(lightEnableCheckBox, SIGNAL(clicked(bool)), controller, SLOT(windowEnableLight(bool)));
    lightsLayout->addWidget(lightEnableCheckBox);

    QHBoxLayout *lightsPosLayout = new QHBoxLayout;
    QHBoxLayout *lightsColorLayout = new QHBoxLayout;
    QString axis[3] = {"X: ", "Y: ", "Z: "};
    QString colors[3] = {"R: ", "G: ", "B: "};
    for (int i=0; i<3; i++) {
        lightPosSpinBoxes[i] = new QDoubleSpinBox(lightsGroupBox);
        lightPosSpinBoxes[i]->setSingleStep(0.1);
        lightPosSpinBoxes[i]->setMinimum(-100);
        lightPosSpinBoxes[i]->setMaximum(100);
        lightPosSpinBoxes[i]->setVisible(false);
        lightPosSpinBoxes[i]->setPrefix(axis[i]);
        lightsPosLayout->addWidget(lightPosSpinBoxes[i]);
        connect(lightPosSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightPos()));
        lightColorSpinBoxes[i] = new QDoubleSpinBox(lightsGroupBox);
        lightColorSpinBoxes[i]->setSingleStep(0.01);
        lightColorSpinBoxes[i]->setMinimum(0);
        lightColorSpinBoxes[i]->setMaximum(1);
        lightColorSpinBoxes[i]->setVisible(false);
        lightColorSpinBoxes[i]->setPrefix(colors[i]);
        lightsColorLayout->addWidget(lightColorSpinBoxes[i]);
        connect(lightColorSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightColor()));
    }
    lightsLayout->addLayout(lightsPosLayout);
    lightsLayout->addLayout(lightsColorLayout);

    lightRadiusSpinBox = new QDoubleSpinBox(lightsGroupBox);
    lightRadiusSpinBox->setSingleStep(0.01);
    lightRadiusSpinBox->setMinimum(0);
    lightRadiusSpinBox->setMaximum(100);
    lightRadiusSpinBox->setVisible(false);
    lightRadiusSpinBox->setPrefix ("Radius: ");
    connect(lightRadiusSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightRadius(double)));
    lightsLayout->addWidget(lightRadiusSpinBox);

    lightIntensitySpinBox = new QDoubleSpinBox(lightsGroupBox);
    lightIntensitySpinBox->setSingleStep(0.1);
    lightIntensitySpinBox->setMinimum(0);
    lightIntensitySpinBox->setMaximum(100);
    lightIntensitySpinBox->setVisible(false);
    lightIntensitySpinBox->setPrefix("Intensity: ");
    connect(lightIntensitySpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightIntensity(double)));
    lightsLayout->addWidget(lightIntensitySpinBox);

    sceneLayout->addWidget(lightsGroupBox);

    // SceneGroup: Render
    QGroupBox * ActionGroupBox = new QGroupBox ("Action", sceneGroupBox);
    QVBoxLayout * actionLayout = new QVBoxLayout (ActionGroupBox);

    stopRenderButton = new QPushButton("Stop", sceneGroupBox);
    actionLayout->addWidget(stopRenderButton);
    stopRenderButton->setVisible(false);
    connect(stopRenderButton, SIGNAL(clicked()), controller, SLOT(windowStopRendering()));
    renderButton = new QPushButton ("Render", sceneGroupBox);
    actionLayout->addWidget (renderButton);
    connect (renderButton, SIGNAL (clicked ()), controller, SLOT (windowRenderRayImage ()));
    renderProgressBar = new QProgressBar(sceneGroupBox);
    renderProgressBar->setMinimum(0);
    renderProgressBar->setMaximum(100);
    actionLayout->addWidget(renderProgressBar);
    realTimeCheckBox = new QCheckBox("Real time", sceneGroupBox);
    connect(realTimeCheckBox, SIGNAL(clicked(bool)), controller, SLOT(windowSetRealTime(bool)));
    actionLayout->addWidget(realTimeCheckBox);
    QPushButton * showButton = new QPushButton ("Show", sceneGroupBox);
    actionLayout->addWidget (showButton);
    connect (showButton, SIGNAL (clicked ()), controller, SLOT (windowShowRayImage ()));
    QPushButton * saveButton  = new QPushButton ("Save", sceneGroupBox);
    connect (saveButton, SIGNAL (clicked ()) , controller, SLOT (windowExportRayImage ()));
    actionLayout->addWidget (saveButton);

    sceneLayout->addWidget (ActionGroupBox);

    layout->addWidget (sceneGroupBox, 1, 1);



    // Global settings
    QGroupBox * globalGroupBox = new QGroupBox ("Global Settings", controlWidget);
    QVBoxLayout * globalLayout = new QVBoxLayout (globalGroupBox);

    QPushButton * bgColorButton  = new QPushButton ("Background Color", globalGroupBox);
    connect (bgColorButton, SIGNAL (clicked()) , controller, SLOT (windowSetBGColor()));
    globalLayout->addWidget (bgColorButton);

    QPushButton * aboutButton  = new QPushButton ("About", globalGroupBox);
    connect (aboutButton, SIGNAL (clicked()) , controller, SLOT (windowAbout()));
    globalLayout->addWidget (aboutButton);

    QPushButton * quitButton  = new QPushButton ("Quit", globalGroupBox);
    connect (quitButton, SIGNAL (clicked()) , qApp, SLOT (closeAllWindows()));
    globalLayout->addWidget (quitButton);

    layout->addWidget (globalGroupBox, 0, 0);
}
