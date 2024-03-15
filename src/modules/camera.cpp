#include "camera.h"
#include "ofMain.h"

Camera::Camera() {
    // Create a camera
    this->cam.setupPerspective(true, 60.0, 0, 10000);

    this->Reset();
}

void Camera::Reset() {
    this->SetPitch(-20);
    this->SetZoomDistance(600);
    this->SetAngleAroundTarget(0);
}

void Camera::LookAt(glm::vec3 pos) {
    // Look at position
    cam.lookAt(pos);
}

glm::vec3 Camera::GetPosition() {
    return cam.getPosition();
}

void Camera::SetPosition(glm::vec3 pos){
    cam.setPosition(pos.x, pos.y, pos.z);
}

float Camera::GetPitch() {
    return this->pitch;
}

void Camera::SetPitch(float pitch) {
    this->pitch = max(min(pitch, 89.0f), -89.0f);
}

float Camera::GetZoomDistance() {
    return this->zoomDistance;
}

void Camera::SetZoomDistance(float zoomDistance) {
    this->zoomDistance = max(zoomDistance, 100.0f);
}


float Camera::GetAngleAroundTarget() {
    return this->angleAroundTarget;
}

void Camera::SetAngleAroundTarget(float angleAroundTarget) {
    this->angleAroundTarget = angleAroundTarget;
}

float Camera::CalculateHorizontalDistance() {
    return this->GetZoomDistance() * cos(this->GetPitch() * PI/180);
}

float Camera::CalculateVerticalDistance() {
    return this->GetZoomDistance() * sin(this->GetPitch() * PI/180);
}

void Camera::Start(){
    cam.begin();
}

void Camera::End(){
    cam.end();
}
