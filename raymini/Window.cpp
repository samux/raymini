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

void Window::update(const Observable *observable) {
    updateLights(observable);
    updateObjects(observable);
    updateMaterials(observable);
    updateColorTextures(observable);
    updateMapping(observable);
    updateFocus(observable);
    updateRealTime(observable);
    updateProgressBar(observable);
    updateStatus(observable);
    updatePreview(observable);
    updateMotionBlur(observable);
    updateShadows(observable);
    updateAntiAliasing(observable);
    updatePathTracing(observable);
    updateBackgroundColor(observable);
}

void Window::updateShadows(const Observable *observable) {
    const RayTracer *rayTracer = controller->getRayTracer();
    if (observable == rayTracer && rayTracer->isChanged(RayTracer::SHADOW_CHANGED)) {
        shadowTypeList->setCurrentIndex(rayTracer->getShadowMode());
        shadowSpinBox->setVisible(rayTracer->getShadowMode() == Shadow::SOFT);
        shadowSpinBox->disconnect();
        shadowSpinBox->setValue(rayTracer->getShadowNbImpulse());
        connect(shadowSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetShadowNbRays(int)));
    }
}

void Window::updateAntiAliasing(const Observable *observable) {
    const RayTracer *rayTracer = controller->getRayTracer();
    if (observable != rayTracer) {
        return;
    }
    bool updateNbRayVisible = rayTracer->isChanged(RayTracer::DEPTH_PT_CHANGED) ||
        rayTracer->isChanged(RayTracer::TYPE_AA_CHANGED);
    if (updateNbRayVisible) {
        bool isPT = rayTracer->getDepthPathTracing() != 0;
        AANbRaySpinBox->setVisible(
                (!isPT) &&
                rayTracer->getTypeAntiAliasing() != AntiAliasing::NONE);
    }
    if (rayTracer->isChanged(RayTracer::NB_RAYS_AA_CHANGED)) {
        AANbRaySpinBox->disconnect();
        AANbRaySpinBox->setValue(rayTracer->getNbRayAntiAliasing());
        connect(AANbRaySpinBox, SIGNAL(valueChanged(int)),
                controller, SLOT(windowSetNbRayAntiAliasing(int)));
    }
}

void Window::updateAmbientOcclusion(const Observable *observable) {
    const RayTracer *rayTracer = controller->getRayTracer();
    if (observable != rayTracer) {
        return;
    }
    bool isAOchanged = rayTracer->isChanged(RayTracer::NB_RAYS_AO_CHANGED);
    if (isAOchanged) {
        bool isAO = rayTracer->getNbRayAmbientOcclusion() != 0;
        AONbRaysSpinBox->disconnect();
        AONbRaysSpinBox->setValue(rayTracer->getNbRayAmbientOcclusion());
        connect(AONbRaysSpinBox, SIGNAL(valueChanged(int)),
                controller, SLOT(windowSetAmbientOcclusionNbRays(int)));
        AORadiusSpinBox->setVisible(isAO);
        AOMaxAngleSpinBox->setVisible(isAO);
    }
    if (rayTracer->isChanged(RayTracer::RADIUS_AO_CHANGED)) {
        AORadiusSpinBox->disconnect();
        AORadiusSpinBox->setValue(rayTracer->getRadiusAmbientOcclusion());
        connect(AORadiusSpinBox, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetAmbientOcclusionRadius(double)));
    }
    if (rayTracer->isChanged(RayTracer::MAX_ANGLE_AO_CHANGED)) {
        AOMaxAngleSpinBox->disconnect();
        int AOAngle = rayTracer->getMaxAngleAmbientOcclusion()*360.0/(2.0*M_PI)+0.5;
        AOMaxAngleSpinBox->setValue(AOAngle);
        connect(AOMaxAngleSpinBox, SIGNAL(valueChanged(int)), controller, SLOT(windowSetAmbientOcclusionMaxAngle(int)));
    }
}

