//
//  ofxThreadPool.h
//
//  Created by ISHII 2bit on 2016/03/04.
//
//

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>

#include "ofThreadChannel.h"
#include "ofEvents.h"
#include "ofLog.h"

template <typename Material, typename Result>
struct ofxThreadPool {
    using ThreadProcess    = std::function<Result(Material &)>;
    using CompleteCallback = std::function<void(Result &&)>;
    
    ofxThreadPool() {}
    
    void setup(ThreadProcess thread_process,
               CompleteCallback callback,
               const std::size_t thread_num = 8)
    {
        this->thread_process = thread_process;
        this->callback       = callback;
        
        processed_per_frame = 0;
        queued_count = 0;
        is_running = false;
        
        threads.resize(thread_num);
        is_running_at.resize(thread_num);
        registerUpdateListener();
    }
    
    virtual ~ofxThreadPool() {
        stop();
        material.close();
        processed.close();
        
        ofLogNotice() << "destruct";
    }
    
    void push(const Material &m) {
        ++queued_count;
        material.send(m);
    }
    
#if 902 < (OF_VERSION_MAJOR * 10000 + OF_VERSION_MINOR * 100 + OF_VERSION_PATCH)
    void push(Material &&m) {
        ++queued_count;
        material.send(m);
    }
#endif
    
    void start() {
        if(isRunning()) {
            ofLogWarning("ofxThreadPool") << "thread already running.";
            return;
        }
        is_running = true;
        for(std::size_t i = 0; i < threadNum(); i++) {
            is_running_at[i] = true;
            threads[i] = std::thread([this, i]() {
                Material m;
                while(is_running && is_running_at[i]) {
                    if(material.tryReceive(m)) {
                        processed.send(std::move(this->thread_process(m)));
                    }
                }
            });
        }
    }
    
    void stop() {
        is_running = false;
        for(std::size_t i = 0; i < threadNum(); i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
    }
    
    void addThread() {
        if(is_running) {
            is_running_at.push_back(true);
            int i = is_running_at.size() - 1;
            threads.emplace_back([this, i]() {
                Material m;
                while(is_running && is_running_at[i]) {
                    if(material.tryReceive(m)) {
                        processed.send(std::move(this->thread_process(m)));
                    }
                }
            });
        } else {
            threads.emplace_back();
        }
    }
    
    void removeThread() {
        if(threads.size()) {
            is_running_at.back() = false;
            if(threads.back().joinable()) threads.back().join();
            threads.pop_back();
            is_running_at.pop_back();
        }
    }
    
    bool isRunning() const {
        return is_running;
    }
    
    std::size_t threadNum() const {
        return threads.size();
    }
    
    std::size_t processedPerFrame() const {
        return processed_per_frame;
    }
    
    float processedPerThread() const {
        if(threads.size() == 0) return 0.0f;
        return static_cast<float>(processed_per_frame) / threads.size();
    }
    
    std::size_t queuedNum() const {
        return queued_count;
    }
    
private:
    std::function<void()> body;
    std::vector<std::thread> threads;
    
    std::atomic<bool> is_running;
    std::vector<bool> is_running_at;
    std::size_t processed_per_frame;
    std::size_t queued_count;
    
    ofThreadChannel<Result>   processed;
    ofThreadChannel<Material> material;
    
    ThreadProcess    thread_process;
    CompleteCallback callback;
    
    void update(ofEventArgs &arg) {
        processed_per_frame = 0;
        Result r;
        while(processed.tryReceive(r)) {
            ++processed_per_frame;
            --queued_count;
            callback(std::move(r));
        }
    }
    
    void registerUpdateListener() {
        ofAddListener(ofEvents().update, this, &ofxThreadPool::update, OF_EVENT_ORDER_BEFORE_APP);
    }
    
    void unregisterUpdateListener() {
        ofRemoveListener(ofEvents().update, this, &ofxThreadPool::update);
    }
};
