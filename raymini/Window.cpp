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
#include <QTabWidget>

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

Vec3Df Window::getObjectPos() const {
    Vec3Df newPos;
    for (int i=0; i<3; i++) {
        newPos[i] = objectPosSpinBoxes[i]->value();
    }
    return newPos;
}

Vec3Df Window::getObjectMobile() const {
    Vec3Df newMobile;
    for (int i=0; i<3; i++) {
        newMobile[i] = objectMobileSpinBoxes[i]->value();
    }
    return newMobile;
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

Vec3Df Window::getTextureColor() const {
    Vec3Df newColor;
    for (int i=0; i<3; i++) {
        newColor[i] = textureColorSpinBoxes[i]->value();
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

    // Materials
    updateMaterials();

    // Textures
    updateTextures();

    // Motion blur
    updateMotionBlur();
}

void Window::updateFromRayTracer() {
    RayTracer *rayTracer = controller->getRayTracer();

    // Shadows
    shadowTypeList->setCurrentIndex(rayTracer->getShadowMode());
    shadowSpinBox->setVisible(rayTracer->getShadowMode() == Shadow::SOFT);
    shadowSpinBox->disconnect();
    shadowSpinBox->setValue(rayTracer->getShadowNbImpulse());
    connect(shadowSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetShadowNbRays(int)));

    // Anti aliasing
    AANbRaySpinBox->setVisible(rayTracer->typeAntiAliasing != AntiAliasing::NONE);
    AANbRaySpinBox->disconnect();
    AANbRaySpinBox->setValue(rayTracer->nbRayAntiAliasing);
    connect(AANbRaySpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetNbRayAntiAliasing(int)));

    // Ambient occlusion
    bool isAO = rayTracer->nbRayAmbientOcclusion != 0;
    AONbRaysSpinBox->disconnect();
    AONbRaysSpinBox->setValue(rayTracer->nbRayAmbientOcclusion);
    connect(AONbRaysSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetAmbientOcclusionNbRays(int)));
    AORadiusSpinBox->setVisible(isAO);
    AOMaxAngleSpinBox->setVisible(isAO);
    AORadiusSpinBox->disconnect();
    AORadiusSpinBox->setValue(rayTracer->radiusAmbientOcclusion);
    connect(AORadiusSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetAmbientOcclusionRadius(double)));
    AOMaxAngleSpinBox->disconnect();
    int AOAngle = rayTracer->maxAngleAmbientOcclusion*360.0/(2.0*M_PI)+0.5;
    AOMaxAngleSpinBox->setValue(AOAngle);
    connect(AOMaxAngleSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetAmbientOcclusionMaxAngle(int)));

    // Focus
    updateFocus();

    // Path tracing
    bool isPT = rayTracer->depthPathTracing != 0;
    PTDepthSpinBox->disconnect();
    PTDepthSpinBox->setValue(rayTracer->depthPathTracing);
    connect (PTDepthSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetDepthPathTracing (int)));
    PTNbRaySpinBox->setVisible(isPT);
    PTMaxAngleSpinBox->setVisible(isPT);
    PTOnlyCheckBox->setVisible(isPT);
    PBGICheckBox->setVisible(!isPT);
    if (isPT) {
        PTNbRaySpinBox->disconnect();
        PTNbRaySpinBox->setValue(rayTracer->nbRayPathTracing);
        connect(PTNbRaySpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetNbRayPathTracing (int)));
        PTMaxAngleSpinBox->disconnect();
        int PTAngle = rayTracer->maxAnglePathTracing*360.0/(2.0*M_PI) + 0.5;
        PTMaxAngleSpinBox->setValue(PTAngle);
        connect(PTMaxAngleSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetMaxAnglePathTracing(int)));
        PTIntensitySpinBox->disconnect();
        PTIntensitySpinBox->setValue(rayTracer->intensityPathTracing);
        connect(PTIntensitySpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetIntensityPathTracing(int)));
    }

    // Real time
    updateRealTime();

    // Motion blur
    updateMotionBlur();
}

void Window::updateFromWindowModel() {
    // Lights
    updateLights();

    // Focus
    updateFocus();

    // Objects
    updateObjects();

    // Materials
    updateMaterials();

    // Textures
    updateTextures();

    // Real time
    updateRealTime();

    // Status
    updateStatus();

    // Preview
    updatePreview();
}