void Window::updatePathTracing(const Observable *observable) {
    const RayTracer *rayTracer = controller->getRayTracer();
    if (observable != rayTracer) {
        return;
    }
    if (rayTracer->isChanged(RayTracer::DEPTH_PT_CHANGED)) {
        bool isPT = rayTracer->getDepthPathTracing() != 0;
        PTDepthSpinBox->disconnect();
        PTDepthSpinBox->setValue(rayTracer->getDepthPathTracing());
        connect(PTDepthSpinBox, SIGNAL(valueChanged(int)),
                controller, SLOT(windowSetDepthPathTracing(int)));
        PTNbRaySpinBox->setVisible(isPT);
        PTOnlyCheckBox->setVisible(isPT);
        PBGICheckBox->setVisible(!isPT);
    }
    if (rayTracer->isChanged(RayTracer::NB_RAYS_PT_CHANGED)) {
        PTNbRaySpinBox->disconnect();
        PTNbRaySpinBox->setValue(rayTracer->getNbRayPathTracing());
        connect(PTNbRaySpinBox, SIGNAL(valueChanged(int)),
                controller, SLOT(windowSetNbRayPathTracing(int)));
    }
    if (rayTracer->isChanged(RayTracer::INTENSITY_AO_CHANGED)) {
        PTIntensitySpinBox->disconnect();
        PTIntensitySpinBox->setValue(rayTracer->getIntensityPathTracing());
        connect(PTIntensitySpinBox, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetIntensityPathTracing(double)));
    }
}

void Window::updateBackgroundColor(const Observable *observable) {
    const RayTracer *rayTracer = controller->getRayTracer();
    if (observable != rayTracer) {
        return;
    }
    if (rayTracer->isChanged(RayTracer::BACKGROUND_CHANGED)) {
        bgColorButton->setIcon(createIconFromColor(rayTracer->getBackgroundColor()));
    }
}

QIcon Window::createIconFromColor(Vec3Df color) {
    QPixmap image(16, 16);
    QPainter p(&image);
    QColor BColor(color[0]*255, color[1]*255, color[2]*255);
    p.fillRect(0, 0, 16, 16, BColor);
    return QIcon(image);
}

void Window::updatePreview(const Observable *observable) {
    const WindowModel *windowModel = controller->getWindowModel();
    if (observable != windowModel) {
        return;
    }
    if (windowModel->isChanged(WindowModel::WIREFRAME_CHANGED)) {
        wireframeCheckBox->setChecked(windowModel->isWireframe());
    }
    if (windowModel->isChanged(WindowModel::RENDERING_MODE_CHANGED)) {
        modeList->setCurrentIndex(windowModel->getRenderingMode());
    }
    if (windowModel->isChanged(WindowModel::SHOW_SURFELS_CHANGED)) {
        surfelsCheckBox->setChecked(windowModel->isShowSurfel());
    }
    if (windowModel->isChanged(WindowModel::SHOW_KDTREE_CHANGED)) {
        kdtreeCheckBox->setChecked(windowModel->isShowKDTree());
    }
}

void Window::updateLights(const Observable *observable) {
    const Scene *scene = controller->getScene();
    const WindowModel *windowModel = controller->getWindowModel();

    int lightIndex = windowModel->getSelectedLightIndex();
    bool isLightSelected = lightIndex != -1;
    bool selectedLightChanged = observable == windowModel &&
            windowModel->isChanged(WindowModel::SELECTED_LIGHT_CHANGED);
    if (selectedLightChanged) {
        lightsList->setCurrentIndex(lightIndex+1);
        lightEnableCheckBox->setVisible(isLightSelected);
        bool isLightEnabled = isLightSelected && scene->getLights()[lightIndex]->isEnabled();
        for (int i=0; i<3; i++) {
            lightPosSpinBoxes[i]->setVisible(isLightEnabled);
        }
        lightColorButton->setVisible(isLightEnabled);
        lightRadiusSpinBox->setVisible(isLightEnabled);
        lightIntensitySpinBox->setVisible(isLightEnabled);
    }

    bool sceneUpdated = (observable != scene) ||
            (!scene->isChanged(Scene::LIGHT_CHANGED));

    if (isLightSelected && (selectedLightChanged || sceneUpdated)) {
        const Light * l = scene->getLights()[lightIndex];
        bool isLightEnabled = l->isEnabled();
        lightEnableCheckBox->setChecked(isLightEnabled);
        Vec3Df color = l->getColor();
        Vec3Df pos = l->getPos();
        float intensity = l->getIntensity();
        float radius = l->getRadius();
        for (int i=0; i<3; i++) {
            lightPosSpinBoxes[i]->disconnect();
            lightPosSpinBoxes[i]->setValue(pos[i]);
            connect(lightPosSpinBoxes[i], SIGNAL(valueChanged(double)),
                    controller, SLOT(windowSetLightPos()));
        }
        lightColorButton->setIcon(createIconFromColor(color));
        lightIntensitySpinBox->disconnect();
        lightIntensitySpinBox->setValue(intensity);
        connect(lightIntensitySpinBox, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetLightIntensity(double)));
        lightRadiusSpinBox->disconnect();
        lightRadiusSpinBox->setValue(radius);
        connect(lightRadiusSpinBox, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetLightRadius(double)));
    }
}

