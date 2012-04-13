// *********************************************************
// OpenGL Viewer Class, based on LibQGLViewer, compatible
// with most hardware (OpenGL 1.2).
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "GLViewer.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <string>

using namespace std;

static const GLuint OpenGLLightID[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};

GLViewer::GLViewer () : QGLViewer () {
    wireframe = false;
    renderingMode = Smooth;
}

GLViewer::~GLViewer () {
}

void GLViewer::setWireframe (bool b) {
    wireframe = b;
    if (wireframe) 
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    updateGL ();
}

void GLViewer::setRenderingMode (RenderingMode m) {
    renderingMode = m;
    updateGL ();
}

void GLViewer::setDisplayMode (DisplayMode m) {
    displayMode = m;
    updateGL ();
}

QString GLViewer::helpString() const {
  QString text("<h2>Raymini</h2>");
  text += "Author: <b>Tamy Boubekeur</b> (boubek@gmail.com)<br>Version: 0.1<br<br>";
  text += "<h3>Disclaimer</h3>";
  text += "This code is not bug-free, use it at your own risks.";
  text += "<h3>Controls</h3>";
  text += "Use the right control panel to setup rendering options.";
  text += "You can respectively zoom and translate with the left and middle mouse buttons. ";
  text += "Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
  text += "Simply press the function key again to restore it. Several keyFrames define a ";
  text += "camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
  text += "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
  text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
  text += "See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
  text += "Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
  text += "A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
  text += "A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
  text += "See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
  text += "Press <b>Escape</b> to exit the viewer.";
  return text;
}

void GLViewer::keyPressEvent (QKeyEvent * /*event*/) {
    
}

void GLViewer::keyReleaseEvent (QKeyEvent * /*event*/) {

}

void GLViewer::mousePressEvent (QMouseEvent * event) {
    setDisplayMode (OpenGLDisplayMode);
    QGLViewer::mousePressEvent(event);
}


void GLViewer::wheelEvent (QWheelEvent * e) {
    setDisplayMode (OpenGLDisplayMode);
    QGLViewer::wheelEvent (e);
}

// -----------------------------------------------
// Drawing functions
// -----------------------------------------------

void GLViewer::init() {
    glClearColor (0.f, 0.f, 0.f, 0.0);
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);
    glDepthFunc (GL_LEQUAL);
    glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable (GL_POINT_SMOOTH);

    Scene * scene = Scene::getInstance ();

    glLoadIdentity ();
    
    glEnable (GL_LIGHTING);
    for (unsigned int i = 0; i < scene->getLights ().size () && i < 8; i++) {
        GLuint glID = OpenGLLightID[i];
        glEnable (glID);
        const Light light = scene->getLights() [i];
        const Vec3Df & p = light.getPos ();
        float intensity = light.getIntensity ();
        const Vec3Df & c = intensity * light.getColor ();
        GLfloat glPos[4] = {p[0], p[1], p[2], 0};
        GLfloat glColor[4] = {c[0], c[1], c[2], 0};
        glLightfv (glID, GL_POSITION, glPos);
        glLightfv (glID, GL_DIFFUSE, glColor);
    }
    
    const BoundingBox & sceneBBox = scene->getBoundingBox ();
    Vec3Df c = sceneBBox.getCenter ();
    float r = sceneBBox.getRadius ();
    setSceneCenter (qglviewer::Vec (c[0], c[1], c[2]));
    setSceneRadius (r);
    showEntireScene ();
}

void GLViewer::draw () {
    if (displayMode == RayDisplayMode) {
        glDrawPixels (rayImage.width (),
                      rayImage.height (),
                      GL_RGB,
                      GL_UNSIGNED_BYTE,
                      rayImage.bits ());
        return;
    }
    Scene * scene = Scene::getInstance ();
    for (unsigned int i = 0; i < scene->getObjects ().size (); i++) {
        const Object & o = scene->getObjects ()[i];
        const Vec3Df & trans = o.getTrans ();
        glPushMatrix ();
        glTranslatef (trans[0], trans[1], trans[2]);
        const Material & mat = o.getMaterial ();
        const Vec3Df & color = mat.getColor ();
        float dif = mat.getDiffuse ();
        float spec = mat.getSpecular ();
        static GLfloat glMatDiff[4]; 
        static GLfloat glMatSpec[4];
        static const GLfloat glMatAmb[4] = {0.f, 0.f, 0.f, 1.f};
        for (unsigned int j = 0; j < 3; j++) {
            glMatDiff[j] = dif*color[j];
            glMatSpec[j] = spec;
        }
        glMatDiff[3] = 1.0f;
        glMatSpec[3] = 1.0f;

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glMatDiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glMatSpec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glMatAmb);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
        glDisable (GL_COLOR_MATERIAL);
        o.getMesh ().renderGL (renderingMode == Flat);
        glPopMatrix ();
    }
}

void GLViewer::setRayImage (const QImage & image) {
    rayImage = image;
}



