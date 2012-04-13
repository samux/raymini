RayMini v0.2
Author: Tamy Boubekeur (tamy.boubekeur@telecom-paristech.fr)
RayMini is a minimal raytracer implemented in C++/OpenGL/Qt.
This software package is meants to be distributed to Telecom ParisTech student only.

Compiling
----------
Required library: GCC >v4, OpenGL, QT >v4.4, libQGLViewer and GLEW (for GPU extensions)
Edit the file raymini.pro to adapt it to your configuration (has been tested under Linux Ubuntu and Win7).

On linux: 
 qmake raymini.pro
 make
 ./raymini

