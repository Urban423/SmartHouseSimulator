#pragma once
#include "NetworkManager.h"
#include "Transform.h"

enum class NetworkMode: char {
    Offline,
    Client,
    Host
};

enum class NetState: char {
    Idle,
    Connecting,
    WaitingForHandshake,
    LoadingScene,
    InGame,
    Disconnecting
};

enum class MSGType: char {
    Connect,
    Disconect,
    ConnectAccepted,
    Input,
    RequestScene,
    LoadScene,
    Snapshot,
};

class ClientServerSystem {
public:
    void host(int port);
    void connect(const char* ip, int port);
    void disconect();

    inline bool isOffline() { return networkMode == NetworkMode::Offline; }
    inline bool isHost() { return networkMode == NetworkMode::Host; }
    inline bool isClient() { return networkMode == NetworkMode::Client; }
    inline void setState(NetState newState) { netState = newState; }

    inline static ClientServerSystem& getInstance() {
        static ClientServerSystem clientServerSystem;
        return clientServerSystem;
    }
    inline Object getLocalPlayer() { return localPlayer; }
    

    void Start();
    void FixedUpdate();
public:
    void handleHostMessage(MessageView& msg);
    void handleClientMessage(MessageView& msg);
private:
    NetworkMode networkMode = NetworkMode::Offline;
    NetState netState = NetState::Idle;
    int connectMessageID;
    int clientID = -1;
    Object localPlayer;

    SparseSet netToEntity;
};