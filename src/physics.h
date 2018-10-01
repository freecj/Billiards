/*
 * physics.h
 */
#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include "Algebra.h"

#define F_GRAVITY Vector(0, -9.81, 0)

inline Vector momentum(Vector velocity, double mass) {
    return velocity * mass;
};

inline double kinetic(Vector velocity, double mass) {
    return 0.5 * mass * dot(velocity,velocity);
};

#endif