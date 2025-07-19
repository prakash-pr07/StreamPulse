// StreamPulse: Multimedia Streaming Engine Simulation
// Author: Prakash (Qualcomm-Aligned LLD Project)

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <memory>
using namespace std;

// -------- Frame Class --------
class Frame {
public:
    int id;
    string data;
    Frame(int id, string data) : id(id), data(data) {}
};

// -------- Codec Strategy --------
class CodecStrategy {
public:
    virtual void decode(Frame* frame) = 0;
};

class BasicDecoder : public CodecStrategy {
public:
    void decode(Frame* frame) override {
        cout << "[Decoder] Decoding frame " << frame->id << endl;
        this_thread::sleep_for(chrono::milliseconds(50)); // simulate decode time
        frame->data += " [decoded]";
    }
};

// -------- State Pattern --------
enum PlayerStateType { STOPPED, PLAYING, PAUSED };

class PlayerState {
public:
    virtual void handleState() = 0;
};

class PlayingState : public PlayerState {
public:
    void handleState() override {
        cout << "[State] Player is now PLAYING...\n";
    }
};

class PausedState : public PlayerState {
public:
    void handleState() override {
        cout << "[State] Player is PAUSED.\n";
    }
};

class StoppedState : public PlayerState {
public:
    void handleState() override {
        cout << "[State] Player is STOPPED.\n";
    }
};

// -------- Buffer Manager --------
class BufferManager {
private:
    queue<Frame*> buffer;
    int capacity;
    mutex mtx;

public:
    BufferManager(int size) : capacity(size) {}

    bool push(Frame* f) {
        lock_guard<mutex> lock(mtx);
        if (buffer.size() < capacity) {
            buffer.push(f);
            return true;
        } else {
            cout << "[Buffer] Full, dropping frame " << f->id << endl;
            return false;
        }
    }

    Frame* pop() {
        lock_guard<mutex> lock(mtx);
        if (!buffer.empty()) {
            Frame* f = buffer.front();
            buffer.pop();
            return f;
        }
        return nullptr;
    }

    bool isEmpty() {
        lock_guard<mutex> lock(mtx);
        return buffer.empty();
    }
};

// -------- Renderer --------
class Renderer {
public:
    void render(Frame* f) {
        cout << "[Renderer] Displaying frame " << f->id << ": " << f->data << endl;
        this_thread::sleep_for(chrono::milliseconds(70));
    }
};

// -------- Streaming Engine --------
class StreamEngine {
private:
    CodecStrategy* decoder;
    BufferManager buffer;
    Renderer renderer;
    unique_ptr<PlayerState> state;
    PlayerStateType currentState;

public:
    StreamEngine() : decoder(new BasicDecoder()), buffer(5), currentState(STOPPED) {
        state = make_unique<StoppedState>();
    }

    void setState(PlayerStateType s) {
        currentState = s;
        switch (s) {
            case PLAYING: state = make_unique<PlayingState>(); break;
            case PAUSED: state = make_unique<PausedState>(); break;
            case STOPPED: state = make_unique<StoppedState>(); break;
        }
        state->handleState();
    }

    void stream(vector<Frame*> streamData) {
        setState(PLAYING);

        // Decode and Buffer Thread
        thread decoderThread([&]() {
            for (auto frame : streamData) {
                if (currentState != PLAYING) break;
                decoder->decode(frame);
                buffer.push(frame);
            }
        });

        // Renderer Thread
        thread renderThread([&]() {
            while (true) {
                if (currentState != PLAYING && buffer.isEmpty()) break;
                Frame* f = buffer.pop();
                if (f != nullptr) renderer.render(f);
                else this_thread::sleep_for(chrono::milliseconds(30));
            }
        });

        decoderThread.join();
        renderThread.join();

        setState(STOPPED);
    }
};

// -------- Main --------
int main() {
    cout << "\n[StreamPulse Simulation Starts]\n\n";
    StreamEngine engine;

    vector<Frame*> videoData;
    for (int i = 1; i <= 10; ++i) {
        videoData.push_back(new Frame(i, "frame_data"));
    }

    engine.stream(videoData);

    cout << "\n[StreamPulse Simulation Ends]\n";
    return 0;
}