void Window::updateFocus(const Observable *observable) {
    const WindowModel *windowModel = controller->getWindowModel();
    if (observable == windowModel &&
            windowModel->isChanged(WindowModel::FOCUS_MODE_CHANGED)) {
        bool isFocusMode = windowModel->isFocusMode();
        changeFocusFixingCheckBox->setChecked(!isFocusMode);
    }
    const RayTracer *rayTracer = controller->getRayTracer();
    if (observable != rayTracer) {
        return;
    }
    if (rayTracer->isChanged(RayTracer::TYPE_FOCUS_CHANGED)) {
        Focus::Type type = rayTracer->getTypeFocus();
        focusTypeComboBox->setCurrentIndex(type);
        bool isFocus = type != Focus::NONE;
        changeFocusFixingCheckBox->setVisible(isFocus);
        focusNbRaysSpinBox->setVisible(isFocus);
        focusApertureSpinBox->setVisible(isFocus);
    }
    if (rayTracer->isChanged(RayTracer::NB_RAYS_FOCUS_CHANGED)) {
        focusNbRaysSpinBox->disconnect();
        focusNbRaysSpinBox->setValue(rayTracer->getNbRayFocus());
        connect(focusNbRaysSpinBox, SIGNAL(valueChanged(int)),
                controller, SLOT(windowSetFocusNbRays(int)));
    }
    if (rayTracer->isChanged(RayTracer::APERTURE_FOCUS_CHANGED)) {
        focusApertureSpinBox->disconnect();
        focusApertureSpinBox->setValue(rayTracer->getApertureFocus());
        connect(focusApertureSpinBox, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetFocusAperture(double)));
    }
}

void Window::updateObjects(const Observable *observable) {
    const Scene *scene = controller->getScene();
    const WindowModel *windowModel = controller->getWindowModel();

    int index = windowModel->getSelectedObjectIndex();
    objectsList->setCurrentIndex(index+1);
    bool isSelected = index != -1;
    bool selectedObjectChanged = observable == windowModel &&
            windowModel->isChanged(WindowModel::SELECTED_OBJECT_CHANGED);
    if (selectedObjectChanged) {
        objectEnableCheckBox->setVisible(isSelected);
        objectMobileLabel->setVisible(isSelected);
        for (unsigned int i=0; i<3; i++) {
            objectPosSpinBoxes[i]->setVisible(isSelected);
            objectMobileSpinBoxes[i]->setVisible(isSelected);
        }
        objectMaterialLabel->setVisible(isSelected);
        objectMaterialsList->setVisible(isSelected);
        int materialIndex = scene->getObjectMaterialIndex(index);
        if (materialIndex != -1) {
            objectMaterialsList->setCurrentIndex(materialIndex);
        }
    }
    bool sceneChanged = observable == scene &&
            scene->isChanged(Scene::OBJECT_CHANGED);
    if (isSelected && (selectedObjectChanged || sceneChanged)) {
        const Object *object = scene->getObjects()[index];
        bool isEnabled = object->isEnabled();
        objectEnableCheckBox->setChecked(isEnabled);
        for (unsigned int i=0; i<3; i++) {
            objectPosSpinBoxes[i]->disconnect();
            objectPosSpinBoxes[i]->setValue(object->getTrans()[i]);
            connect(objectPosSpinBoxes[i], SIGNAL(valueChanged(double)),
                    controller, SLOT(windowSetObjectPos()));
            objectMobileSpinBoxes[i]->disconnect();
            objectMobileSpinBoxes[i]->setValue(object->getMobile()[i]);
            connect(objectMobileSpinBoxes[i], SIGNAL(valueChanged(double)),
                    controller, SLOT(windowSetObjectMobile()));
        }
    }
}