void Window::updatePreview() {
    WindowModel *windowModel = controller->getWindowModel();
    wireframeCheckBox->setChecked(windowModel->isWireframe());
    modeList->setCurrentIndex(windowModel->getRenderingMode());
    surfelsCheckBox->setChecked(windowModel->isShowSurfel());
    kdtreeCheckBox->setChecked(windowModel->isShowKDTree());
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
            lightPosSpinBoxes[i]->disconnect();
            lightPosSpinBoxes[i]->setValue(pos[i]);
            connect(lightPosSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightPos()));
            lightColorSpinBoxes[i]->disconnect();
            lightColorSpinBoxes[i]->setValue(color[i]);
            connect(lightColorSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightColor()));
        }
        lightIntensitySpinBox->disconnect();
        lightIntensitySpinBox->setValue(intensity);
        connect(lightIntensitySpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightIntensity(double)));
        lightRadiusSpinBox->disconnect();
        lightRadiusSpinBox->setValue(radius);
        connect(lightRadiusSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightRadius(double)));
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
    focusNbRaysSpinBox->disconnect();
    focusNbRaysSpinBox->setValue(rayTracer->nbRayFocus);
    connect(focusNbRaysSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetFocusNbRays(int)));
    focusApertureSpinBox->disconnect();
    focusApertureSpinBox->setValue(rayTracer->apertureFocus);
    connect(focusApertureSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetFocusAperture(double)));
}

void Window::updateObjects() {
    Scene *scene = controller->getScene();
    WindowModel *windowModel = controller->getWindowModel();

    int index = windowModel->getSelectedObjectIndex();
    objectsList->setCurrentIndex(index+1);
    bool isSelected = index != -1;
    objectEnableCheckBox->setVisible(isSelected);
    objectMobileLabel->setVisible(isSelected);
    for (unsigned int i=0; i<3; i++) {
        objectPosSpinBoxes[i]->setVisible(isSelected);
        objectMobileSpinBoxes[i]->setVisible(isSelected);
    }
    objectMaterialLabel->setVisible(isSelected);
    objectMaterialsList->setVisible(isSelected);
    if (isSelected) {
        const Object *object = scene->getObjects()[index];
        bool isEnabled = object->isEnabled();
        objectEnableCheckBox->setChecked(isEnabled);
        for (unsigned int i=0; i<3; i++) {
            objectPosSpinBoxes[i]->disconnect();
            objectPosSpinBoxes[i]->setValue(object->getTrans()[i]);
            connect(objectPosSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetObjectPos()));
            objectMobileSpinBoxes[i]->disconnect();
            objectMobileSpinBoxes[i]->setValue(object->getMobile()[i]);
            connect(objectMobileSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetObjectMobile()));
        }
        objectMaterialsList->setCurrentIndex(scene->getObjectMaterialIndex(index));
    }
}

void Window::updateMaterials() {
    Scene *scene = controller->getScene();
    WindowModel *windowModel = controller->getWindowModel();

    int index = windowModel->getSelectedMaterialIndex();
    materialsList->setCurrentIndex(index+1);
    bool isSelected = index != -1;
    materialDiffuseSpinBox->setVisible(isSelected);
    materialSpecularSpinBox->setVisible(isSelected);
    materialGlossyRatio->setVisible(isSelected);
    materialTextureLabel->setVisible(isSelected);
    materialTexturesList->setVisible(isSelected);

    if (isSelected) {
        const Material *material = scene->getMaterials()[index];
        materialDiffuseSpinBox->disconnect();
        materialDiffuseSpinBox->setValue(material->getDiffuse());
        connect(materialDiffuseSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetMaterialDiffuse(double)));
        materialSpecularSpinBox->disconnect();
        materialSpecularSpinBox->setValue(material->getSpecular());
        connect(materialSpecularSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetMaterialSpecular(double)));
        materialGlossyRatio->disconnect();
        materialGlossyRatio->setValue(material->getGlossyRatio());
        connect(materialGlossyRatio, SIGNAL(valueChanged(double)), controller, SLOT(windowSetMaterialGlossyRatio(double)));
        materialTexturesList->setCurrentIndex(scene->getMaterialTextureIndex(index));
    }
}

