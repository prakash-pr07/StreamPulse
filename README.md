# StreamPulse
Multimedia Streaming Engine Simulation with Real-Time Pipeline


// StreamPulse: Multimedia Streaming Engine Simulation
// Author: Prakash (Qualcomm-Aligned LLD Project)
// Description: Real-time simulation of a multimedia pipeline using C++, reflecting RTOS behavior.

/*
UML Overview — StreamPulse Architecture

    +----------------+             +----------------+            +----------------+
    |  InputManager  |  ------->   |    Decoder     |  ------->  | BufferManager  |
    | (simulated I/P)|             | (BasicDecoder) |            | (Queue w/ lock)|
    +----------------+             +----------------+            +----------------+
                                                                          |
                                                                          v
                                                                 +----------------+
                                                                 |    Renderer    |
                                                                 |  (Frame Out)   |
                                                                 +----------------+

                       ↖                                      ↑
                  Frame Class                          State Pattern (Playback)
               (id + raw data)                        Strategy Pattern (Codec)

Components:
- Frame: Represents media unit (frame_id + data)
- BasicDecoder: Implements CodecStrategy for decoding
- BufferManager: Manages buffer queue with mutex (RTOS-style)
- Renderer: Renders decoded frames (simulated)
- StreamEngine: Central orchestrator
- PlayerState: Manages playback state transitions (Stopped, Playing, Paused)

Design Patterns:
- State Pattern → Playback modes
- Strategy Pattern → Codec switching
- Thread Simulation → Pipeline parallelism
- Mutex (std::lock_guard) → Synchronization like embedded firmware

*/