void Window::updateMaterials(const Observable *observable) {
    const Scene *scene = controller->getScene();
    const WindowModel *windowModel = controller->getWindowModel();

    int index = windowModel->getSelectedMaterialIndex();
    bool isSelected = index != -1;
    bool selectedMaterialChanged = observable == windowModel &&
            windowModel->isChanged(WindowModel::SELECTED_MATERIAL_CHANGED);
    bool isMaterialGlass = false;
    bool isMaterialSkyBox = false;
    if (isSelected) {
        const Material *material = scene->getMaterials()[index];
        isMaterialGlass = dynamic_cast<const Glass*>(material);
        isMaterialSkyBox = dynamic_cast<const SkyBoxMaterial*>(material);
    }

    bool sceneChanged = observable == scene &&
            scene->isChanged(Scene::MATERIAL_CHANGED);

    if (isSelected && (sceneChanged || selectedMaterialChanged)) {
        const Material *material = scene->getMaterials()[index];
        materialDiffuseSpinBox->disconnect();
        materialDiffuseSpinBox->setValue(material->getDiffuse());
        connect(materialDiffuseSpinBox, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetMaterialDiffuse(double)));
        materialSpecularSpinBox->disconnect();
        materialSpecularSpinBox->setValue(material->getSpecular());
        connect(materialSpecularSpinBox, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetMaterialSpecular(double)));
        materialGlossyRatio->disconnect();
        materialGlossyRatio->setValue(material->getGlossyRatio());
        connect(materialGlossyRatio, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetMaterialGlossyRatio(double)));
        if (isMaterialGlass) {
            const Glass *glass = dynamic_cast<const Glass*>(material);
            glassAlphaSpinBox->disconnect();
            glassAlphaSpinBox->setValue(glass->getAlpha());
            connect(glassAlphaSpinBox, SIGNAL(valueChanged(double)),
                    controller, SLOT(windowSetMaterialGlassAlpha(double)));
        }
    }

    if (selectedMaterialChanged) {
        materialsList->setCurrentIndex(index+1);
        bool dsgVisible = isSelected && !isMaterialSkyBox;
        materialDiffuseSpinBox->setVisible(dsgVisible);
        materialSpecularSpinBox->setVisible(dsgVisible);
        materialGlossyRatio->setVisible(dsgVisible);
        materialColorTextureLabel->setVisible(isSelected);
        materialColorTexturesList->setVisible(isSelected);
        materialNormalTextureLabel->setVisible(isSelected);
        materialNormalTexturesList->setVisible(isSelected);
        int colorTextureIndex = scene->getMaterialColorTextureIndex(index);
        if (colorTextureIndex != -1) {
            materialColorTexturesList->setCurrentIndex(colorTextureIndex);
        }
        int normalTextureIndex = scene->getMaterialNormalTextureIndex(index);
        if (normalTextureIndex != -1) {
            materialNormalTexturesList->setCurrentIndex(normalTextureIndex);
        }
        glassAlphaSpinBox->setVisible(isMaterialGlass);
    }
}

void Window::updateColorTextures(const Observable *observable) {
    const WindowModel *windowModel = controller->getWindowModel();

    int index = windowModel->getSelectedTextureIndex();
    bool isSelected = index != -1;
    bool selectedTextureChanged = observable == windowModel &&
            windowModel->isChanged(WindowModel::SELECTED_TEXTURE_CHANGED);
    if (selectedTextureChanged) {
        colorTexturesList->setCurrentIndex(index+1);
        colorTextureColorButton->setVisible(isSelected);
    }

    const Scene *scene = controller->getScene();
    bool sceneChanged = observable == scene &&
            scene->isChanged(Scene::COLOR_TEXTURE_CHANGED);
    if (isSelected && (sceneChanged || selectedTextureChanged)) {
        const ColorTexture *texture = scene->getColorTextures()[index];
        colorTextureColorButton->setIcon(createIconFromColor(texture->getRepresentativeColor()));
    }
}

