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
    PTIntensitySpinBox->setVisible(isPT);
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
    }
    lightRadiusSpinBox->setVisible(isLightEnabled);
    lightIntensitySpinBox->setVisible(isLightEnabled);

    if (isLightSelected) {
        Light l = *(scene->getLights())[lightIndex];
        isLightEnabled = l.isEnabled();
        lightEnableCheckBox->setChecked(isLightEnabled);
        Vec3Df pos = l.getPos();
        float intensity = l.getIntensity();
        float radius = l.getRadius();
        for (int i=0; i<3; i++) {
            lightPosSpinBoxes[i]->setValue(pos[i]);
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



    //  Anti Aliasing
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

    //  Ambient occlusion
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

    rayLayout->addWidget(AOGroupBox);

    //  Shadows
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

    //  Path Tracing
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

    PTIntensitySpinBox = new QSpinBox(PTGroupBox);
    PTIntensitySpinBox->setPrefix ("Intensity: ");
    PTIntensitySpinBox->setMinimum (1);
    PTIntensitySpinBox->setMaximum (1000);
    PTIntensitySpinBox->setVisible(false);
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

    //  Focal
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

    // Motion Blur
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

    //  Objects
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


    //  Lights
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
    QString axis[3] = {"X: ", "Y: ", "Z: "};
    for (int i=0; i<3; i++) {
        lightPosSpinBoxes[i] = new QDoubleSpinBox(lightsGroupBox);
        lightPosSpinBoxes[i]->setSingleStep(0.1);
        lightPosSpinBoxes[i]->setMinimum(-100);
        lightPosSpinBoxes[i]->setMaximum(100);
        lightPosSpinBoxes[i]->setVisible(false);
        lightPosSpinBoxes[i]->setPrefix(axis[i]);
        lightsPosLayout->addWidget(lightPosSpinBoxes[i]);
        connect(lightPosSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightPos()));
    }
    lightsLayout->addLayout(lightsPosLayout);

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

    // Render
    QGroupBox * ActionGroupBox = new QGroupBox ("Action", sceneGroupBox);
    QVBoxLayout * actionLayout = new QVBoxLayout (ActionGroupBox);

    QPushButton * rayButton = new QPushButton ("Render", sceneGroupBox);
    actionLayout->addWidget (rayButton);
    connect (rayButton, SIGNAL (clicked ()), controller, SLOT (windowRenderRayImage ()));
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

    //layout->addStretch (0);
}
