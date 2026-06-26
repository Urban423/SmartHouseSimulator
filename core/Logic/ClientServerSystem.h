#pragma once

class ClientServerSystem {
public:
    void Start();
    void FixedUpdate();

    inline static ClientServerSystem& getInstance() {
        static ClientServerSystem clientServerSystem;
        return clientServerSystem;
    }
};