void Window::updateProgressBar(const Observable *observable) {
    const RenderThread *renderThread = controller->getRenderThread();
    const WindowModel *windowModel = controller->getWindowModel();
    bool renderThreadUpdated =
        observable == renderThread &&
        renderThread->isChanged(RenderThread::RENDER_CHANGED);
    bool windowModelUpdated =
        observable == windowModel &&
        windowModel->isChanged(WindowModel::REAL_TIME_CHANGED);
    bool isRendering = renderThread->isRendering();
    if (renderThreadUpdated || windowModelUpdated) {
        bool isRealTime = windowModel->isRealTime();
        stopRenderButton->setVisible(isRendering||isRealTime);
        renderButton->setVisible(!isRendering && !isRealTime);
    }
    if (renderThreadUpdated) {
        if (isRendering) {
            float percent = renderThread->getPercent();
            renderProgressBar->setValue(percent);
        }
        else {
            renderProgressBar->setValue(100);
        }
    }
}

void Window::updateRealTime(const Observable *observable) {
    const WindowModel *windowModel = controller->getWindowModel();
    if (observable == windowModel) {
        if (windowModel->isChanged(WindowModel::REAL_TIME_CHANGED)) {
            bool isRealTime = windowModel->isRealTime();
            realTimeCheckBox->setChecked(isRealTime);
            dragCheckBox->setVisible(isRealTime);
            durtiestQualityComboBox->setVisible(isRealTime);
            durtiestQualityLabel->setVisible(isRealTime);
        }
        if (windowModel->isChanged(WindowModel::DRAG_ENABLED_CHANGED)) {
            dragCheckBox->setChecked(windowModel->isDragEnabled());
        }
    }
    const RayTracer *rayTracer = controller->getRayTracer();
    if (observable == rayTracer) {
        if (rayTracer->isChanged(RayTracer::DURTIEST_QUALITY_CHANGED)) {
            int quality = rayTracer->getDurtiestQuality();
            durtiestQualityComboBox->setCurrentIndex(quality);
            qualityDividerSpinBox->setVisible(quality == RayTracer::Quality::ONE_OVER_X);
        }
        if (rayTracer->isChanged(RayTracer::DURTIEST_QUALITY_DIVIDER_CHANGED)) {
            qualityDividerSpinBox->disconnect();
            qualityDividerSpinBox->setValue(rayTracer->getDurtiestQualityDivider());
            connect(qualityDividerSpinBox, SIGNAL(valueChanged(int)),
                    controller, SLOT(windowSetQualityDivider(int)));
        }
    }
}

