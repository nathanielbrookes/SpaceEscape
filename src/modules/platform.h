#include "ofMain.h"
#include "ode/ode.h"

#ifndef PLATFORM_H
#define PLATFORM_H

class platform
{
private:
    dSpaceID space;
    int index;

    float width;
    float length;
    float depth;

    glm::vec3 initPos;
    glm::vec3 velocity = {0, 0, 0};
    glm::vec3 rotationVelocity = {0, 0, 0};

    ofMaterial material;
    ofColor emissiveColour = ofColor(100, 0, 0);

public:
    platform(dSpaceID space, glm::vec3 pos, float w, float l, int index);
    void Draw();
    void Update();
    void Collide();
    void SetPosition(glm::vec3 pos);
    glm::vec3 GetPosition();
    ofPoint GetCentre();
    void SetOrientation(ofQuaternion quat);
    ofQuaternion GetOrientation();
    void SetVelocity(glm::vec3 vel);
    glm::vec3 GetVelocity();
    void SetRotationVelocity(glm::vec3 vel);
    glm::vec3 GetRotationVelocity();

    dGeomID box;
    enum platformType {STATIC, MOVING_HORIZONTAL, MOVING_VERTICAL, ROTATING};
    platformType type = STATIC;
    bool disappearing = false;
    bool goal = false;
    bool detect = false;
    bool visited = false;
    int collisionCount = 0;
};

#endif // PLATFORM_H
