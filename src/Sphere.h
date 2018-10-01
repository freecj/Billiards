#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include "tess.h"

class Sphere : public Shape {
public:
	void draw();
	void drawNormal();
    double Intersect(Point eyePointP, Vector rayV, Matrix transformMatrix);
    Vector findIsectNormal(Point eyePoint, Vector ray, double dist);
};

#endif
