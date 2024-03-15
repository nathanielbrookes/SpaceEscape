#include "ofApp.h"
#include "ofMain.h"
#include "ode/ode.h"
#include "platform.h"

platform::platform(dSpaceID space, glm::vec3 pos, float w, float l, int index) {
    this->width = w;
    this->length = l;
    this->depth = 50;
    this->index = index;

    this->box = dCreateBox(space, w, this->depth, l);
    dGeomSetPosition(this->box, pos.x + w/2, pos.y + this->depth/2, pos.z + l/2);

    this->material.setDiffuseColor(ofColor(100, 100, 100)); // Dark color
    this->material.setSpecularColor(ofColor(0, 0, 0));
    this->material.setAmbientColor(ofColor(0, 10, 0));
    this->material.setEmissiveColor(this->emissiveColour);
}

void platform::Draw() {
    // Set platform colour
    if (this->disappearing) {
        this->emissiveColour.set(((cos(ofGetElapsedTimef()/2+50*this->index)+1)/2 * 200), 0, 0);
    }
    else if (this->type == this->STATIC) {
        this->emissiveColour.set(12, 52, 112);
    }
    else if (this->type == this->MOVING_HORIZONTAL || this->type == this->MOVING_VERTICAL) {
        this->emissiveColour.set(112, 12, 12);
    }
    else if (this->type == this->ROTATING) {
        this->emissiveColour.set(17, 112, 12);
    }
    this->material.setEmissiveColor(this->emissiveColour);

    // Draw platform with location and rotation
    glm::vec3 platformPos = this->GetPosition();
    ofQuaternion quat = this->GetOrientation();
    ofPushMatrix();
    ofTranslate(platformPos.x, platformPos.y, platformPos.z);
    ofRotateRad(2 * glm::acos(quat.w()), quat.x(), quat.y(), quat.z());
    ofTranslate(-platformPos.x, -platformPos.y, -(platformPos.z));
    this->material.begin();
    ofDrawBox(platformPos.x, platformPos.y, platformPos.z, this->width, this->depth, this->length);
    this->material.end();
    ofPopMatrix();
}

void platform::Update() {
    glm::vec3 pos = this->GetPosition();
    glm::vec3 velocity = this->GetVelocity();
    glm::vec3 rotationVelocity = this->GetRotationVelocity();

    glm::vec3 newPosition = pos;

    // Handle Moving Platforms
    newPosition += velocity;

    // Update position
    this->SetPosition(newPosition);

    // Handle Rotating Platforms
    float yaw = ofGetElapsedTimef() * rotationVelocity.x;
    float pitch = ofGetElapsedTimef() * rotationVelocity.y;
    float roll = ofGetElapsedTimef() * rotationVelocity.z;

    ofQuaternion quat;
    quat.makeRotate(yaw, ofVec3f(1, 0, 0), pitch, ofVec3f(0, 1, 0), roll, ofVec3f(0, 0, 1));
    this->SetOrientation(quat);

    // Update velocity
    if (this->type == this->MOVING_VERTICAL) {
        this->SetVelocity(glm::vec3(cos((ofGetElapsedTimef()+100*this->index)), cos((ofGetElapsedTimef()+500*this->index))*0.8, 0));
    }
    else if (this->type == this->MOVING_HORIZONTAL) {
        this->SetVelocity(glm::vec3(cos((ofGetElapsedTimef()+500*this->index))*2, 0, 0));
    }
}

void platform::SetPosition(glm::vec3 pos) {
    dGeomSetPosition(this->box, pos.x, pos.y, pos.z);
}

glm::vec3 platform::GetPosition(){
    const dReal *pos = dGeomGetPosition(this->box);

    // {x, y, z}
    return glm::vec3(pos[0], pos[1], pos[2]);
}

void platform::SetOrientation(ofQuaternion quat){
    dQuaternion q = {quat.w(), quat.x(), quat.y(), quat.z()};
    dGeomSetQuaternion(this->box, q);
}

ofQuaternion platform::GetOrientation(){
    dQuaternion orient;
    dGeomGetQuaternion(this->box, orient);

    // {i, j, k, l}
    return ofQuaternion(orient[1], orient[2], orient[3], orient[0]);
}

void platform::SetVelocity(glm::vec3 vel) {
    this->velocity = vel;
}

glm::vec3 platform::GetVelocity() {
    return this->velocity;
}

void platform::SetRotationVelocity(glm::vec3 rotateVel) {
    this->rotationVelocity = rotateVel;
}

glm::vec3 platform::GetRotationVelocity() {
    return this->rotationVelocity;
}

ofPoint platform::GetCentre() {
    const dReal *pos = dGeomGetPosition(this->box);
    return ofPoint(pos[0], pos[1]-this->depth/2, pos[2]);
}

void platform::Collide() {
    this->collisionCount += 1;

    if (!this->visited) {
        this->visited = true;
    }
}
