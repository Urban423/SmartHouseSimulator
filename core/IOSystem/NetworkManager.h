#pragma once
#include "ECSCore.h"
#include "IOSystem.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "stdlibInclude.h"

class WinSock {
private: 
    WinSock() = default;
    ~WinSock() { quit(); }
public:
    WinSock(const WinSock&) = delete;
    WinSock& operator=(const WinSock&) = delete;
    inline static WinSock& getInstance() { static WinSock winSock; return winSock; }

    inline bool init() {
        if (initialized) return true;
        initialized = WSAStartup(MAKEWORD(2,2), &wsa) == 0;
        return initialized;
    }
    inline void quit() { 
        if (initialized) {
            WSACleanup();
            initialized = false;
        }
    }
private:
    WSADATA wsa;
    bool initialized = false;
};


enum class ProcessResult: char {
    Consume,
    Pass,
    Retry
};

struct RawPacket {
    sockaddr_in address;
    int size;
    char* data;
};

struct OutPacket {
    sockaddr_in address;
    const void* part1;
    unsigned long size1;
    const void* part2;
    unsigned long size2;
    const void* part3;
    unsigned long size3;
};

class TransportLayer {
public:
    TransportLayer(): csocket(INVALID_SOCKET) { };
    ~TransportLayer() { close(); };

    TransportLayer(const TransportLayer&) = delete;
    TransportLayer& operator=(const TransportLayer&) = delete;
public:
    bool open();
    bool bindPort(int port);
    void close();

    inline bool send(OutPacket& packet) const {
        if (csocket == INVALID_SOCKET) return false;
        WSABUF bufs[3];
        DWORD count = 0;
        if (packet.size1 > 0) bufs[count++] = { packet.size1, (CHAR*)packet.part1 };
        if (packet.size2 > 0) bufs[count++] = { packet.size2, (CHAR*)packet.part2 };
        if (packet.size3 > 0) bufs[count++] = { packet.size3, (CHAR*)packet.part3 };
        if (count == 0) return false;
        // printf("%d %d %d\n", packet.size1, packet.size2, packet.size3);

        DWORD sent = 0;
        return WSASendTo(csocket, bufs, count, &sent, 0, reinterpret_cast<const sockaddr*>(&packet.address), sizeof(sockaddr_in), nullptr, nullptr) == 0;
    }
    bool receive(RawPacket& out) {
        if (csocket == INVALID_SOCKET) return false;
        socklen_t len = sizeof(out.address);
        int size = recvfrom(csocket, out.data, 1500, 0, (sockaddr*)&out.address, &len);
        if (size == SOCKET_ERROR) return false;
        out.size = size;

        // printf("r %d\n", size);
        return true;
    }
private:
    SOCKET csocket;
};




constexpr int TIMEOUT = 600;

struct AddressComponent {
    sockaddr_in address;
};

struct LastSeenComponent {
    int tick;
};

struct SessionPacket {
    char* data;
    int size;
    int peerID;
};

struct OutSessionPacket {
    int peerID;
    const void* part1;
    unsigned long size1;
    const void* part2;
    unsigned long size2;
    const void* part3;
    unsigned long size3;
};

class SessionLayer {
public:
    SessionLayer(ECSCore& ecs): ecs(ecs) {};
    bool process(RawPacket& rawPacket, SessionPacket& out);
    void process(OutSessionPacket& session, OutPacket& out);
    bool popPeer(int& peerID);
    void beginCleanupCycle() { index = 0; }
    int createPeer(const char* ip, int port, bool& isNew);
private:
    int getPeer(const sockaddr_in& address) const;
    int createPeer(const sockaddr_in& address);
    void deletePeer(int peerID) { ecs.destroy(peerID); }
private:
    ECSCore& ecs;
    int index = 0;
};








struct ReassemblySlot {
    int packetId = -1;
    int peerID = -1;
    int expectedCount = 0;
    int receivedCount = 0;
    int receivedBytes = 0;
    std::vector<char> data;
    std::vector<char> mask;
};

struct FragmentHeader {
    int packetId;
    int index;
    int count;
    int dataSize;
};

struct PacketHeader {
    int tick;
    int id;
    int broadcastSize;
    int directSize;
};

struct FragementPacketHeader {
    FragmentHeader fragmentHeader;
    PacketHeader packetHeader;
};

struct FragmentCursor {
    int currentPart = 0;
    size_t offsetInPart = 0;
};

constexpr const int MTU = 1400;
constexpr const int HEADER = sizeof(FragmentHeader);
constexpr const int CHUNK = MTU - HEADER;
constexpr int MAX_PACKETS = 32;

class FragmentLayer {
public:
    FragmentLayer(): slots(MAX_PACKETS) {};

    void processFragment(SessionPacket& sessionPacket);
    bool pullPacket(SessionPacket& out);
    bool getFragmentPacket(OutSessionPacket& out);
    void setOutSessionPacket(OutSessionPacket& newPacket) { 
        packet = newPacket;
        offset2 = 0;
        offset3 = 0;
        totalSize = packet.size2 + packet.size3;
        sentTotal = 0;

        PacketHeader header;
        memcpy(&header, newPacket.part1, sizeof(PacketHeader));

        int totalSize = packet.size1 + packet.size2 + packet.size3;
        fragementPacketHeader.fragmentHeader.index = 0;
        fragementPacketHeader.fragmentHeader.dataSize = 0;
        fragementPacketHeader.fragmentHeader.packetId = header.id;
        fragementPacketHeader.fragmentHeader.count = (int)((totalSize + CHUNK - 1) / CHUNK);
        fragementPacketHeader.packetHeader = header;

        packet.peerID = newPacket.peerID;
        packet.part1 = &fragementPacketHeader;
        packet.size1 = sizeof(FragementPacketHeader);
    }
private:
    int getSlotIndex(int packetId) { return packetId % MAX_PACKETS; }
public:
    FragementPacketHeader fragementPacketHeader;
    OutSessionPacket packet;
    int offset2 = 0;
    int offset3 = 0;

