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
}

void Window::updateFromWindowModel() {
    // Lights
    updateLights();

    // Focus
    updateFocus();
}

void Window::updateLights() {
    Scene *scene = controller->getScene();
    WindowModel *windowModel = controller->getWindowModel();

    int lightIndex = windowModel->getSelectedLightIndex();
    bool isLightSelected = lightIndex != -1;
    bool isLightEnabled = isLightSelected && scene->getLights()[lightIndex].isEnabled();
    lightEnableCheckBox->setVisible(isLightSelected);
    for (int i=0; i<3; i++) {
        lightPosSpinBoxes[i]->setVisible(isLightEnabled);
    }
    lightRadiusSpinBox->setVisible(isLightEnabled);
    lightIntensitySpinBox->setVisible(isLightEnabled);

    if (isLightSelected) {
        Light l = scene->getLights()[lightIndex];
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
    bool isFocus = rayTracer->focusEnabled();
    selecFocusedObject->setVisible(isFocus);
    if (isFocus) {
        bool isFocusMode = windowModel->isFocusMode();
        if (isFocusMode) {
            selecFocusedObject->setText("Choose focused point");
        }
        else {
            selecFocusedObject->setText("Change focus point");
        }
    }
}

void Window::initControlWidget () {
    Scene *scene = controller->getScene();
    RayTracer *rayTracer = controller->getRayTracer();

    // Control widget
    controlWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (controlWidget);

    // Preview
    QGroupBox * previewGroupBox = new QGroupBox ("Preview", controlWidget);
    QVBoxLayout * previewLayout = new QVBoxLayout (previewGroupBox);

    QCheckBox * wireframeCheckBox = new QCheckBox ("Wireframe", previewGroupBox);
    connect (wireframeCheckBox, SIGNAL(toggled (bool)), controller, SLOT(viewerSetWireframe(bool)));
    previewLayout->addWidget (wireframeCheckBox);

    QComboBox *modeList = new QComboBox(previewGroupBox);
    modeList->addItem("Smooth");
    modeList->addItem("Flat");
    previewLayout->addWidget(modeList);
    connect (modeList, SIGNAL(activated (int)), controller, SLOT(viewerSetRenderingMode(int)));

    QPushButton * snapshotButton  = new QPushButton ("Save preview", previewGroupBox);
    connect (snapshotButton, SIGNAL (clicked ()) , this, SLOT (exportGLImage ()));
    previewLayout->addWidget (snapshotButton);

    layout->addWidget (previewGroupBox);

    // Ray tracing
    QGroupBox * rayGroupBox = new QGroupBox ("Ray Tracing", controlWidget);
    QVBoxLayout * rayLayout = new QVBoxLayout (rayGroupBox);

    //  Lights
    QGroupBox *lightsGroupBox = new QGroupBox("Lights", rayGroupBox);
    QVBoxLayout *lightsLayout = new QVBoxLayout(lightsGroupBox);

    lightsList = new QComboBox(lightsGroupBox);
    lightsList->addItem("No light selected");
    for (unsigned int i=0; i<scene->getLights().size(); i++) {
        QString name;
        name = QString("Light #%1").arg(i);
        lightsList->addItem(name);
    }
    connect(lightsList, SIGNAL(activated(int)), this, SLOT(selectLight(int)));
    lightsLayout->addWidget(lightsList);

    lightEnableCheckBox = new QCheckBox("Enable");
    lightEnableCheckBox->setVisible(false);
    connect(lightEnableCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableLight(bool)));
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
        connect(lightPosSpinBoxes[i], SIGNAL(valueChanged(double)), this, SLOT(setLightPos()));
    }
    lightsLayout->addLayout(lightsPosLayout);

    lightRadiusSpinBox = new QDoubleSpinBox(lightsGroupBox);
    lightRadiusSpinBox->setSingleStep(0.01);
    lightRadiusSpinBox->setMinimum(0);
    lightRadiusSpinBox->setMaximum(100);
    lightRadiusSpinBox->setVisible(false);
    lightRadiusSpinBox->setPrefix ("Radius: ");
    connect(lightRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setLightRadius(double)));
    lightsLayout->addWidget(lightRadiusSpinBox);

    lightIntensitySpinBox = new QDoubleSpinBox(lightsGroupBox);
    lightIntensitySpinBox->setSingleStep(0.1);
    lightIntensitySpinBox->setMinimum(0);
    lightIntensitySpinBox->setMaximum(100);
    lightIntensitySpinBox->setVisible(false);
    lightIntensitySpinBox->setPrefix("Intensity: ");
    connect(lightIntensitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(setLightIntensity(double)));
    lightsLayout->addWidget(lightIntensitySpinBox);

    rayLayout->addWidget(lightsGroupBox);

    //  Anti Aliasing
    QGroupBox * AAGroupBox = new QGroupBox ("Anti aliasing", rayGroupBox);
    QVBoxLayout * AALayout = new QVBoxLayout (AAGroupBox);

    QComboBox *antiAliasingList = new QComboBox(AAGroupBox);
    antiAliasingList->addItem("None");
    antiAliasingList->addItem("Uniform");
    antiAliasingList->addItem("Regular polygon");
    antiAliasingList->addItem("Stochastic");

    AALayout->addWidget(antiAliasingList);
    connect(antiAliasingList, SIGNAL(activated(int)), this, SLOT(changeAntiAliasingType(int)));

    AANbRaySpinBox = new QSpinBox(AAGroupBox);
    AANbRaySpinBox->setSuffix(" rays");
    AANbRaySpinBox->setMinimum(4);
    AANbRaySpinBox->setMaximum(10);
    AANbRaySpinBox->setVisible(false);
    AANbRaySpinBox->setValue(rayTracer->nbRayAntiAliasing);
    connect(AANbRaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(setNbRayAntiAliasing(int)));
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
    connect(AONbRaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeAmbientOcclusionNbRays(int)));

    AOMaxAngleSpinBox = new QSpinBox(AOGroupBox);
    AOMaxAngleSpinBox->setPrefix ("Max angle: ");
    AOMaxAngleSpinBox->setSuffix (" degrees");
    AOMaxAngleSpinBox->setMinimum (0);
    AOMaxAngleSpinBox->setMaximum (180);
    AOMaxAngleSpinBox->setVisible(false);
    AOMaxAngleSpinBox->setValue(rayTracer->maxAngleAmbientOcclusion*360.0/(2.0*M_PI));
    connect(AOMaxAngleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setAmbientOcclusionMaxAngle(int)));
    AOLayout->addWidget(AOMaxAngleSpinBox);

    AORadiusSpinBox = new QDoubleSpinBox(AOGroupBox);
    AORadiusSpinBox->setPrefix("Radius: ");
    AORadiusSpinBox->setMinimum(0);
    AORadiusSpinBox->setSingleStep(0.1);
    AORadiusSpinBox->setValue(rayTracer->radiusAmbientOcclusion);
    AORadiusSpinBox->setVisible(false);
    connect(AORadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setAmbientOcclusionRadius(double)));
    AOLayout->addWidget(AORadiusSpinBox);

    rayLayout->addWidget(AOGroupBox);

    //  Shadows
    QGroupBox * shadowsGroupBox = new QGroupBox ("Shadows", rayGroupBox);
    QVBoxLayout * shadowsLayout = new QVBoxLayout (shadowsGroupBox);

    QComboBox *shadowTypeList = new QComboBox(shadowsGroupBox);
    shadowTypeList->addItem("None");
    shadowTypeList->addItem("Hard shadow");
    shadowTypeList->addItem("Soft shadow");
    connect (shadowTypeList, SIGNAL (activated (int)), this, SLOT (setShadowMode (int)));
    shadowsLayout->addWidget (shadowTypeList);

    shadowSpinBox = new QSpinBox(shadowsGroupBox);
    shadowSpinBox->setSuffix (" rays");
    shadowSpinBox->setMinimum (2);
    shadowSpinBox->setMaximum (1000);
    shadowSpinBox->setVisible (false);
    shadowSpinBox->setValue(rayTracer->getShadowMode());
    connect (shadowSpinBox, SIGNAL (valueChanged(int)), this, SLOT (setShadowNbRays (int)));
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
    connect (PTDepthSpinBox, SIGNAL (valueChanged(int)), this, SLOT (setDepthPathTracing (int)));
    PTLayout->addWidget (PTDepthSpinBox);

    PTNbRaySpinBox = new QSpinBox(PTGroupBox);
    PTNbRaySpinBox->setSuffix (" rays");
    PTNbRaySpinBox->setMinimum (1);
    PTNbRaySpinBox->setMaximum (1000);
    PTNbRaySpinBox->setVisible(false);
    PTNbRaySpinBox->setValue(rayTracer->nbRayPathTracing);
    connect (PTNbRaySpinBox, SIGNAL (valueChanged(int)), this, SLOT (setNbRayPathTracing (int)));
    PTLayout->addWidget (PTNbRaySpinBox);

    PTMaxAngleSpinBox = new QSpinBox(PTGroupBox);
    PTMaxAngleSpinBox->setPrefix ("Max angle: ");
    PTMaxAngleSpinBox->setSuffix (" degrees");
    PTMaxAngleSpinBox->setMinimum (0);
    PTMaxAngleSpinBox->setMaximum (180);
    PTMaxAngleSpinBox->setVisible(false);
    PTMaxAngleSpinBox->setValue(rayTracer->maxAnglePathTracing*360.0/(2.0*M_PI));
    connect (PTMaxAngleSpinBox, SIGNAL (valueChanged(int)), this, SLOT (setMaxAnglePathTracing (int)));
    PTLayout->addWidget (PTMaxAngleSpinBox);

    PTIntensitySpinBox = new QSpinBox(PTGroupBox);
    PTIntensitySpinBox->setPrefix ("Intensity: ");
    PTIntensitySpinBox->setMinimum (1);
    PTIntensitySpinBox->setMaximum (1000);
    PTIntensitySpinBox->setVisible(false);
    PTIntensitySpinBox->setValue(rayTracer->intensityPathTracing);
    connect (PTIntensitySpinBox, SIGNAL (valueChanged(int)), this, SLOT (setIntensityPathTracing (int)));
    PTLayout->addWidget (PTIntensitySpinBox);

    PTOnlyCheckBox = new QCheckBox ("Only path tracing coloring", PTGroupBox);
    connect (PTOnlyCheckBox, SIGNAL (toggled (bool)), this, SLOT (setOnlyPT (bool)));
    PTOnlyCheckBox->setVisible(false);
    PTLayout->addWidget (PTOnlyCheckBox);

    rayLayout->addWidget (PTGroupBox);

    //  Focal
    QGroupBox * focalGroupBox = new QGroupBox ("Focal", rayGroupBox);
    QVBoxLayout * focalLayout = new QVBoxLayout (focalGroupBox);

    QCheckBox * focalCheckBox = new QCheckBox ("Enable Focus", focalGroupBox);
    connect (focalCheckBox, SIGNAL (toggled (bool)), this, SLOT (enableFocal (bool)));
    focalLayout->addWidget (focalCheckBox);

    selecFocusedObject  = new QPushButton ("", focalGroupBox);
    selecFocusedObject->setVisible(false);
    connect (selecFocusedObject, SIGNAL (clicked ()) , this, SLOT ( setFocal()));
    focalLayout->addWidget (selecFocusedObject);

    rayLayout->addWidget (focalGroupBox);

    // Motion Blur
    if(scene->hasMobile()) {
        QGroupBox * mBlurGroupBox = new QGroupBox ("Motion blur", rayGroupBox);
        QVBoxLayout * mBlurLayout = new QVBoxLayout (mBlurGroupBox);

        QSpinBox * mBlurNbImagesSpinBox = new QSpinBox(PTGroupBox);
        mBlurNbImagesSpinBox->setSuffix (" images");
        mBlurNbImagesSpinBox->setMinimum (1);
        mBlurNbImagesSpinBox->setMaximum (100);
        connect (mBlurNbImagesSpinBox, SIGNAL (valueChanged(int)), this, SLOT (setNbImagesSpinBox (int)));
        mBlurLayout->addWidget (mBlurNbImagesSpinBox);

        rayLayout->addWidget (mBlurGroupBox);
    }

    // Render
    QPushButton * rayButton = new QPushButton ("Render", rayGroupBox);
    rayLayout->addWidget (rayButton);
    connect (rayButton, SIGNAL (clicked ()), this, SLOT (renderRayImage ()));
    QPushButton * showButton = new QPushButton ("Show", rayGroupBox);
    rayLayout->addWidget (showButton);
    connect (showButton, SIGNAL (clicked ()), this, SLOT (showRayImage ()));
    QPushButton * saveButton  = new QPushButton ("Save", rayGroupBox);
    connect (saveButton, SIGNAL (clicked ()) , this, SLOT (exportRayImage ()));
    rayLayout->addWidget (saveButton);

    layout->addWidget (rayGroupBox);

    // Global settings
    QGroupBox * globalGroupBox = new QGroupBox ("Global Settings", controlWidget);
    QVBoxLayout * globalLayout = new QVBoxLayout (globalGroupBox);

    QPushButton * bgColorButton  = new QPushButton ("Background Color", globalGroupBox);
    connect (bgColorButton, SIGNAL (clicked()) , this, SLOT (setBGColor()));
    globalLayout->addWidget (bgColorButton);

    QPushButton * aboutButton  = new QPushButton ("About", globalGroupBox);
    connect (aboutButton, SIGNAL (clicked()) , this, SLOT (about()));
    globalLayout->addWidget (aboutButton);

    QPushButton * quitButton  = new QPushButton ("Quit", globalGroupBox);
    connect (quitButton, SIGNAL (clicked()) , qApp, SLOT (closeAllWindows()));
    globalLayout->addWidget (quitButton);

    layout->addWidget (globalGroupBox);

    layout->addStretch (0);
}
