// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <QProgressDialog>

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include "Brdf.h"
#include "Noise.h"
#include "Model.h"

using namespace std;

static RayTracer * instance = NULL;

RayTracer * RayTracer::getInstance () {
    if (instance == NULL)
        instance = new RayTracer ();
    return instance;
}

void RayTracer::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

/** Prepare a list of anti aliasing offsets for both i and j */
vector<pair<float, float> > prepareAntiAliasingOffsets()
{
	// Each ray picking is translated to fill the pixel which bottom left coordinate is i,j
	Model *model = Model::getInstance();
	unsigned int rays = model->getAntiAliasingRaysPerPixel();
	AntiAliasingType type = model->getAntiAliasingType();
	vector<pair<float, float>> offsets;

	switch (type)
	{
		case NO_ANTIALIASING:
			// One ray on the bottom left of the pixel
			offsets.push_back(make_pair(0.0, 0.0));
			break;

		case UNIFORM:
			{
				// Fill as well as possible the space (optimal if rays is a square)
				// Chosen algorithm:
				// - cut the pixel in ceil(sqrt(rays))^2 cells
				// - pick each center until rays is reached
				unsigned int raysSqrt = ceil(sqrt(rays));
				float cutting = 1.0/(float)(2*raysSqrt);
				unsigned int count = 0;
				for (unsigned int i=0; i<raysSqrt && count<rays; i++) {
					for (unsigned int j=0; j<raysSqrt && count<rays; j++) {
						offsets.push_back(make_pair(float(2*i+1)*cutting, float(2*j+1)*cutting));
						count++;
					}
				}
			}
			break;

		case POLYGONAL:
			{
				// Turn around a circle
				float angleStep = 2.0*M_PI/float(rays);
				float angle = 0.0;
				for (unsigned int i=0; i<rays; i++) {
					float cosAngle = (cos(angle) + 1.0) / 2.0;
					float sinAngle = (sin(angle) + 1.0) / 2.0;
					offsets.push_back(make_pair(cosAngle, sinAngle));
					angle += angleStep;
				}
			}
			break;

		case STOCHASTIC:
			{
				// Picked using randomness
				srand(time(NULL));
				for (unsigned int i=0; i<rays; i++) {
					float di = (float)rand() / (float)RAND_MAX;
					float dj = (float)rand() / (float)RAND_MAX;
					offsets.push_back(make_pair(di, dj));
				}
			}
			break;
	}

	return offsets;
}

inline int clamp (float f, int inf, int sup) {
    int v = static_cast<int> (f);
    return (v < inf ? inf : (v > sup ? sup : v));
}

// POINT D'ENTREE DU PROJET.
// Le code suivant ray trace uniquement la boite englobante de la scene.
// Il faut remplacer ce code par une veritable raytracer
QImage RayTracer::render (const Vec3Df & camPos,
                          const Vec3Df & direction,
                          const Vec3Df & upVector,
                          const Vec3Df & rightVector,
                          float fieldOfView,
                          float aspectRatio,
                          unsigned int screenWidth,
                          unsigned int screenHeight) {
    QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
    Scene * scene = Scene::getInstance ();
    vector<Vec3Df> posLight;
    for(const auto &light : scene->getLights())
        posLight.push_back(light.getPos());
    Brdf brdf(posLight, Vec3Df(0,0,0), Vec3Df(1.0,1.0,1.0), Vec3Df(0.5,0.5,0.0), 1.0, 1.0, 0.1, 1.5);
    QProgressDialog progressDialog ("Raytracing...", "Cancel", 0, 100);
    progressDialog.show ();

	vector<pair<float, float>> offsets = prepareAntiAliasingOffsets();

	// For each pixel
	for (unsigned int i = 0; i < screenWidth; i++) {
		progressDialog.setValue ((100*i)/screenWidth);
		for (unsigned int j = 0; j < screenHeight; j++) {

			Vec3Df c (backgroundColor);
			bool isColorInit(false);
      bool inter(false);
      Vec3Df dir;
      Vertex inter_nearest;

			// For each ray in each pixel
			for (pair<float, float> offset : offsets) {
				float tanX = tan (fieldOfView)*aspectRatio;
				float tanY = tan (fieldOfView);
				Vec3Df stepX = (float(i)+offset.first - screenWidth/2.f)/screenWidth * tanX * rightVector;
				Vec3Df stepY = (float(j)+offset.second - screenHeight/2.f)/screenHeight * tanY * upVector;
				Vec3Df step = stepX + stepY;
				dir = direction + step;
				dir.normalize ();

				float smallestIntersectionDistance = 1000000.f;
				Vec3Df addedColor(backgroundColor);

				// For each object
				for (Object & o : scene->getObjects()) {
					static const Perlin perlin(0.5f, 4, 10);
					brdf.colorDif = o.getMaterial().getColor();
					brdf.Kd = o.getMaterial().getDiffuse();
					brdf.Ks = o.getMaterial().getSpecular();
					Ray ray (camPos-o.getTrans (), dir);

					if (o.getKDtree().intersect(ray)) {
						float intersectionDistance = ray.getIntersectionDistance();
						const Vertex &intersection = ray.getIntersection();
						float noise = perlin(intersection.getPos());
						brdf.colorDif = noise*o.getMaterial().getColor();
						if(intersectionDistance < smallestIntersectionDistance) {
							addedColor = brdf.getColor(intersection.getPos(), intersection.getNormal(), camPos) * 255.0;
							smallestIntersectionDistance = intersectionDistance;
              inter_nearest = intersection;
              inter = true;
						}
					}
				}
				if (isColorInit) {
					c += addedColor;
				} else {
					c = addedColor;
					isColorInit = true;
				}

			}
			c /= offsets.size();

      if(inter) {
          for(Object & o : scene->getObjects()) {
              dir = posLight[0] - inter_nearest.getPos() - o.getTrans();
              dir.normalize();
              Ray ray_light(inter_nearest.getPos() - o.getTrans() + 0.000001*dir, dir);
              if(o.getKDtree().intersect(ray_light)) {
                  if(ray_light.getIntersectionDistance() > 0.0001) {
                      c = Vec3Df(0, 0, 0);
                      break;
                  }
              }
          }
      }


			image.setPixel (i, j, qRgb (clamp (c[0], 0, 255), clamp (c[1], 0, 255), clamp (c[2], 0, 255)));
		}
	}
    progressDialog.setValue (100);
    return image;
}