    size_t sentTotal = 0;
    size_t totalSize;

    std::deque<SessionPacket> readyPackets;
    std::vector<ReassemblySlot> slots;
};








struct Packet {
    char* data;
    int size;
    int senderID;
    int id;
    int tick;
    int broadcastSize;
    int directSize;
};

struct directComponent {
    PacketHeader header;
    std::vector<char> directSection;
};

class PacketLayer {
public:
    PacketLayer(ECSCore& ecs): ecs(ecs) { };
    ~PacketLayer() {}
public:
    bool build(int peerID, OutSessionPacket& out);
    bool process(SessionPacket& sessionPacket, Packet& out);
    void clearGlobal() { globalSection.clear(); }
    void writeToPeer(int peerID, const char* header, int headerSize, const char* data, int dataSize);
    void writeGlobal(const char* header, int headerSize, const char* data, int dataSize);
private:
    ECSCore& ecs;
    std::vector<char> globalSection;
    int packetId = 0;
};







struct MessageHeader {
    int size;
    char type;
};

struct MessageView {
    MessageHeader header;
    char* data;
    int senderID;
};


class MessageLayer {
public:
    MessageLayer(ECSCore& ecs): ecs(ecs) {};

    bool pull(MessageView& out);
    void begin(Packet& packet);
    void buildHeader(int size, char userData, MessageHeader& out);
private: 
    ECSCore& ecs;

    Packet currentPacket;
    char* cursor = nullptr;
    int remaining = 0;
};







constexpr int RELIABLE_INBOX_WINDOW = 64;
constexpr int RELIABLE_TIMEOUT = 60;
constexpr int RELIABLE_MAX_ATTEMPS = 10;

enum class MessageType: char {
    None,
    Reliable,
    Confirm,
};

struct ProtocolHeader {
    MessageType type;
    int reliableId;
};

struct ProtocolMessage {
    MessageHeader message;
    ProtocolHeader protocol;
    int attemps = 0;
    const char* data;
    int size;
    int sendToPeerID;
};

struct ReliableMessage {
    MessageHeader message;
    ProtocolHeader protocol;
    int peerID;
    int lastSendTick;
    int attemptNumber;
    std::vector<char> data;
};

struct ReliableComponent {
    std::vector<ReliableMessage> messages;
};

struct ProtocolSideEffect {
    bool sendConfirm = false;

    bool reliableDelivered = false;
    int deliveredPeerID = -1;
    int deliveredReliableID = -1;
};

class ProtocolLayer {
public:
    ProtocolLayer(ECSCore& ecs): ecs(ecs) {};
public:
    void buildHeader(MessageHeader& message, MessageType type, int peerID, const char* data, int size, ProtocolHeader& out);
    ProcessResult process(MessageView& out, ProtocolSideEffect& side);
    bool popResend(ProtocolMessage& out);
    void beginResendCycle() { rrComponentIndex = 0; rrMessageIndex = 0; }
private:
    ECSCore& ecs;
    int nextReliableId = 0;
    size_t rrComponentIndex = 0;
    size_t rrMessageIndex = 0;
    bool rrWrapped = false;

    uint64_t receivedMask = 0;
    int baseId = 0;
};










enum class NetworkEventType
{
    Connected,
    Disconnected,
    ReliableDelivered,
    ReliableFailed
};

struct NetworkEvent
{
    NetworkEventType type;
    int peerID;
    int reliableID;
};

struct HeaderBuffer {
    MessageHeader message;
    ProtocolHeader protocol;
};

struct NetStats {
    int bytesSent = 0;
    int bytesRecv = 0;
    int packetsSent = 0;
    int packetsRecv = 0;
    int lostPackets = 0;
};

class NetworkManager {
private:
    NetworkManager(): sessionLayer(ecs), packetLayer(ecs), messageLayer(ecs), protocolLayer(ecs) {};
public:
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    inline static NetworkManager& getInstance() { static NetworkManager networkManager; return networkManager; }
public:
    inline void open() { transportLayer.open(); }
    inline void bind(int port) { transportLayer.bindPort(port); }
    inline void close() { transportLayer.close(); }

    void update();
    void receive();
    void send();
    void pushMessage(const char* data, int size, char userData);
    void pushDirectMessage(const char* data, int size, char userData, int peerID);
    int pushReliableMessage(const char* data, int size, char userData, int peerID);
    int pushReliableMessage(const char* data, int size, char userData, const char* id, int port);
    bool pullMessage(MessageView& out);
    bool pullEvent(NetworkEvent& out);
private:
    void pushEvent(NetworkEventType type, int peerID = -1, int reliableID = -1);
private:
    ECSCore ecs;
    int serverID;
    char recvBuffer[1500];
    std::deque<NetworkEvent> events;
    NetStats netStats;

    TransportLayer transportLayer;
    SessionLayer sessionLayer;
    FragmentLayer fragmentLayer;
    PacketLayer packetLayer;
    MessageLayer messageLayer;
    ProtocolLayer protocolLayer;
};