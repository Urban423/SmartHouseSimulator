#include "NetworkManager.h"
#include "TimeSystem.h"

void NetworkEndpoint::sendDirectMessage(int id, MessageType type, const void* data, size_t size, int relID) {
    auto& buffer = connections[id].directBuffer;
    MessageHeader header{};
    header.type = type;
    header.reliableId = relID;
    header.size = static_cast<int>(size);
    size_t oldSize = buffer.size();
    buffer.resize(oldSize + sizeof(MessageHeader) + size);
    memcpy(buffer.data() + oldSize, &header, sizeof(header));
    if(size > 0) memcpy(buffer.data() + oldSize + sizeof(header), data, size);
}

void NetworkEndpoint::sendReliable(int id, MessageType type, const void* data, size_t size) {
    int relID = reliableId++;
    ReliablePacket packet{};
    packet.reliableId = relID;
    packet.connectionID = id;
    packet.lastSendTick = Time::tick;
    packet.attemptNumber = 1;
    packet.type = type;
    packet.size = static_cast<int>(size);
    packet.data.resize(size);
    if(size > 0) memcpy(packet.data.data(), data, size);
    reliablePackets.push_back(packet);

    sendDirectMessage(id, type, data, size, relID);
}

void NetworkEndpoint::sendMessage(MessageType type, const void* data, size_t size) {
    MessageHeader header{};
    header.type = type;
    header.reliableId = -1;
    header.size = static_cast<int>(size);
    size_t oldSize = broadcastBuffer.size();
    broadcastBuffer.resize(oldSize + sizeof(MessageHeader) + size);
    memcpy(broadcastBuffer.data() + oldSize, &header, sizeof(header));
    if(size > 0) memcpy(broadcastBuffer.data() + oldSize + sizeof(header), data, size);
}

void NetworkEndpoint::send() {
    PacketHeader* header = reinterpret_cast<PacketHeader*>(broadcastBuffer.data());
    header->tick = Time::tick;
    header->id = packetId++;
    header->broadcastSize = broadcastBuffer.size() - sizeof(PacketHeader);
    for(int i = 0; i < connections.size(); i++) {
        header->directSize = connections[i].directBuffer.size();
        broadcastBuffer.resize(sizeof(PacketHeader) + header->broadcastSize + connections[i].directBuffer.size());
        header = reinterpret_cast<PacketHeader*>(broadcastBuffer.data());
        if(connections[i].directBuffer.size() > 0) 
            memcpy(broadcastBuffer.data() + sizeof(PacketHeader) + header->broadcastSize, connections[i].directBuffer.data(), connections[i].directBuffer.size());
        sendto(сsocket, broadcastBuffer.data(), static_cast<int>(broadcastBuffer.size()), 0, reinterpret_cast<sockaddr*>(&connections[i].address), sizeof(connections[i].address));
        connections[i].directBuffer.clear();
    }
    broadcastBuffer.resize(sizeof(PacketHeader));
}









void NetworkEndpoint::receive() {
    std::vector<char> buffer(4096);
    sockaddr_in sender{};
    int senderSize = sizeof(sender);
    while(true)
    {
        int received = recvfrom(сsocket, buffer.data(), static_cast<int>(buffer.size()), 0, reinterpret_cast<sockaddr*>(&sender), &senderSize);
        if(received < sizeof(PacketHeader)) break;
        if(received == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if(err == WSAEWOULDBLOCK) break;
            break;
        }
        PacketHeader* packet = reinterpret_cast<PacketHeader*>(buffer.data());
        if(packet->broadcastSize + packet->directSize + sizeof(PacketHeader) != received) break;

        int senderID = getTargetId(sender);
        char* cursor = buffer.data() + sizeof(PacketHeader);
        size_t broadcastEnd =  reinterpret_cast<size_t>(cursor) + packet->broadcastSize;
        while(reinterpret_cast<size_t>(cursor) < broadcastEnd) {
            processMessage(reinterpret_cast<MessageHeader*>(cursor), cursor, senderID, sender, received, buffer);
        }

        size_t directEnd = reinterpret_cast<size_t>(cursor) + packet->directSize;
        while(reinterpret_cast<size_t>(cursor) < directEnd) {
            processMessage(reinterpret_cast<MessageHeader*>(cursor), cursor, senderID, sender, received, buffer);
        }
    }
}