void Window::updateTextures() {
    Scene *scene = controller->getScene();
    WindowModel *windowModel = controller->getWindowModel();

    int index = windowModel->getSelectedTextureIndex();
    texturesList->setCurrentIndex(index+1);
    bool isSelected = index != -1;
    for (unsigned int i=0; i<3; i++) {
        textureColorSpinBoxes[i]->setVisible(isSelected);
    }

    if (isSelected) {
        const Texture *texture = scene->getTextures()[index];
        for (unsigned int i=0; i<3; i++) {
            textureColorSpinBoxes[i]->disconnect();
            textureColorSpinBoxes[i]->setValue(texture->getRepresentativeColor()[i]);
            connect(textureColorSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetTextureColor()));
        }
    }
}

void Window::updateProgressBar() {
    RenderThread *renderThread = controller->getRenderThread();
    WindowModel *windowModel = controller->getWindowModel();
    bool isRendering = renderThread->isRendering();
    bool isRealTime = windowModel->isRealTime();
    stopRenderButton->setVisible(isRendering||isRealTime);
    renderButton->setVisible(!isRendering && !isRealTime);
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
    durtiestQualityComboBox->setVisible(isRealTime);
    durtiestQualityLabel->setVisible(isRealTime);
    if (isRealTime) {
        RayTracer *rayTracer = controller->getRayTracer();
        int quality = rayTracer->durtiestQuality;
        durtiestQualityComboBox->setCurrentIndex(quality);
        qualityDividerSpinBox->setVisible(quality == RayTracer::Quality::ONE_OVER_X);
        qualityDividerSpinBox->disconnect();
        qualityDividerSpinBox->setValue(rayTracer->durtiestQualityDivider);
        connect(qualityDividerSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetQualityDivider(int)));
    }
    else {
        qualityDividerSpinBox->setVisible(false);
    }
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
    int divider = rayTracer->qualityDivider;
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
                QString(RayTracer::qualityToString(quality, divider))+
                QString(" quality...");
        }
        statusBar()->showMessage(message);
    }
}

void Window::updateMotionBlur() {
    Scene *scene = controller->getScene();
    RayTracer *rayTracer = controller->getRayTracer();
    mBlurGroupBox->setVisible(scene->hasMobile());
    mBlurNbImagesSpinBox->disconnect();
    mBlurNbImagesSpinBox->setValue(rayTracer->nbPictures);
    connect (mBlurNbImagesSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetNbImagesSpinBox (int)));
}

