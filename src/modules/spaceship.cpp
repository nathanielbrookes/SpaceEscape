#include "ofApp.h"
#include "ofxAssimpModelLoader.h"
#include "spaceship.h"

spaceship::spaceship(glm::vec3 pos) {
    this->SetPosition(pos);

    this->ship.loadModel("Ship.obj", true);
    this->ship.setRotation(0, -90.0, 1, 0, 0);
    this->ship.setScale(0.2,0.2,0.2);
}

void spaceship::SetPosition(glm::vec3 pos) {
    this->ship.setPosition(pos.x, pos.y, pos.z);
    this->pos = pos;
}

glm::vec3 spaceship::GetPosition() {
    return this->pos;
}

void spaceship::Draw() {
    /*ofSetColor(255, 200, 255);
    ofDrawSphere(this->pos[0], this->pos[1] - 100, this->pos[2], 100);*/

    ofLight light;
    light.setPointLight();
    light.setAttenuation(1,0,0);
    light.setSpecularColor(ofColor(255));
    light.setDiffuseColor(ofColor(255));
    light.setAmbientColor(ofColor(255));

    light.setPosition(this->pos[0], this->pos[1] - 50, this->pos[2] + 100);
    light.lookAt(glm::vec3(this->pos[0], this->pos[1], this->pos[2]));

    ofTranslate(150, -100, 0);
    light.enable();
    this->ship.drawFaces();
    light.disable();
    ofTranslate(-(150), -(-100), -(0));
}
