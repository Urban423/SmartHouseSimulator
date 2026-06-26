#pragma once
#include "IOSystem.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

enum class NetworkMode {
    Offline,
    Client,
    Host
};


constexpr int RELIABLE_MAX_ATTEMPTS = 10;
constexpr int RELIABLE_TIMEOUT = 30;

enum class MessageType {
    Connect,
    Disconnect,
    ConnectAccept,
    Confirm,
    Input,
    Snapshot
};

struct PacketHeader {
    int tick;
    int id;
    int broadcastSize;
    int directSize;
};

struct MessageHeader {
    MessageType type;
    int size;
    int reliableId;
};

struct ReliablePacket {
    int reliableId;
    int connectionID;
    int lastSendTick;
    int attemptNumber;
    MessageType type;
    int size;
    std::vector<char> data;
};

struct Connection {
    sockaddr_in address;
    std::vector<char> directBuffer;
    int lastReceiveTick;
    bool connected;

    int reliableHistory[5]{-1,-1,-1,-1,-1};
    int reliableIndex = 0;
};




class NetworkEndpoint {
public:
    NetworkEndpoint() { broadcastBuffer.resize(sizeof(PacketHeader)); } 
    void sendDirectMessage(int id, MessageType type, const void* data, size_t size, int relID = -1);
    void sendReliable(int id, MessageType type, const void* data, size_t size);
    void sendMessage(MessageType type, const void* data, size_t size);
    void receive();
    void send();
protected:
    int createConnection(sockaddr_in& address);
    void removeConnection(int id);
    void processMessage(MessageHeader* msg, char*& cursor, int& senderID, sockaddr_in& sender, int received, std::vector<char>& buffer);
    virtual void onMessage(MessageType type, char* data, size_t size, int senderID) = 0;
    int getTargetId(sockaddr_in& address);
    void updateReliable();

    void markReceived(Connection& c, int id);
    bool wasReceived(Connection& c, int id);
protected:
    SOCKET сsocket;
    std::vector<char> broadcastBuffer;
    std::vector<Connection> connections;
    std::vector<ReliablePacket> reliablePackets;
    int packetId = 0;
    int reliableId = 0;
};





class NetworkClient : public NetworkEndpoint {
public:
    void connectToServer(const char* ip, int port);
protected:
    void onMessage(MessageType type, char* data, size_t size, int senderID);
private:
    int serverID = -1;
};

class NetworkServer : public NetworkEndpoint {
public:
    void host(int port);
protected:
    void onMessage(MessageType type, char* data, size_t size, int senderID);
};




class NetworkManager {
private:
    NetworkManager() = default;
public:
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    
    inline static NetworkManager& getInstance() {
		static NetworkManager networkManager;
		return networkManager;
	}
    void host(int port);
    void connect(const char* ip, int port);
    void disconnect();
public: 
    NetworkMode networkMode = NetworkMode::Offline;
    NetworkClient client;
    NetworkServer server;
};