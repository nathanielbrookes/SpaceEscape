#include "ofApp.h"
#include "particle.h"
#include "ode/ode.h"

particle::particle(glm::vec3 pos, int entityID) {
    this->id = entityID;

    this->SetPosition(pos);

    this->material.setDiffuseColor(ofColor(0, 0, 0, 150));
    this->material.setSpecularColor(ofColor(0, 0, 0, 150));
    this->material.setAmbientColor(ofColor(0, 0, 0, 150));
}

void particle::SetPosition(glm::vec3 pos) {
    this->pos = glm::vec3(pos.x, pos.y-15, pos.z);
}

glm::vec3 particle::GetPosition() {
    return this->pos;
}

void particle::Draw() {
    if (this->captured) return;

    glm::vec3 pos = this->GetPosition();

    // Set colour to vary over time
    this->material.setEmissiveColor(ofColor(ofApp::VaryValue(145, 200, 1), ofApp::VaryValue(22, 50, 1), ofApp::VaryValue(181, 200, 1), ofApp::VaryValue(150, 255, 5)));

    ofPushMatrix();
    ofTranslate(pos.x, pos.y + 10, pos.z);
    ofRotateXDeg(90.0);
    ofSetColor(150, 150, 150, 100);
    ofDrawCircle(0, -1, 0, ofApp::VaryValue(20, 30, 2));
    ofPopMatrix();

    float liftValue = ofApp::VaryValue(0, 15, 2);

    ofSetColor(255);
    ofDrawSphere(pos.x, pos.y-(liftValue+5), pos.z, 5);

    this->material.begin();
    ofDrawSphere(pos.x, pos.y-(liftValue+5), pos.z, 15);
    this->material.end();
}