void Window::updateStatus(const Observable *observable) {
    const WindowModel *windowModel = controller->getWindowModel();
    bool windowModelUpdated =
        windowModel == observable &&
        windowModel->isChanged(WindowModel::ELAPSED_TIME_CHANGED);

    const RayTracer *rayTracer = controller->getRayTracer();
    bool rayTracerUpdated =
        rayTracer == observable &&
        rayTracer->isChanged(RayTracer::QUALITY_CHANGED | RayTracer::QUALITY_DIVIDER_CHANGED);

    const RenderThread *renderThread = controller->getRenderThread();
    bool renderThreadUpdated =
        observable == renderThread &&
        renderThread->isChanged(RenderThread::RENDER_CHANGED);

    if (!windowModelUpdated && !rayTracerUpdated && !renderThreadUpdated) {
        return;
    }

    int elapsed = windowModel->getElapsedTime();
    qglviewer::Camera * cam = controller->getViewer()->camera ();
    unsigned int screenWidth = cam->screenWidth ();
    unsigned int screenHeight = cam->screenHeight ();
    RayTracer::Quality quality = rayTracer->getQuality();
    int divider = rayTracer->getQualityDivider();
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

void Window::updateMotionBlur(const Observable *observable) {
    const Scene *scene = controller->getScene();
    if (observable == scene &&
            scene->isChanged(Scene::OBJECT_CHANGED)) {
        bool isMobile = scene->hasMobile();
        int widgetIndex = -1;
        for (int i=0; i<rayTabs->count(); i++) {
            if (rayTabs->widget(i) == mBlurGroupBox) {
                widgetIndex = i;
                break;
            }
        }
        //rayTabs->setTabEnabled(widgetIndex, isMobile);
        if (isMobile && widgetIndex == -1) {
            rayTabs->addTab(mBlurGroupBox, "Motion Blur");
        }
        if (!isMobile && widgetIndex != -1) {
            rayTabs->removeTab(widgetIndex);
        }
        mBlurNbImagesSpinBox->setVisible(isMobile);
    }

    const RayTracer *rayTracer = controller->getRayTracer();
    if (observable == rayTracer &&
            rayTracer->isChanged(RayTracer::NB_PICTURES_CHANGED)) {
        mBlurNbImagesSpinBox->disconnect();
        mBlurNbImagesSpinBox->setValue(rayTracer->getNbPictures());
        connect(mBlurNbImagesSpinBox, SIGNAL (valueChanged(int)),
                controller, SLOT (windowSetNbImagesSpinBox (int)));
    }
}

void Window::updateMapping(const Observable *observable) {
    const WindowModel *windowModel = controller->getWindowModel();
    int index = windowModel->getSelectedObjectIndex();
    bool isSelected = index != -1;

    bool selectedObjectChanged = observable == windowModel &&
            windowModel->isChanged(WindowModel::SELECTED_OBJECT_CHANGED);

    if (selectedObjectChanged) {
        mappingObjectsList->setCurrentIndex(index+1);
        mappingUScale->setVisible(isSelected);
        mappingVScale->setVisible(isSelected);
        mappingSphericalPushButton->setVisible(isSelected);
        mappingSquarePushButton->setVisible(isSelected);
        mappingCubePushButton->setVisible(isSelected);
    }

    const Scene *scene = controller->getScene();
    bool sceneChanged = observable == scene &&
            scene->isChanged(Scene::OBJECT_CHANGED);
    if (isSelected && (selectedObjectChanged || sceneChanged)) {
        const Mesh &mesh = scene->getObjects()[index]->getMesh();
        mappingUScale->disconnect();
        mappingUScale->setValue(mesh.getUScale());
        connect(mappingUScale, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetUScale(double)));
        mappingVScale->disconnect();
        mappingVScale->setValue(mesh.getVScale());
        connect(mappingVScale, SIGNAL(valueChanged(double)),
                controller, SLOT(windowSetVScale(double)));
    }
}

