#include "ofMain.h"
#include "ode/ode.h"

#ifndef CAMERA_H
#define CAMERA_H


class Camera
{
private:
    glm::vec3 position = {0,0,0};

    float pitch;
    float zoomDistance;
    float angleAroundTarget;

public:
    Camera();
    void SetPosition(glm::vec3 pos);
    glm::vec3 GetPosition();
    void Start();
    void End();
    void LookAt(glm::vec3 pos);
    void Reset();
    float GetPitch();
    void SetPitch(float pitch);
    float GetZoomDistance();
    void SetAngleAroundTarget(float angleAroundTarget);
    float GetAngleAroundTarget();
    void SetZoomDistance(float zoomDistance);
    float CalculateHorizontalDistance();
    float CalculateVerticalDistance();

    ofCamera cam;
};

#endif // CAMERA_H
