#pragma once

#include "ofApp.h"
#include "ofMain.h"
#include "simulation.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    simulation* game;

    // Function returns a value that moves between the range min and max in a sinusoidal pattern
    static float VaryValue(float min, float max, float speed);
};

/* ODE requires a global function to use as the collision callback; this
 * function, combined with the ofApp pointer, allows us to put the collision
 * code within myApp. Look at the .cpp for details of how this works.
 */
static void nearCallback (void *, dGeomID o1, dGeomID o2);
extern ofApp *myApp;