void Window::initControlWidget() {
    const Scene *scene = controller->getScene();

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

    QHBoxLayout *surfelsLayout = new QHBoxLayout;

    surfelsCheckBox = new QCheckBox("Show surfels", previewGroupBox);
    connect(surfelsCheckBox, SIGNAL(clicked(bool)), controller, SLOT(viewerSetShowSurfel(bool)));
    surfelsLayout->addWidget(surfelsCheckBox);

    QPushButton *surfelUpdateButton = new QPushButton("Update point cloud", previewGroupBox);
    connect(surfelUpdateButton, SIGNAL(clicked()), controller, SLOT(windowUpdatePBGI()));
    surfelsLayout->addWidget(surfelUpdateButton);

    previewLayout->addLayout(surfelsLayout);

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
    rayTabs = new QTabWidget(rayGroupBox);
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
    AANbRaySpinBox->setMaximum(10000);
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
    PTNbRaySpinBox->setMaximum (10000);
    connect(PTNbRaySpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetNbRayPathTracing (int)));
    PTLayout->addWidget (PTNbRaySpinBox);

    PTIntensitySpinBox = new QDoubleSpinBox(PTGroupBox);
    PTIntensitySpinBox->setPrefix ("Intensity: ");
    PTIntensitySpinBox->setMinimum (0.2);
    PTIntensitySpinBox->setMaximum (100.0);
    PTIntensitySpinBox->setSingleStep(0.2);
    connect(PTIntensitySpinBox, SIGNAL(valueChanged(double)), controller, SLOT(windowSetIntensityPathTracing(double)));
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
    QVBoxLayout * mBlurLayout = new QVBoxLayout(mBlurGroupBox);

    mBlurNbImagesSpinBox = new QSpinBox(PTGroupBox);
    mBlurNbImagesSpinBox->setSuffix (" images");
    mBlurNbImagesSpinBox->setMinimum (1);
    mBlurNbImagesSpinBox->setMaximum (100);
    mBlurNbImagesSpinBox->setVisible(false);
    connect (mBlurNbImagesSpinBox, SIGNAL (valueChanged(int)), controller, SLOT (windowSetNbImagesSpinBox (int)));
    mBlurLayout->addWidget (mBlurNbImagesSpinBox);

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

    QHBoxLayout *objectMobileLayout = new QHBoxLayout;
    objectMobileLabel = new QLabel("Movement:", objectsGroupBox);
    objectMobileLayout->addWidget(objectMobileLabel);
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


    //  SceneGroup: Mapping
    QWidget *mappingGroupBox = new QWidget(sceneTabs);
    QVBoxLayout *mappingLayout = new QVBoxLayout(mappingGroupBox);

    QHBoxLayout *mappingObjectLayout = new QHBoxLayout;

    QLabel *mappingLabel = new QLabel("Select an object:", mappingGroupBox);
    mappingObjectLayout->addWidget(mappingLabel);

    mappingObjectsList = new QComboBox(mappingGroupBox);
    mappingObjectsList->addItem("No object selected");
    for (const Object * o : scene->getObjects()) {
        QString name = QString::fromStdString(o->getName());
        mappingObjectsList->addItem(name);
    }
    connect(mappingObjectsList, SIGNAL(activated(int)), controller, SLOT(windowSelectObject(int)));
    mappingObjectLayout->addWidget(mappingObjectsList);

    mappingLayout->addLayout(mappingObjectLayout);

    QHBoxLayout *mappingScaleLayout = new QHBoxLayout;

    mappingUScale = new QDoubleSpinBox(mappingGroupBox);
    mappingUScale->setMinimum(0.01);
    mappingUScale->setMaximum(10000);
    mappingUScale->setSingleStep(1);
    mappingUScale->setPrefix("U scale:");
    connect(mappingUScale, SIGNAL(valueChanged(double)), controller, SLOT(windowSetUScale(double)));
    mappingScaleLayout->addWidget(mappingUScale);

    mappingVScale = new QDoubleSpinBox(mappingGroupBox);
    mappingVScale->setMinimum(0.01);
    mappingVScale->setMaximum(10000);
    mappingVScale->setSingleStep(1);
    mappingVScale->setPrefix("U scale:");
    connect(mappingVScale, SIGNAL(valueChanged(double)), controller, SLOT(windowSetVScale(double)));
    mappingScaleLayout->addWidget(mappingVScale);

    mappingLayout->addLayout(mappingScaleLayout);

    mappingSphericalPushButton = new QPushButton("Sperical mapping", mappingGroupBox);
    connect(mappingSphericalPushButton, SIGNAL(clicked()),
            controller, SLOT(windowSetSphericalMapping()));
    mappingLayout->addWidget(mappingSphericalPushButton);

    mappingSquarePushButton = new QPushButton("Square mapping", mappingGroupBox);
    connect(mappingSquarePushButton, SIGNAL(clicked()),
            controller, SLOT(windowSetSquareMapping()));
    mappingLayout->addWidget(mappingSquarePushButton);

    mappingCubePushButton = new QPushButton("Cube mapping", mappingGroupBox);
    connect(mappingCubePushButton, SIGNAL(clicked()),
            controller, SLOT(windowSetCubicMapping()));
    mappingLayout->addWidget(mappingCubePushButton);

    sceneTabs->addTab(mappingGroupBox, "Texture mapping");


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
    connect(materialGlossyRatio, SIGNAL(valueChanged(double)),
            controller, SLOT(windowSetMaterialGlossyRatio(double)));
    materialsLayout->addWidget(materialGlossyRatio);

    QHBoxLayout *materialColorTexturesLayout = new QHBoxLayout;

    materialColorTextureLabel = new QLabel("Color texture:", materialsGroupBox);
    materialColorTexturesLayout->addWidget(materialColorTextureLabel);

    materialColorTexturesList = new QComboBox(materialsGroupBox);
    for (const ColorTexture * t : scene->getColorTextures()) {
        materialColorTexturesList->addItem(t->getName().c_str());
    }
    connect(materialColorTexturesList, SIGNAL(activated(int)),
            controller, SLOT(windowSetMaterialColorTexture(int)));
    materialColorTexturesLayout->addWidget(materialColorTexturesList);

    materialsLayout->addLayout(materialColorTexturesLayout);

    QHBoxLayout *materialNormalTexturesLayout = new QHBoxLayout;

    materialNormalTextureLabel = new QLabel("Normal texture:", materialsGroupBox);
    materialNormalTexturesLayout->addWidget(materialNormalTextureLabel);

    materialNormalTexturesList = new QComboBox(materialsGroupBox);
    for (const NormalTexture * t : scene->getNormalTextures()) {
        materialNormalTexturesList->addItem(t->getName().c_str());
    }
    connect(materialNormalTexturesList, SIGNAL(activated(int)),
            controller, SLOT(windowSetMaterialNormalTexture(int)));
    materialNormalTexturesLayout->addWidget(materialNormalTexturesList);

    materialsLayout->addLayout(materialNormalTexturesLayout);

    glassAlphaSpinBox = new QDoubleSpinBox(materialsGroupBox);
    glassAlphaSpinBox->setMinimum(0);
    glassAlphaSpinBox->setMaximum(1);
    glassAlphaSpinBox->setPrefix("Alpha: ");
    connect(glassAlphaSpinBox, SIGNAL(valueChanged(double)),
            controller, SLOT(windowSetMaterialGlassAlpha(double)));
    materialsLayout->addWidget(glassAlphaSpinBox);

    sceneTabs->addTab(materialsGroupBox, "Materials");

    // SceneGroup: Textures

    QWidget *colorTexturesGroupBox = new QWidget(sceneTabs);
    QVBoxLayout *colorTexturesLayout = new QVBoxLayout(colorTexturesGroupBox);

    colorTexturesList = new QComboBox(colorTexturesGroupBox);
    colorTexturesList->addItem("No color texture selected");
    for (const ColorTexture *t : scene->getColorTextures()) {
        colorTexturesList->addItem(t->getName().c_str());
    }
    connect(colorTexturesList, SIGNAL(activated(int)),
            controller, SLOT(windowSelectTexture(int)));
    colorTexturesLayout->addWidget(colorTexturesList);

    colorTextureColorButton = new QPushButton("Base color", colorTexturesGroupBox);
    connect(colorTextureColorButton, SIGNAL(clicked()),
            controller, SLOT(windowSetColorTextureColor()));
    colorTexturesLayout->addWidget(colorTextureColorButton);

    sceneTabs->addTab(colorTexturesGroupBox, "Textures");

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
    QString axis[3] = {"X: ", "Y: ", "Z: "};
    for (int i=0; i<3; i++) {
        lightPosSpinBoxes[i] = new QDoubleSpinBox(lightsGroupBox);
        lightPosSpinBoxes[i]->setSingleStep(0.1);
        lightPosSpinBoxes[i]->setMinimum(-100);
        lightPosSpinBoxes[i]->setMaximum(100);
        lightPosSpinBoxes[i]->setPrefix(axis[i]);
        connect(lightPosSpinBoxes[i], SIGNAL(valueChanged(double)), controller, SLOT(windowSetLightPos()));
        lightsPosLayout->addWidget(lightPosSpinBoxes[i]);
    }
    lightsLayout->addLayout(lightsPosLayout);

    lightColorButton = new QPushButton("Color", lightsGroupBox);
    connect(lightColorButton, SIGNAL(clicked()), controller, SLOT(windowSetLightColor()));
    lightsLayout->addWidget(lightColorButton);

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

    dragCheckBox = new QCheckBox("Mouse moves objects", sceneTabs);
    connect(dragCheckBox, SIGNAL(clicked(bool)), controller, SLOT(windowSetDragEnabled(bool)));
    actionLayout->addWidget(dragCheckBox);

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

    bgColorButton = new QPushButton ("Background Color", globalGroupBox);
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