void NetworkEndpoint::processMessage(MessageHeader* msg, char*& cursor, int& senderID, sockaddr_in& sender, int received, std::vector<char>& buffer) {
        if(senderID == -1 && msg->type != MessageType::Connect) return;
        switch(msg->type) {
            case(MessageType::Connect): {
                if(senderID == -1) senderID = createConnection(sender);
                sendReliable(senderID, MessageType::ConnectAccept, &senderID, sizeof(senderID));
                break;
            }
            case(MessageType::Confirm): {
                //delete rel
                for(size_t i = 0; i < reliablePackets.size(); i++) {
                    if(reliablePackets[i].reliableId == msg->reliableId) {
                        reliablePackets[i] = std::move(reliablePackets.back());
                        reliablePackets.pop_back();
                        break;
                    }
                }
                cursor += sizeof(MessageHeader) + msg->size;
                return;
            }
            case(MessageType::Disconnect): {
                removeConnection(senderID);
                break;
            }
        }

        if(msg->reliableId != -1) {
            sendDirectMessage(senderID, MessageType::Confirm, nullptr, 0, msg->reliableId);
            auto& c = connections[senderID];
            if(wasReceived(c, msg->reliableId)) return;
            markReceived(c, msg->reliableId);
        }

        char* data = cursor + sizeof(MessageHeader);
        if(data + msg->size > buffer.data() + received) return;

        onMessage(msg->type, data, msg->size, senderID);
        cursor += sizeof(MessageHeader) + msg->size;
}

void NetworkEndpoint::updateReliable() {
    for(size_t i = 0; i < reliablePackets.size();) {
        int connectionID = reliablePackets[i].connectionID;
        if(reliablePackets[i].attemptNumber >= RELIABLE_MAX_ATTEMPTS) {
            sendDirectMessage(connectionID, MessageType::Disconnect, nullptr, 0);
            removeConnection(connectionID);
            continue;
        }
        if(Time::tick - reliablePackets[i].lastSendTick > RELIABLE_TIMEOUT) {
            sendDirectMessage(reliablePackets[i].connectionID, reliablePackets[i].type, reliablePackets[i].data.data(), reliablePackets[i].size, reliablePackets[i].reliableId);
            reliablePackets[i].lastSendTick = Time::tick;
            reliablePackets[i].attemptNumber++;
        }
        i++;
    }
}

int NetworkEndpoint::getTargetId(sockaddr_in& address) {
    for(size_t i = 0; i < connections.size(); i++) {
        if(connections[i].address.sin_addr.s_addr == address.sin_addr.s_addr && connections[i].address.sin_port == address.sin_port) {
            return i;
        }
    }
    return -1;
}

int NetworkEndpoint::createConnection(sockaddr_in& address) {
    Connection c{};
    c.address = address;
    c.lastReceiveTick = Time::tick;
    c.connected = true;
    connections.push_back(std::move(c));
    return connections.size() - 1;
}

void NetworkEndpoint::removeConnection(int id) {
    int lastID = connections.size() - 1;

    for(size_t i = 0; i < reliablePackets.size();) {
        if(reliablePackets[i].connectionID == id) {
            reliablePackets[i] = std::move(reliablePackets.back());
            reliablePackets.pop_back();
        }
        else {
            if(reliablePackets[i].connectionID == lastID) reliablePackets[i].connectionID = id;
            i++;
        }
    }

    if(id != lastID) {
        connections[id] = std::move(connections[lastID]);
    }
    connections.pop_back();
}

void NetworkEndpoint::markReceived(Connection& c, int id) {
    c.reliableHistory[c.reliableIndex] = id;
    c.reliableIndex++;
    if(c.reliableIndex >= 5) c.reliableIndex = 0;
}

bool NetworkEndpoint::wasReceived(Connection& c, int id) {
    for(int i = 0; i < 5; i++) {
        if(c.reliableHistory[i] == id) return true;
    }
    return false;
}















void NetworkManager::host(int port) {
    networkMode = NetworkMode::Host;
    server.host(port);
}

void NetworkManager::connect(const char* ip, int port) {
    networkMode = NetworkMode::Client;
    client.connectToServer(ip, port);
}

void NetworkManager::disconnect() {
    switch (networkMode)
    {
        case NetworkMode::Host:
            server.sendMessage(MessageType::Disconnect, nullptr, 0);
            break;
        case NetworkMode::Client:
            client.sendMessage(MessageType::Disconnect, nullptr, 0);
            break;
        default:
            break;
    }
    networkMode = NetworkMode::Offline;
}



void NetworkClient::connectToServer(const char* ip, int port) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    сsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(сsocket == INVALID_SOCKET) return;

    u_long mode = 1;
    ioctlsocket(сsocket, FIONBIO, &mode);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr);
    serverID = createConnection(server);
    sendReliable(serverID, MessageType::Connect, nullptr, 0);
}

void NetworkClient::onMessage(MessageType type, char* data, size_t size, int senderID) {
    switch(type) {
        case MessageType::Snapshot: {
            printf("snapshot received\n");
            break;
        }
        case MessageType::ConnectAccept:
        {
            printf("connected\n");
            break;
        }
    }
}


void NetworkServer::host(int port) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    сsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(сsocket == INVALID_SOCKET) return;

    u_long mode = 1;
    ioctlsocket(сsocket, FIONBIO, &mode);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    bind(сsocket, (sockaddr*)&addr, sizeof(addr));
}

void NetworkServer::onMessage(MessageType type, char* data, size_t size, int senderID) {
    switch(type) {
        case MessageType::Input: {
            printf("input received\n");
            break;
        }
    }
}
