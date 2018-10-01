
#include "pool.h"

PoolGame::PoolGame(int type) {

    //double SQRT_3 = sqrt(3.0);
	if (type == 0) {
		for (int i = 0; i < NUM_BALLS; ++i) {
			balls.push_back(Ball(i, 0));
		}
	} else if (type == 1) {
		for (int i = 0; i < 10; ++i) {
			balls.push_back(Ball(i, 1));
		}
	}
	m_type = type;
	winner = 0;
}

void PoolGame::reset(int type) {
	if (type == 0) {
		for (int i = 0; i < NUM_BALLS; ++i) {
			balls[i] = Ball(i, 0);
		}
	}
	else {
		for (int i = 0; i < 10; ++i) {
			balls[i] = Ball(i, 1);
		}
		winner = 0;
	}
  
}

void PoolGame::strikeBall(int i, Vector applied_force, double time) {
    Vector total_momentum = BALL_MASS * balls[i].velocity + time * applied_force;
    balls[i].velocity = total_momentum / BALL_MASS;
    balls[i].vAngular = cross(Vector(0,BALL_RADIUS,0),balls[i].velocity);
    updateMotion(time);
}

void PoolGame::updateMotion(double secs) {
    for (int i = 0; i < balls.size(); ++i) {
            if (balls[i].isInHole())
                continue;
			if ((balls[i].velocity).length() < 0.02) {
				balls[i].velocity = Vector(0, 0, 0);
				continue;
			}
				
			Point temp = balls[i].position;
            balls[i].position = balls[i].position + (balls[i].velocity * secs);
			balls[i].distance = (balls[i].position - temp).length();
            if (checkHole(i))
                continue;

            Vector friction = -balls[i].velocity;
            friction.normalize();

            friction = MU_K * friction;

            balls[i].velocity = balls[i].velocity + friction * secs;
			balls[i].vAngular = cross(Vector(0, BALL_RADIUS, 0), balls[i].velocity);
    }
    for (int i = 0; i < balls.size(); ++i) {
		if (checkHole(i))
			continue;
        checkCollision(i);
    }
}

bool PoolGame::checkCollision(int ball) {
    tableCollision(ball);
    for (int i = 0; i < balls.size(); ++i) {
        if (ball == i || balls[i].isInHole())
            continue;
        bool flag = ballCollision(ball,i);
		if (balls[ball].isCollision) {

		}
		else {
			balls[ball].isCollision = flag;
		}
		if (balls[i].isCollision) {

		}
		else {
			balls[i].isCollision = flag;
		}
	
    }
    return true;
}

bool PoolGame::ballCollision(int ball1, int ball2) {

    if (ball1 == ball2) return false;

    Point x1 = Point(balls[ball1].position);
    Point x2 = Point(balls[ball2].position);

    Vector v1 = Vector(balls[ball1].velocity);
    Vector v2 = Vector(balls[ball2].velocity);

    Vector d1 = x1 - x2;
    Vector d2 = x2 - x1;

    if (d1.length() <= 2.0 * BALL_RADIUS && dot(v1,d2) > 0) {
        
        balls[ball1].velocity = v1 - dot(v1-v2,d1) / SQR(d1.length()) * d1;
        balls[ball1].vAngular = cross(Vector(0,BALL_RADIUS,0),balls[ball1].velocity);

        balls[ball2].velocity = v2 - dot(v2-v1,d2) / SQR(d2.length()) * d2;
        balls[ball2].vAngular =  cross(Vector(0,BALL_RADIUS,0),balls[ball2].velocity);
		if (ball1 < 5 && ball2 >= 5) bumpercollision = true;
		if (ball2 < 5 && ball1 >= 5) bumpercollision = true;
		
        return true;
    }
    return false;
}

