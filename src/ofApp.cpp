#include <iostream>
using namespace std;
#include "ofApp.h"
#include "ode/ode.h"
#include "simulation.h"

//--------------------------------------------------------------
void ofApp::setup(){
    game = new simulation();
}

//--------------------------------------------------------------
void ofApp::update(){
    game->Update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    game->Draw();
}

//--------------------------------------------------------------
void ofApp::exit() {
    game->Destroy();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    game->KeyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    game->KeyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

// Function returns a value that moves between the range min and max in a sinusoidal pattern
float ofApp::VaryValue(float min, float max, float speed) {
    return (cos(ofGetElapsedTimef()*speed)*(max-min)+(max-min))/2+min;
}
