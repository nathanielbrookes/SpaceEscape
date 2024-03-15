#include "ofMain.h"
#include "ode/ode.h"
#include "platform.h"
#include "particle.h"
#include "spaceship.h"

#ifndef LEVEL_H
#define LEVEL_H


class level
{
private:
    dWorldID world;
    dSpaceID space;
    dSpaceID local_space;
    float zOffset = 0;

    void GenerateLevel();

public:
    level(dWorldID worldID, dSpaceID spaceID, int levelNo);
    void Start();
    void Reset();
    void Draw();
    void Update();
    void Finish();
    int GetPlatformIndex(dGeomID ID);

    vector<platform> platforms;
    vector<particle> particles;
    spaceship* ship;
    float startTime = 0;
    float endTime = 0;
    int levelNo = 1;
    int particlesCaptured = 0;
    bool complete = false;
};

#endif // LEVEL_H
