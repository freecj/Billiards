/*
 * pool.h
 */
#ifndef POOL_H
#define POOL_H

#include "physics.h"
#include "ball.h"
#include <vector>
#include <time.h>
#include "SceneParser.h"
#include "SceneData.h"

#define NUM_BALLS 16
#define TABLE_LENGTH 16.0
#define TABLE_WIDTH 8.0
#define MU_K 0.01

class PoolGame {
public:
    PoolGame(int type);
    void strikeBall(int i, Vector applied_force, double time);
    void updateMotion(double secs);
    bool checkCollision(int ball);
    /* both collision functions are recursive, that is, they begin with balls
     * in motion and keep recursing on other balls that they may hit at each
     * iteration
     */
    std::vector<Ball> balls;
    void reset(int type);
    bool checkHole(int ball);
	int winner;
	bool bumpercollision;
private:
    bool ballCollision(int ball1, int ball2);
    bool tableCollision(int ball);
    double t_prior;
	int m_type;

};

#endif