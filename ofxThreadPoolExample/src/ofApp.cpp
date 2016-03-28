#include "ofMain.h"
#include "ofxThreadPool.h"

#include <vector>

class ofApp : public ofBaseApp {
    ofxThreadPool<std::string, ofJson> my_thread;
    bool bPush;
    std::size_t current;
    std::string large_json_str;
public:
    void setup() {
        bPush = false;
        current = 0;
        
        my_thread.setup([](std::string &&json_str) -> ofJson {
            return ofJson::parse(json_str);
        }, [this](ofJson &&l) {});
        
        my_thread.start();
        ofBackground(0, 0, 0);
        ofSetColor(255, 255, 255);
        
        ofJson json;
        json["i"] = ofJson::array();
        json["j"] = ofJson::array();
        for(std::size_t i = 0; i < 20000; i++) {
            json["i"][i] = i;
            json["j"][i] = ofRandom(10.0);
        }
        
        large_json_str = json.dump();
    }
    void update() {
        if(ofGetFrameNum() % 3 == 0) {
            if(bPush) {
                for(int i = 0; i < 10; i++) {
                    my_thread.push(large_json_str);
                }
                current++;
            } else {
                for(int i = 0; i < 10; i++) {
                    ofJson json = ofJson::parse(large_json_str);
                }
            }
        }
        ofSetWindowTitle(ofToString(ofGetFrameRate()));
    }
    void draw() {
        ofSetColor(bPush ? 0 : 255, bPush ? 255 : 0, 0);
        ofDrawLine(ofGetFrameNum() % (int)ofGetWidth(), 0, ofGetFrameNum() % (int)ofGetWidth(), ofGetHeight());
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(bPush ? "now parse in background" : "now parse in main thread", 20, 20);
        ofDrawBitmapString(ofVAArgsToString("queued:               %8d",
                                            my_thread.queuedNum()), 20, 50);
        ofDrawBitmapString(ofVAArgsToString("processed/frame:      %8d",
                                            my_thread.processedPerFrame()), 20, 70);
        ofDrawBitmapString(ofVAArgsToString("processed/thread:     %8.3f",
                                            my_thread.processedPerThread()), 20, 90);
        ofDrawBitmapString(ofVAArgsToString("thread num:           %8d",
                                            my_thread.threadNum()), 20, 110);
        ofDrawBitmapString("space:   start/stop thread", 20, 140);
        ofDrawBitmapString(std::string("         thread ") + (my_thread.isRunning() ? "running" : "stopped"), 20, 160);
        ofDrawBitmapString("ENTER:   start/stop push queue", 20, 180);
        ofDrawBitmapString(bPush ? "         pushing queue now" : "         doesn't push queue now", 20, 200);
        ofDrawBitmapString("+:       add thread pool", 20, 220);
        ofDrawBitmapString("-:       remove thread pool", 20, 240);
    }
    
    void exit() { my_thread.stop(); }
    
    void keyPressed(int key) {
        if(key == ' ') {
            if(my_thread.isRunning()) my_thread.stop();
            else my_thread.start();
        }
        if(key == OF_KEY_RETURN) {
            bPush ^= true;
        }
        if(key == '+') {
            my_thread.addThread();
        }
        if(key == '-') {
            my_thread.removeThread();
        }
    }
    void keyReleased(int key) {}
    void mouseMoved(int x, int y) {}
    void mouseDragged(int x, int y, int button) {}
    void mousePressed(int x, int y, int button) {}
    void mouseReleased(int x, int y, int button) {}
    void mouseEntered(int x, int y) {}
    void mouseExited(int x, int y) {}
    void windowResized(int w, int h) {}
    void dragEvent(ofDragInfo dragInfo) {}
    void gotMessage(ofMessage msg) {}
};

int main() {
    ofSetupOpenGL(320, 240, OF_WINDOW);
    ofRunApp(new ofApp);
}