void Window::initControlWidget () {
    Scene *scene = controller->getScene();

    // Control widget
    controlWidget = new QGroupBox ();
    QGridLayout * layout = new QGridLayout (controlWidget);

    // Preview
    QGroupBox * previewGroupBox = new QGroupBox ("Preview", controlWidget);
    QVBoxLayout * previewLayout = new QVBoxLayout (previewGroupBox);

    wireframeCheckBox = new QCheckBox ("Wireframe", previewGroupBox);
    connect(wireframeCheckBox, SIGNAL(clicked(bool)), controller, SLOT(viewerSetWireframe(bool)));
    previewLayout->addWidget (wireframeCheckBox);

    modeList = new QComboBox(previewGroupBox);
    modeList->addItem("Smooth");
    modeList->addItem("Flat");
    previewLayout->addWidget(modeList);
    connect (modeList, SIGNAL(activated (int)), controller, SLOT(viewerSetRenderingMode(int)));

    surfelsCheckBox = new QCheckBox("Show surfels", previewGroupBox);
    connect(surfelsCheckBox, SIGNAL(clicked(bool)), controller, SLOT(viewerSetShowSurfel(bool)));
    previewLayout->addWidget(surfelsCheckBox);

    kdtreeCheckBox = new QCheckBox("Show KD-tree", previewGroupBox);
    connect(kdtreeCheckBox, SIGNAL(clicked(bool)), controller, SLOT(viewerSetShowKDTree(bool)));
    previewLayout->addWidget(kdtreeCheckBox);

    QPushButton * snapshotButton  = new QPushButton ("Save preview", previewGroupBox);
    connect (snapshotButton, SIGNAL(clicked ()) ,controller, SLOT(windowExportGLImage()));
    previewLayout->addWidget (snapshotButton);

    layout->addWidget(previewGroupBox, 0, 0, 2, 1);




    // Ray tracing
    QGroupBox * rayGroupBox = new QGroupBox("Ray Tracing", controlWidget);
    QVBoxLayout *rayLayout = new QVBoxLayout(rayGroupBox);
    QTabWidget * rayTabs = new QTabWidget(rayGroupBox);
    rayTabs->setUsesScrollButtons(false);

    //  RayGroup: Anti Aliasing
    QWidget * AAGroupBox = new QWidget(rayTabs);
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
    AALayout->addWidget(AANbRaySpinBox);
    connect(AANbRaySpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetNbRayAntiAliasing(int)));

    rayTabs->addTab(AAGroupBox, "Anti Aliasing");

    //  RayGroup: Ambient occlusion
    QWidget * AOGroupBox = new QWidget(rayTabs);
    QVBoxLayout * AOLayout = new QVBoxLayout (AOGroupBox);

    AONbRaysSpinBox = new QSpinBox(AOGroupBox);
    AONbRaysSpinBox->setSuffix(" rays");
    AONbRaysSpinBox->setMinimum(0);
    AONbRaysSpinBox->setMaximum(1000);
    connect(AONbRaysSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetAmbientOcclusionNbRays(int)));
    AOLayout->addWidget(AONbRaysSpinBox);

    AOMaxAngleSpinBox = new QSpinBox(AOGroupBox);
    AOMaxAngleSpinBox->setPrefix ("Max angle: ");
    AOMaxAngleSpinBox->setSuffix (" degrees");
    AOMaxAngleSpinBox->setMinimum (0);
    AOMaxAngleSpinBox->setMaximum (180);
    connect(AOMaxAngleSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetAmbientOcclusionMaxAngle(int)));
    AOLayout->addWidget(AOMaxAngleSpinBox);

    AORadiusSpinBox = new QDoubleSpinBox(AOGroupBox);
    AORadiusSpinBox->setPrefix("Radius: ");
    AORadiusSpinBox->setMinimum(0);
    AORadiusSpinBox->setSingleStep(0.1);
    connect(AORadiusSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetAmbientOcclusionRadius(double)));
    AOLayout->addWidget(AORadiusSpinBox);

    QCheckBox * AOOnlyCheckBox = new QCheckBox ("Only ambient coloring", AOGroupBox);
    connect (AOOnlyCheckBox, SIGNAL (toggled (bool)), controller, SLOT(windowSetOnlyAO (bool)));
    AOLayout->addWidget (AOOnlyCheckBox);

    rayTabs->addTab(AOGroupBox, "Ambient Occlusion");

    //  RayGroup: Shadows
    QWidget * shadowsGroupBox = new QWidget(rayTabs);
    QVBoxLayout * shadowsLayout = new QVBoxLayout (shadowsGroupBox);

    shadowTypeList = new QComboBox(shadowsGroupBox);
    shadowTypeList->addItem("None");
    shadowTypeList->addItem("Hard shadow");
    shadowTypeList->addItem("Soft shadow");
    connect (shadowTypeList, SIGNAL (activated (int)), controller, SLOT (windowSetShadowMode (int)));
    shadowsLayout->addWidget (shadowTypeList);

    shadowSpinBox = new QSpinBox(shadowsGroupBox);
    shadowSpinBox->setSuffix (" rays");
    shadowSpinBox->setMinimum (2);
    shadowSpinBox->setMaximum (1000);
    connect(shadowSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetShadowNbRays(int)));
    shadowsLayout->addWidget (shadowSpinBox);

    rayTabs->addTab(shadowsGroupBox, "Shadows");

    //  RayGroup: Path Tracing
    QWidget * PTGroupBox = new QWidget(rayTabs);
    QVBoxLayout * PTLayout = new QVBoxLayout (PTGroupBox);

    PTDepthSpinBox = new QSpinBox(PTGroupBox);
    PTDepthSpinBox->setPrefix ("Depth: ");
    PTDepthSpinBox->setMinimum (0);
    PTDepthSpinBox->setMaximum (5);
    connect (PTDepthSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetDepthPathTracing (int)));
    PTLayout->addWidget (PTDepthSpinBox);

    PTNbRaySpinBox = new QSpinBox(PTGroupBox);
    PTNbRaySpinBox->setSuffix (" rays");
    PTNbRaySpinBox->setMinimum (1);
    PTNbRaySpinBox->setMaximum (1000);
    connect(PTNbRaySpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetNbRayPathTracing (int)));
    PTLayout->addWidget (PTNbRaySpinBox);

    PTMaxAngleSpinBox = new QSpinBox(PTGroupBox);
    PTMaxAngleSpinBox->setPrefix ("Max angle: ");
    PTMaxAngleSpinBox->setSuffix (" degrees");
    PTMaxAngleSpinBox->setMinimum (0);
    PTMaxAngleSpinBox->setMaximum (180);
    connect(PTMaxAngleSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetMaxAnglePathTracing(int)));
    PTLayout->addWidget (PTMaxAngleSpinBox);

    PTIntensitySpinBox = new QSpinBox(PTGroupBox);
    PTIntensitySpinBox->setPrefix ("Intensity: ");
    PTIntensitySpinBox->setMinimum (1);
    PTIntensitySpinBox->setMaximum (1000);
    connect(PTIntensitySpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetIntensityPathTracing(int)));
    PTLayout->addWidget (PTIntensitySpinBox);

    PTOnlyCheckBox = new QCheckBox ("Only path tracing coloring", PTGroupBox);
    connect (PTOnlyCheckBox, SIGNAL (clicked (bool)), controller, SLOT (windowSetOnlyPT (bool)));
    PTLayout->addWidget (PTOnlyCheckBox);

    PBGICheckBox = new QCheckBox ("PBGI mode", PTGroupBox);
    connect (PBGICheckBox, SIGNAL (clicked (bool)), controller, SLOT (windowSetRayTracerMode (bool)));
    PTLayout->addWidget (PBGICheckBox);

    rayTabs->addTab(PTGroupBox, "Path Tracing");

    //  RayGroup: Focal
    QWidget * focalGroupBox = new QWidget(rayTabs);
    QVBoxLayout * focalLayout = new QVBoxLayout(focalGroupBox);

    focusTypeComboBox = new QComboBox(focalGroupBox);
    focusTypeComboBox->addItem("No focus");
    focusTypeComboBox->addItem("Uniform");
    focusTypeComboBox->addItem("Stochastic");
    connect(focusTypeComboBox, SIGNAL(activated(int)), controller, SLOT(windowSetFocusType(int)));
    focalLayout->addWidget(focusTypeComboBox);

    changeFocusFixingCheckBox = new QCheckBox("Focal point is fixed", focalGroupBox);
    connect(changeFocusFixingCheckBox, SIGNAL(clicked(bool)), controller, SLOT(windowSetFocalFixing(bool)));
    focalLayout->addWidget(changeFocusFixingCheckBox);

    focusNbRaysSpinBox = new QSpinBox(focalGroupBox);
    focusNbRaysSpinBox->setSuffix(" rays");
    focusNbRaysSpinBox->setMinimum(1);
    connect(focusNbRaysSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetFocusNbRays(int)));
    focalLayout->addWidget(focusNbRaysSpinBox);

    focusApertureSpinBox = new QDoubleSpinBox(focalGroupBox);
    focusApertureSpinBox->setPrefix("Aperture: ");
    focusApertureSpinBox->setMinimum(0.01);
    focusApertureSpinBox->setMaximum(0.3);
    focusApertureSpinBox->setSingleStep(0.01);
    connect(focusApertureSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetFocusAperture(double)));
    focalLayout->addWidget(focusApertureSpinBox);

    rayTabs->addTab(focalGroupBox, "Focal");

    // RayGroup: Motion Blur
    mBlurGroupBox = new QWidget(rayTabs);
    QVBoxLayout * mBlurLayout = new QVBoxLayout (mBlurGroupBox);

    mBlurNbImagesSpinBox = new QSpinBox(PTGroupBox);
    mBlurNbImagesSpinBox->setSuffix (" images");
    mBlurNbImagesSpinBox->setMinimum (1);
    mBlurNbImagesSpinBox->setMaximum (100);
    connect (mBlurNbImagesSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetNbImagesSpinBox (int)));
    mBlurLayout->addWidget (mBlurNbImagesSpinBox);

    rayTabs->addTab(mBlurGroupBox, "Motion Blur");

    rayLayout->addWidget(rayTabs);

    layout->addWidget(rayGroupBox, 0, 1, 3, 1);



    // scene param
    QGroupBox *sceneGroupBox = new QGroupBox("Scene", controlWidget);
    QVBoxLayout *sceneLayout = new QVBoxLayout(sceneGroupBox);
    QTabWidget *sceneTabs = new QTabWidget(sceneGroupBox);
    sceneTabs->setUsesScrollButtons(false);

    //  SceneGroup: Objects
    QWidget *objectsGroupBox = new QWidget(sceneTabs);
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
    connect(objectEnableCheckBox, SIGNAL(clicked(bool)), controller, SLOT(windowEnableObject(bool)));
    objectsLayout->addWidget(objectEnableCheckBox);

    QHBoxLayout *objectPosLayout = new QHBoxLayout;
    QString objectPosNames[3] = {"X: ", "Y: ", "Z: "};
    for (unsigned int i=0; i<3; i++) {
        objectPosSpinBoxes[i] = new QDoubleSpinBox(objectsGroupBox);
        objectPosSpinBoxes[i]->setMinimum(-100);
        objectPosSpinBoxes[i]->setMaximum(100);
        objectPosSpinBoxes[i]->setSingleStep(0.01);
        objectPosSpinBoxes[i]->setPrefix(objectPosNames[i]);
        connect(objectPosSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetObjectPos()));
        objectPosLayout->addWidget(objectPosSpinBoxes[i]);
    }
    objectsLayout->addLayout(objectPosLayout);

    objectMobileLabel = new QLabel("Movement: (nulify all to immobilise)", objectsGroupBox);
    objectsLayout->addWidget(objectMobileLabel);

    QHBoxLayout *objectMobileLayout = new QHBoxLayout;
    QString objectMobileNames[3] = {"X: ", "Y: ", "Z: "};
    for (unsigned int i=0; i<3; i++) {
        objectMobileSpinBoxes[i] = new QDoubleSpinBox(objectsGroupBox);
        objectMobileSpinBoxes[i]->setMinimum(-100);
        objectMobileSpinBoxes[i]->setMaximum(100);
        objectMobileSpinBoxes[i]->setSingleStep(0.01);
        objectMobileSpinBoxes[i]->setPrefix(objectMobileNames[i]);
        connect(objectMobileSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetObjectMobile()));
        objectMobileLayout->addWidget(objectMobileSpinBoxes[i]);
    }
    objectsLayout->addLayout(objectMobileLayout);

    QHBoxLayout *objectMaterialsLayout = new QHBoxLayout;

    objectMaterialLabel = new QLabel("Material:", objectsGroupBox);
    objectMaterialsLayout->addWidget(objectMaterialLabel);

    objectMaterialsList = new QComboBox(objectsGroupBox);
    for (const Material *mat : scene->getMaterials()) {
        objectMaterialsList->addItem(QString(mat->getName().c_str()));
    }
    connect(objectMaterialsList, SIGNAL(activated(int)), controller, SLOT(windowSetObjectMaterial(int)));
    objectMaterialsLayout->addWidget(objectMaterialsList);

    objectsLayout->addLayout(objectMaterialsLayout);

    sceneTabs->addTab(objectsGroupBox, "Objects");


    //  SceneGroup: Materials
    QWidget *materialsGroupBox = new QWidget(sceneTabs);
    QVBoxLayout *materialsLayout = new QVBoxLayout(materialsGroupBox);

    materialsList = new QComboBox(materialsGroupBox);
    materialsList->addItem("No material selected");
    for (const Material *material : scene->getMaterials()) {
        materialsList->addItem(QString(material->getName().c_str()));
    }
    connect(materialsList, SIGNAL(activated(int)), controller, SLOT(windowSelectMaterial(int)));
    materialsLayout->addWidget(materialsList);

    materialDiffuseSpinBox = new QDoubleSpinBox(materialsGroupBox);
    materialDiffuseSpinBox->setPrefix("Diffuse: ");
    materialDiffuseSpinBox->setMinimum(0);
    materialDiffuseSpinBox->setMaximum(1);
    materialDiffuseSpinBox->setSingleStep(0.01);
    connect(materialDiffuseSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetMaterialDiffuse(double)));
    materialsLayout->addWidget(materialDiffuseSpinBox);

    materialSpecularSpinBox = new QDoubleSpinBox(materialsGroupBox);
    materialSpecularSpinBox->setPrefix("Specular: ");
    materialSpecularSpinBox->setMinimum(0);
    materialSpecularSpinBox->setMaximum(1);
    materialSpecularSpinBox->setSingleStep(0.01);
    connect(materialSpecularSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetMaterialSpecular(double)));
    materialsLayout->addWidget(materialSpecularSpinBox);

    materialGlossyRatio = new QDoubleSpinBox(materialsGroupBox);
    materialGlossyRatio->setMinimum(0);
    materialGlossyRatio->setMaximum(1);
    materialGlossyRatio->setSingleStep(0.01);
    materialGlossyRatio->setPrefix("Glossy ratio: ");
    connect(materialGlossyRatio, SIGNAL(valueChanged(double)), controller, SLOT(windowSetMaterialGlossyRatio(double)));
    materialsLayout->addWidget(materialGlossyRatio);

    QHBoxLayout *materialTexturesLayout = new QHBoxLayout;

    materialTextureLabel = new QLabel("Texture:", materialsGroupBox);
    materialTexturesLayout->addWidget(materialTextureLabel);

    materialTexturesList = new QComboBox(materialsGroupBox);
    for (const Texture * t : scene->getTextures()) {
        materialTexturesList->addItem(t->getName().c_str());
    }
    connect(materialTexturesList, SIGNAL(activated(int)), controller, SLOT(windowSetMaterialTexture(int)));
    materialTexturesLayout->addWidget(materialTexturesList);

    materialsLayout->addLayout(materialTexturesLayout);

    sceneTabs->addTab(materialsGroupBox, "Materials");

    // SceneGroup: Textures

    QWidget *texturesGroupBox = new QWidget(sceneTabs);
    QVBoxLayout *texturesLayout = new QVBoxLayout(texturesGroupBox);

    texturesList = new QComboBox(texturesGroupBox);
    texturesList->addItem("No texture selected");
    for (const Texture *t : scene->getTextures()) {
        texturesList->addItem(t->getName().c_str());
    }
    connect(texturesList, SIGNAL(activated(int)), controller, SLOT(windowSelectTexture(int)));
    texturesLayout->addWidget(texturesList);

    QHBoxLayout *textureColorLayout = new QHBoxLayout;
    QString textureColorsName[3] = {"R: ", "G: ", "B: "};
    for (unsigned int i=0; i<3; i++) {
        textureColorSpinBoxes[i] = new QDoubleSpinBox(texturesGroupBox);
        textureColorSpinBoxes[i]->setMinimum(0);
        textureColorSpinBoxes[i]->setMaximum(1);
        textureColorSpinBoxes[i]->setSingleStep(0.01);
        textureColorSpinBoxes[i]->setPrefix(textureColorsName[i]);
        connect(textureColorSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetTextureColor()));
        textureColorLayout->addWidget(textureColorSpinBoxes[i]);
    }
    texturesLayout->addLayout(textureColorLayout);

    sceneTabs->addTab(texturesGroupBox, "Textures");

    //  SceneGroup: Lights
    QWidget *lightsGroupBox = new QWidget(sceneTabs);
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
        lightPosSpinBoxes[i]->setPrefix(axis[i]);
        connect(lightPosSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightPos()));
        lightsPosLayout->addWidget(lightPosSpinBoxes[i]);
        lightColorSpinBoxes[i] = new QDoubleSpinBox(lightsGroupBox);
        lightColorSpinBoxes[i]->setSingleStep(0.01);
        lightColorSpinBoxes[i]->setMinimum(0);
        lightColorSpinBoxes[i]->setMaximum(1);
        lightColorSpinBoxes[i]->setPrefix(colors[i]);
        connect(lightColorSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightColor()));
        lightsColorLayout->addWidget(lightColorSpinBoxes[i]);
    }
    lightsLayout->addLayout(lightsPosLayout);
    lightsLayout->addLayout(lightsColorLayout);

    lightRadiusSpinBox = new QDoubleSpinBox(lightsGroupBox);
    lightRadiusSpinBox->setSingleStep(0.01);
    lightRadiusSpinBox->setMinimum(0);
    lightRadiusSpinBox->setMaximum(100);
    lightRadiusSpinBox->setPrefix ("Radius: ");
    connect(lightRadiusSpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightRadius(double)));
    lightsLayout->addWidget(lightRadiusSpinBox);

    lightIntensitySpinBox = new QDoubleSpinBox(lightsGroupBox);
    lightIntensitySpinBox->setSingleStep(0.1);
    lightIntensitySpinBox->setMinimum(0);
    lightIntensitySpinBox->setMaximum(100);
    lightIntensitySpinBox->setPrefix("Intensity: ");
    connect(lightIntensitySpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightIntensity(double)));
    lightsLayout->addWidget(lightIntensitySpinBox);

    sceneTabs->addTab(lightsGroupBox, "Lights");

    sceneLayout->addWidget(sceneTabs);

    layout->addWidget(sceneGroupBox, 3, 1, 3, 1);


    // Render
    QGroupBox * ActionGroupBox = new QGroupBox ("Action", sceneTabs);
    QVBoxLayout * actionLayout = new QVBoxLayout (ActionGroupBox);

    stopRenderButton = new QPushButton("Stop", sceneTabs);
    actionLayout->addWidget(stopRenderButton);
    connect(stopRenderButton, SIGNAL(clicked()), controller, SLOT(windowStopRendering()));

    renderButton = new QPushButton ("Render", sceneTabs);
    actionLayout->addWidget (renderButton);
    connect (renderButton, SIGNAL (clicked ()), controller, SLOT (windowRenderRayImage ()));

    renderProgressBar = new QProgressBar(sceneTabs);
    renderProgressBar->setMinimum(0);
    renderProgressBar->setMaximum(100);
    actionLayout->addWidget(renderProgressBar);

    realTimeCheckBox = new QCheckBox("Real time", sceneTabs);
    connect(realTimeCheckBox, SIGNAL(clicked(bool)), controller, SLOT(windowSetRealTime(bool)));
    actionLayout->addWidget(realTimeCheckBox);

    durtiestQualityLabel = new QLabel("Durtiest quality:", sceneTabs);
    actionLayout->addWidget(durtiestQualityLabel);

    QHBoxLayout *durtiestLayout = new QHBoxLayout;

    durtiestQualityComboBox = new QComboBox(sceneTabs);
    durtiestQualityComboBox->addItem("Optimal");
    durtiestQualityComboBox->addItem("Basic");
    durtiestQualityComboBox->addItem("One over");
    connect(durtiestQualityComboBox, SIGNAL(activated(int)), controller, SLOT(windowSetDurtiestQuality(int)));
    durtiestLayout->addWidget(durtiestQualityComboBox);

    class SquareSpinBox: public QSpinBox {
    public:
        SquareSpinBox(QWidget *w): QSpinBox(w) {}
    protected:
        QString textFromValue(int value) const
        {
            return QString("%1 x %1 pixels").arg(value);
        }
    };
    qualityDividerSpinBox = new SquareSpinBox(sceneTabs);
    qualityDividerSpinBox->setMinimum(2);
    qualityDividerSpinBox->setMaximum(1000);
    connect(qualityDividerSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetQualityDivider(int)));
    durtiestLayout->addWidget(qualityDividerSpinBox);

    actionLayout->addLayout(durtiestLayout);

    QPushButton * showButton = new QPushButton ("Show", sceneTabs);
    actionLayout->addWidget (showButton);
    connect (showButton, SIGNAL (clicked ()), controller, SLOT (windowShowRayImage ()));

    QPushButton * saveButton  = new QPushButton ("Save", sceneTabs);
    connect (saveButton, SIGNAL (clicked ()) , controller, SLOT (windowExportRayImage ()));
    actionLayout->addWidget (saveButton);

    layout->addWidget(ActionGroupBox, 2, 0, 2, 1);


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

    layout->addWidget (globalGroupBox, 4, 0, 2, 1);
}
