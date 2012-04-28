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

using namespace std;


Window::Window () : QMainWindow (NULL) {
    try {
        viewer = new GLViewer;
    } catch (GLViewer::Exception e) {
        cerr << e.getMessage () << endl;
        exit (1);
    }
    setCentralWidget (viewer);

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

void Window::setShadowMode(int i) {
    RayTracer * rayTracer = RayTracer::getInstance ();
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
    shadowSpinBox->setVisible(i == 2);
}

void Window::setShadowNbRays (int i) {
    RayTracer *rayTracer = RayTracer::getInstance();
    rayTracer->setShadowNbImpule(i);
}

void Window::renderRayImage () {
    qglviewer::Camera * cam = viewer->camera ();
    RayTracer * rayTracer = RayTracer::getInstance ();
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
    viewer->setDisplayMode (GLViewer::RayDisplayMode);
}

void Window::setBGColor () {
    RayTracer *rayTracer = RayTracer::getInstance();
    Vec3Df bg = 255*rayTracer->getBackgroundColor();
    QColor c = QColorDialog::getColor (QColor (bg[0], bg[1], bg[2]), this);
    if (c.isValid () == true) {
        RayTracer::getInstance ()->setBackgroundColor (Vec3Df (c.red ()/255.f, c.green ()/255.f, c.blue ()/255.f));
        viewer->setBackgroundColor (c);
        viewer->updateGL ();
    }
}

void Window::showRayImage () {
    viewer->setDisplayMode (GLViewer::RayDisplayMode);
}

void Window::exportGLImage () {
    viewer->saveSnapshot (false, false);
}

void Window::exportRayImage () {
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

void Window::about () {
    QMessageBox::about (this,
                        "About This Program",
                        "<b>RayMini</b> by: <br> <i>Tamy Boubekeur <br> Axel Schumacher <br> Bertrand Chazot <br> Samuel Mokrani</i>.");
}

void Window::changeAntiAliasingType(int index) {
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
    RayTracer::getInstance()->typeAntiAliasing = type;
    AANbRaySpinBox->setVisible(type != AntiAliasing::NONE);
}

void Window::setNbRayAntiAliasing(int i) {
    RayTracer::getInstance()->nbRayAntiAliasing = i;
}

void Window::changeAmbientOcclusionNbRays(int index) {
    RayTracer::getInstance()->nbRayAmbientOcclusion = index;
    AORadiusSpinBox->setVisible(index);
    AOMaxAngleSpinBox->setVisible(index);
}

void Window::setAmbientOcclusionMaxAngle(int i) {
    RayTracer::getInstance()->maxAngleAmbientOcclusion = (float)i*2.0*M_PI/360.0;
}

void Window::setAmbientOcclusionRadius(double f) {
    RayTracer::getInstance()->radiusAmbientOcclusion = f;
}

void Window::enableFocal(bool isFocal) {
    selecFocusedObject->setText("Choose focused point");
    selecFocusedObject->setVisible(isFocal);
    viewer->focusMode = isFocal;
    if(!isFocal)
        RayTracer::getInstance()->noFocus();
    viewer->updateGL();
}

void Window::setFocal() {
    if(viewer->focusMode) {
        viewer->focusMode = false;
        RayTracer::getInstance()->setFocus(viewer->getFocusPoint());
        selecFocusedObject->setText("Change focus point");
    }
    else {
        enableFocal(true);
    }
}

void Window::setDepthPathTracing(int i) {
    RayTracer::getInstance()->depthPathTracing = i;
    PTNbRaySpinBox->setVisible(i != 0);
    PTMaxAngleSpinBox->setVisible(i != 0);
}

void Window::setNbRayPathTracing(int i) {
    RayTracer::getInstance()->nbRayPathTracing = i;
}
void Window::setMaxAnglePathTracing(int i) {
    RayTracer::getInstance()->maxAnglePathTracing = (float)i*2.0*M_PI/360.0;
}

void Window::setNbImagesSpinBox(int i) {
    RayTracer::getInstance()->nbPictures = i;
}

void Window::initControlWidget () {
    // Control widget
    controlWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (controlWidget);

    // Preview
    QGroupBox * previewGroupBox = new QGroupBox ("Preview", controlWidget);
    QVBoxLayout * previewLayout = new QVBoxLayout (previewGroupBox);

    QCheckBox * wireframeCheckBox = new QCheckBox ("Wireframe", previewGroupBox);
    connect (wireframeCheckBox, SIGNAL (toggled (bool)), viewer, SLOT (setWireframe (bool)));
    previewLayout->addWidget (wireframeCheckBox);

    QComboBox *modeList = new QComboBox(previewGroupBox);
    modeList->addItem("Smooth");
    modeList->addItem("Flat");
    previewLayout->addWidget(modeList);
    connect (modeList, SIGNAL (activated (int)), viewer, SLOT (setRenderingMode (int)));

    QPushButton * snapshotButton  = new QPushButton ("Save preview", previewGroupBox);
    connect (snapshotButton, SIGNAL (clicked ()) , this, SLOT (exportGLImage ()));
    previewLayout->addWidget (snapshotButton);

    layout->addWidget (previewGroupBox);

    // Ray tracing
    QGroupBox * rayGroupBox = new QGroupBox ("Ray Tracing", controlWidget);
    QVBoxLayout * rayLayout = new QVBoxLayout (rayGroupBox);

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
    AANbRaySpinBox->setValue(RayTracer::getInstance()->nbRayAntiAliasing);
    connect(AANbRaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(setNbRayAntiAliasing(int)));
    AALayout->addWidget(AANbRaySpinBox);

    rayLayout->addWidget(AAGroupBox);

    //  Ambient occlusion
    QGroupBox * AOGroupBox = new QGroupBox ("Ambient Occlusion", rayGroupBox);
    QVBoxLayout * AOLayout = new QVBoxLayout (AOGroupBox);

    QSpinBox *AONbRaysSpinBox = new QSpinBox(AOGroupBox);
    AONbRaysSpinBox->setSuffix(" rays");
    AONbRaysSpinBox->setValue(RayTracer::getInstance()->nbRayAmbientOcclusion);
    AOLayout->addWidget(AONbRaysSpinBox);
    connect(AONbRaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeAmbientOcclusionNbRays(int)));

    AOMaxAngleSpinBox = new QSpinBox(AOGroupBox);
    AOMaxAngleSpinBox->setPrefix ("Max angle: ");
    AOMaxAngleSpinBox->setSuffix (" degrees");
    AOMaxAngleSpinBox->setMinimum (0);
    AOMaxAngleSpinBox->setMaximum (180);
    AOMaxAngleSpinBox->setVisible(false);
    AOMaxAngleSpinBox->setValue(RayTracer::getInstance()->maxAngleAmbientOcclusion*360.0/(2.0*M_PI));
    connect(AOMaxAngleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setAmbientOcclusionMaxAngle(int)));
    AOLayout->addWidget(AOMaxAngleSpinBox);

    AORadiusSpinBox = new QDoubleSpinBox(AOGroupBox);
    AORadiusSpinBox->setPrefix("Radius: ");
    AORadiusSpinBox->setMinimum(0);
    AORadiusSpinBox->setSingleStep(0.1);
    AORadiusSpinBox->setValue(RayTracer::getInstance()->radiusAmbientOcclusion);
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
    shadowSpinBox->setValue(RayTracer::getInstance()->getShadowMode());
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
    PTDepthSpinBox->setValue(RayTracer::getInstance()->depthPathTracing);
    connect (PTDepthSpinBox, SIGNAL (valueChanged(int)), this, SLOT (setDepthPathTracing (int)));
    PTLayout->addWidget (PTDepthSpinBox);

    PTNbRaySpinBox = new QSpinBox(PTGroupBox);
    PTNbRaySpinBox->setSuffix (" rays");
    PTNbRaySpinBox->setMinimum (1);
    PTNbRaySpinBox->setMaximum (1000);
    PTNbRaySpinBox->setVisible(false);
    PTNbRaySpinBox->setValue(RayTracer::getInstance()->nbRayPathTracing);
    connect (PTNbRaySpinBox, SIGNAL (valueChanged(int)), this, SLOT (setNbRayPathTracing (int)));
    PTLayout->addWidget (PTNbRaySpinBox);

    PTMaxAngleSpinBox = new QSpinBox(PTGroupBox);
    PTMaxAngleSpinBox->setPrefix ("Max angle: ");
    PTMaxAngleSpinBox->setSuffix (" degrees");
    PTMaxAngleSpinBox->setMinimum (0);
    PTMaxAngleSpinBox->setMaximum (180);
    PTMaxAngleSpinBox->setVisible(false);
    PTMaxAngleSpinBox->setValue(RayTracer::getInstance()->maxAnglePathTracing*360.0/(2.0*M_PI));
    connect (PTMaxAngleSpinBox, SIGNAL (valueChanged(int)), this, SLOT (setMaxAnglePathTracing (int)));
    PTLayout->addWidget (PTMaxAngleSpinBox);

    rayLayout->addWidget (PTGroupBox);

    //  Focal
    QGroupBox * focalGroupBox = new QGroupBox ("Focal", rayGroupBox);
    QVBoxLayout * focalLayout = new QVBoxLayout (focalGroupBox);

    QCheckBox * focalCheckBox = new QCheckBox ("Enable Focus", rayGroupBox);
    connect (focalCheckBox, SIGNAL (toggled (bool)), this, SLOT (enableFocal (bool)));
    focalLayout->addWidget (focalCheckBox);

    selecFocusedObject  = new QPushButton ("", rayGroupBox);
    selecFocusedObject->setVisible(false);
    connect (selecFocusedObject, SIGNAL (clicked ()) , this, SLOT ( setFocal()));
    focalLayout->addWidget (selecFocusedObject);

    rayLayout->addWidget (focalGroupBox);

    // Motion Blur
    if(Scene::getInstance()->hasMobile()) {
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
