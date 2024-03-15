#include "ofMain.h"
#include "ode/ode.h"
#include "ofxAssimpModelLoader.h"

#ifndef PLAYER_H
#define PLAYER_H

class player {
private:
    ofxAssimpModelLoader model;

    dWorldID world;
    dSpaceID space;

    dBodyID body;
    dReal width, height, depth;

    glm::vec3 initPos = {0, 0, 0};
    glm::vec3 platformVelocity = {0, 0, 0};

    // Player Particle Effect
    ofVboMesh particlesMesh;
    vector<ofPoint> particles;
    int timeInterval = 50;
    int lastTime = 0;

    ofSoundPlayer jumpFx;

public:
    player(dWorldID worldID, dSpaceID spaceID, glm::vec3 pos);
    void Draw();
    void Update();
    void Reset();
    void SetPosition(glm::vec3 pos);
    glm::vec3 GetPosition();
    void SetOrientation(ofQuaternion quat);
    ofQuaternion GetOrientation();
    void SetPlatformVelocity(glm::vec3 vel);
    glm::vec3 GetPlatformVelocity();
    void Jump();
    void Die();

    dGeomID box;
    bool isJumping = false;
    bool jumpAllowed = true;
    bool hide = false;
    bool isDead = false;
    bool isDying = false;
    float velocity = 0;
    float rotation = 0;
    float rotationVelocity = 0;
};

#endif // PLAYER_H
