
#include "ball.h"


Ball::Ball() {
    velocity = Vector(0,0,0);
    radius = BALL_RADIUS;
    mass = BALL_MASS;
    vAngular = Vector(0,0,0);
    inHole = false;
    setMaterial(0.25,0,1.0);
	isCollision = false;
};

Ball::Ball(int color, int type) {
    velocity = Vector(0,0,0);
    radius = BALL_RADIUS;
    mass = BALL_MASS;
    vAngular = Vector(0,0,0);
    inHole = false;
	double SQRT_3 = sqrt(3.0);
	if (type == 0) {
		switch (color) {
		case SOLID_YELLOW:
			position = Point(-SQRT_3, 0.5, -2);
			setMaterial(1.0, 1.0, 0.0);
			break;
		case SOLID_BLUE:
			position = Point(-SQRT_3, 0.5, -1);
			setMaterial(0.0, 0.0, 1.0);
			break;
		case SOLID_RED:
			position = Point(-SQRT_3, 0.5, 0);
			setMaterial(1.0, 0, 0);
			break;
		case SOLID_PURPLE:
			position = Point(-SQRT_3, 0.5, 1);
			setMaterial(0.5, 0, 1.0);
			break;
		case SOLID_ORANGE:
			position = Point(-SQRT_3, 0.5, 2);
			setMaterial(1.0, 0.5, 0);
			break;
		case SOLID_GREEN:
			position = Point(-SQRT_3 * 0.5, 0.5, -1.5);
			setMaterial(0, 1, 0);
			break;
		case SOLID_BURGUNDY:
			position = Point(-SQRT_3 * 0.5, 0.5, -0.5);
			setMaterial(1.0, .4, .4);
			break;
		case STRIPED_YELLOW:
			position = Point(-SQRT_3 * 0.5, 0.5, 0.5);
			setMaterial(0.5, 0.5, 0);
			break;
		case STRIPED_BLUE:
			position = Point(-SQRT_3 * 0.5, 0.5, 1.5);
			setMaterial(0, 0, 0.5);
			break;
		case STRIPED_RED:
			position = Point(0, 0.5, -1);
			setMaterial(0.5, 0, 0);
			break;
		case STRIPED_PURPLE:
			position = Point(0, 0.5, 0);
			setMaterial(0.25, 0, 0.5);
			break;
		case STRIPED_ORANGE:
			position = Point(0, 0.5, 1);
			setMaterial(0.5, 0.25, 0);
			break;
		case STRIPED_GREEN:
			position = Point(SQRT_3*0.5, 0.5, -0.5);
			setMaterial(0, 0.5, 0);
			break;
		case STRIPED_BURGUNDY:
			position = Point(SQRT_3*0.5, 0.5, 0.5);
			setMaterial(0.5, 0.2, 0.2);
			break;
		case SOLID_BLACK:
			position = Point(SQRT_3, 0.5, 0);
			setMaterial(0, 0, 0);
			break;
		case SOLID_WHITE:
			position = Point(6, 0.5, 0);
			setMaterial(1, 1, 1);
			break;
		};
	}
	else if (type == 1){
		switch (color) {
		case SOLID_YELLOW:
			position = Point(-6.5, 0.5, 0);
			setMaterial(1.0, 1.0, 0.0);
			break;
		case SOLID_BLUE:
			position = Point(-8, 0.5, -4);
			setMaterial(0.0, 0.0, 1.0);
			break;
		case SOLID_RED:
			position = Point(-8, 0.5, -2.5);
			setMaterial(1.0, 0, 0);
			break;
		case SOLID_PURPLE:
			position = Point(-8, 0.5, 2.5);
			setMaterial(0.5, 0, 1.0);
			break;
		case SOLID_ORANGE:
			position = Point(-8, 0.5, 4);
			setMaterial(1.0, 0.5, 0);
			break;
		case SOLID_GREEN:
			position = Point(6.5, 0.5, 0);
			setMaterial(0, 1, 0);
			break;
		case SOLID_BURGUNDY:
			position = Point(8, 0.5, -4);
			setMaterial(1.0, .4, .4);
			break;
		case STRIPED_YELLOW:
			position = Point(8, 0.5, -2.5);
			setMaterial(0.5, 0.5, 0);
			break;
		case STRIPED_BLUE:
			position = Point(8,  0.5, 4);
			setMaterial(0, 0, 0.5);
			break;
		case STRIPED_RED:
			position = Point(8, 0.5, 2.5);
			setMaterial(0.5, 0, 0);
			break;
		};
	}
	prev_position = position;
	prev_inHole = false;
	isCollision = false;
};

Point Ball::getPosition() {
    return position;
};

void Ball::setMaterial(double r, double g, double b) {
    material.cAmbient.r = 0.7;
    material.cAmbient.g = 0.7;
    material.cAmbient.b = 0.4;
	material.cDiffuse.r = 0.5;
	material.cDiffuse.g = 0.5;
	material.cDiffuse.b = 0.5;
    material.cSpecular.r = material.cSpecular.g = material.cSpecular.b =1;
    material.cEmissive.r = material.cEmissive.g = material.cEmissive.b = 10;
	material.shininess = 128;

};

bool Ball::isInHole() {
    return inHole;
};

void Ball::fallIntoHole() {
    inHole = true;
};
void Ball::setAngle(double ang) {
	angle = ang;
}