bool PoolGame::tableCollision(int ball) {
    Point p = balls[ball].position;
    Vector v = balls[ball].velocity;
	
    bool collision = false;
	
	float setx = 9.1;
	float sety = 5.15;
    if (p[0] >= setx && v[0] > 0) {
        balls[ball].velocity[0] = -v[0];
        collision = true;
    } else if (p[0] <= -setx && v[0] < 0) {
        collision = true;
        balls[ball].velocity[0] = -v[0];
    } else if (p[2] >= sety && v[2] > 0) {
        collision = true;
        balls[ball].velocity[2] = -v[2];
    } else if (p[2] <= -sety && v[2] < 0) {
        collision = true;
        balls[ball].velocity[2] = - v[2];
    }

	if (m_type == 1) {
		float setx = 8.5;
		float sety = 1.1;
		float setz = 2.5;
		Point test[12] = { Point(setx,0.5,-sety),Point(setx,0.5,sety),Point(-setx,0.5,-sety),Point(-setx,0.5,sety), 
			Point(0,0.5,-sety),Point(0,0.5,sety),Point(-sety,0.5,0),Point(sety,0.5,0),
			Point(0,0.5,-setz),Point(0,0.5,setz),Point(-setz,0.5,0),Point(setz,0.5,0)
		};

		for (int i = 0; i < 12; i++) {
			Point x1 = Point(balls[ball].position);
			Point x2 = Point(test[i]);

			Vector v1 = Vector(balls[ball].velocity);
			Vector v2 =-v1;

			Vector d1 = x1 - x2;
			Vector d2 = x2 - x1;
			if (d1.length() <= 2.4 * BALL_RADIUS && dot(v1, d2) > 0) {
				collision = true;
				balls[ball].velocity = v - dot(v1 - v2, d1) / SQR(d1.length()) * d1;
				balls[ball].vAngular = cross(Vector(0, BALL_RADIUS, 0), balls[ball].velocity);
				break;
			}
		}
	}
    if (collision) {
        balls[ball].velocity = balls[ball].velocity * 0.75; // 75% elasticity
        balls[ball].vAngular =cross(Vector(0,BALL_RADIUS,0),balls[ball].velocity);
        return true;
    }

    return false;
}

bool PoolGame::checkHole(int ball) {
	/*Point test[6] = { Point(0,0,-5.5),Point(0,0,5.5),
		Point(-9.4,0,-5.5),Point(-9.4,0,5.5), Point(9.4,0,-5.5),Point(9.4,0,5.5)
	};*/
	if (m_type == 0) {
		Point test[6] = { Point(0,0.5,-5.2),Point(0,0.5,5.2),
			Point(-9.3,0.5,-5.2),Point(-9.3,0.5,5.2), Point(9.3,0.5,-5.2),Point(9.3,0.5,5.2)
		};
		for (int i = 0; i < 6; i++) {
			if ((balls[ball].position - test[i]).length() <= 0.4) {
				balls[ball].fallIntoHole();
				balls[ball].velocity = Vector(0, 0, 0);
				return true;
			}
		}
		/* if (   (balls[ball].position - Point(0,1,-3.25)).length() < 0.75
		|| (balls[ball].position - Point(0,1,3.25)).length() < 0.75
		|| (balls[ball].position - Point(-7.25,1,-3.25)).length() < 0.75
		|| (balls[ball].position - Point(-7.25,1,3.25)).length() < 0.75
		|| (balls[ball].position - Point(7.25,1,-3.25)).length() < 0.75
		|| (balls[ball].position - Point(7.25,1,3.25)).length() < 0.75 )
		{
		balls[ball].fallIntoHole();
		return true;
		}*/
		return false;
	}
	else {
		
		Point test[2] = { Point(8.5,0.5,0),Point(-8.5,0.5,0) };
		for (int i = 0; i < 2; i++) {
			if ((balls[ball].position - test[i]).length() <= 0.4) {
				
				if (ball < 5) {
					if (i ==1 ) {
						balls[ball].position[0] = -7;
						balls[ball].position[2] = 0;
						balls[ball].velocity = Vector(0, 0, 1);
						int times = 2;
						for (int j = 0+5; j < 5+5; j++) {
							if (times > 0) {
								if (!balls[j].isInHole()) {
									balls[j].fallIntoHole();
									times--;
								}
							}

						}
						if (times > 0) {
							winner = 2;
						}
						return false;
					}
					else {
						balls[ball].velocity = Vector(0, 0, 0);
						balls[ball].fallIntoHole();
						for (int j = 0; j < 5; j++) {
							if (!balls[j].isInHole()) {
								return true;
							}
						}
						winner = 1;
						return true;
					}
				}
				else {
					if (i == 0) {
						balls[ball].position[0] = 7;
						balls[ball].position[2] = 0;
						balls[ball].velocity = Vector(0,0,0.1);
						int times = 2;
						for (int j = 0; j < 5; j++) {
							if (times > 0) {
								if (!balls[j].isInHole()) {
									balls[j].fallIntoHole();
									times--;
								}
							}
							
						}
						if (times > 0) {
							winner = 1;
						}
						return false;
					}
					else {
						balls[ball].velocity = Vector(0, 0, 0);
						balls[ball].fallIntoHole();
						for (int j = 0+5; j < 5+5; j++) {
								if (!balls[j].isInHole()) {
									return true;
								}
						}
						winner = 2;
					}
				}
			
			}
		}
	
		return false;
	}
	
}
