#include "ofMain.h"
#include "ofxThreadPool.h"

class ofApp : public ofBaseApp {
    ofxThreadPool<long, long> my_thread;
    bool bPush;
    std::size_t current;
public:
    void setup() {
        bPush = false;
        current = 0;
        
        my_thread.setup([](long l) -> double {
            long u = l;
            for(int i = 0; i < 30; i++) {
                l = u;
                while(l && l != 1) {
                    if(l % 2 == 0) {
                        l /= 2;
                    } else {
                        l = l * 3 + 1;
                    }
                }
            }
            return u;
        }, [this](long l) {});
        
        my_thread.start();
        ofBackground(0, 0, 0);
        ofSetColor(255, 255, 255);
    }
    void update() {
        if(bPush && ofGetFrameNum() % 3 == 0) {
            for(int i = 0; i < 10000; i++) {
                my_thread.push(current * 10000 + i);
            }
            current++;
        }
    }
    void draw() {
        ofDrawBitmapString(ofVAArgsToString("queued:               %8d",
                                            my_thread.queuedNum()), 20, 20);
        ofDrawBitmapString(ofVAArgsToString("processed/frame:      %8d",
                                            my_thread.processedPerFrame()), 20, 40);
        ofDrawBitmapString(ofVAArgsToString("processed/thread:     %8.3f",
                                            my_thread.processedPerThread()), 20, 60);
        ofDrawBitmapString(ofVAArgsToString("thread num:           %8d",
                                            my_thread.threadNum()), 20, 80);
        ofDrawBitmapString("space:   start/stop thread", 20, 110);
        ofDrawBitmapString(std::string("         thread ") + (my_thread.isRunning() ? "running" : "stopped"), 20, 130);
        ofDrawBitmapString("ENTER:   start/stop push queue", 20, 150);
        ofDrawBitmapString(bPush ? "         pushing queue now" : "         doesn't push queue now", 20, 170);
        ofDrawBitmapString("+:       add thread pool", 20, 190);
        ofDrawBitmapString("-:       remove thread pool", 20, 210);
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
