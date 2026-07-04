#include "NetworkManager.h"
#include "TimeSystem.h"

bool TransportLayer::open() {
    if (csocket != INVALID_SOCKET) return true;
    if (!WinSock::getInstance().init()) return false;

    csocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (csocket == INVALID_SOCKET) {
        return false;
    }
    u_long mode = 1;
    if (ioctlsocket(csocket, FIONBIO, &mode) == SOCKET_ERROR) {
        closesocket(csocket);
        csocket = INVALID_SOCKET;
        return false;
    }
    return true;
}

bool TransportLayer::bindPort(int port) {
    if (!open()) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    return ::bind(csocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != SOCKET_ERROR;
}

void TransportLayer::close() {
    if (csocket != INVALID_SOCKET) {
        closesocket(csocket);
        csocket = INVALID_SOCKET;
    }
}



bool SessionLayer::process(RawPacket& rawPacket, SessionPacket& out) {
    bool isNewPeer = false;

    int id = getPeer(rawPacket.address);
    if (id == -1) {
        id = createPeer(rawPacket.address);
        isNewPeer = true;
    }
    ecs.GetComponent<LastSeenComponent>(id).tick = Time::tick;

    out.peerID = id;
    out.data = rawPacket.data;
    out.size = rawPacket.size;
    return isNewPeer;
}

void SessionLayer::process(OutSessionPacket& session, OutPacket& out) {
    out.address = ecs.GetComponent<AddressComponent>(session.peerID).address;

    out.part1 = session.part1;
    out.size1 = session.size1;

    out.part2 = session.part2;
    out.size2 = session.size2;

    out.part3 = session.part3;
    out.size3 = session.size3;
}

bool SessionLayer::popPeer(int& peerID) {
    int currentTick = Time::tick;
    Span<LastSeenComponent> lastSeen = ecs.GetComponents<LastSeenComponent>();

    while (index < lastSeen.size()) {
        if (currentTick - lastSeen[index].tick > TIMEOUT) {
            peerID = ecs.GetEntity(lastSeen[index]);
            return true;
        }
        ++index;
    }
    return false;
}


int SessionLayer::createPeer(const char* ip, int port, bool& isNew) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);
    
    isNew = false;
    int id = getPeer(addr);
    if (id == -1) {
        isNew = true;
        id = createPeer(addr);
    }
    ecs.GetComponent<LastSeenComponent>(id).tick = Time::tick;
    return id;
}

int SessionLayer::createPeer(const sockaddr_in& address) {
    int peerID = ecs.create();
    ecs.AddComponent<AddressComponent>(peerID).address = address;
    ecs.AddComponent<LastSeenComponent>(peerID).tick = Time::tick;
    ecs.AddComponent<directComponent>(peerID);
    ecs.AddComponent<ReliableComponent>(peerID);
    return peerID;
}

int SessionLayer::getPeer(const sockaddr_in& address) const {
    Span<AddressComponent> addresses = ecs.GetComponents<AddressComponent>();
    for (int i = 0; i < addresses.size(); ++i) {
        const AddressComponent& component = addresses[i];
        if (component.address.sin_addr.s_addr == address.sin_addr.s_addr && component.address.sin_port == address.sin_port) {
            return ecs.GetEntity(component);
        }
    }
    return -1;
}









void FragmentLayer::processFragment(SessionPacket& sessionPacket) {
    if (sessionPacket.size < sizeof(FragmentHeader)) { printf("erro\n"); return; }

    auto* fh = (FragmentHeader*)sessionPacket.data;

    size_t idx = getSlotIndex(fh->packetId);
    ReassemblySlot& slot = slots[idx];
    if (slot.packetId != fh->packetId) {
        slot.packetId = fh->packetId;
        slot.expectedCount = fh->count;
        slot.receivedCount = 0;
        slot.receivedBytes = 0;
        slot.peerID = sessionPacket.peerID;

        slot.mask.clear();
        slot.mask.resize(fh->count, false);

        size_t size = 0;
        if (__builtin_mul_overflow((size_t)fh->count, (size_t)CHUNK, &size)) return;

        slot.data.clear();
        slot.data.resize(size);
    }

    int offset = (fh->index - 1) * CHUNK;
    int headerSize = sizeof(FragmentHeader);
    int dataSize = sessionPacket.size - headerSize;

    memcpy(slot.data.data() + offset, sessionPacket.data + headerSize, dataSize);
    if (!slot.mask[fh->index - 1]) {
        slot.mask[fh->index - 1] = 1;
        slot.receivedCount++;
    }

    slot.receivedBytes += dataSize;
    // printf("packet: %d %d %d\n", slot.receivedCount, slot.receivedBytes, slot.expectedCount);
    if (slot.receivedCount == slot.expectedCount) {
        SessionPacket out;

        out.data = slot.data.data();
        out.size = slot.receivedBytes;
        out.peerID = slot.peerID;

        readyPackets.push_back(out);
        slot.packetId = -1;
    }
}

bool FragmentLayer::pullPacket(SessionPacket& out) {
    if (readyPackets.empty()) return false;

    out = readyPackets.front();
    readyPackets.pop_front();
    return true;
}

bool FragmentLayer::getFragmentPacket(OutSessionPacket& out)  {
    if (sentTotal >= totalSize) return false;
    out = {};

    int remaining = CHUNK;
    int took = 0;

    out.peerID = packet.peerID;
    fragementPacketHeader.fragmentHeader.index++;
    fragementPacketHeader.fragmentHeader.count = (int)((totalSize + CHUNK - 1) / CHUNK);
    
    if (fragementPacketHeader.fragmentHeader.index == 1) {
        out.part1 = &fragementPacketHeader;
        out.size1 = sizeof(FragementPacketHeader);
        took += sizeof(FragmentHeader);
        remaining -= took;
    } else {
        out.part1 = &fragementPacketHeader.fragmentHeader;
        out.size1 = sizeof(FragmentHeader);
    }

    int available = packet.size2 - offset2;
    int take = std::min(available, remaining);
    if (take > 0) {
        out.part2 = (char*)packet.part2 + offset2;
        out.size2 = take;
        offset2 += take;
        remaining -= take;
        took += take;
    }

    available = packet.size3 - offset3;
    take = std::min(available, remaining);
    if (take > 0) {
        out.part3 = (char*)packet.part3 + offset3;
        out.size3 = take;
        offset3 += take;
        remaining -= take;
        took += take;
    }

    fragementPacketHeader.fragmentHeader.dataSize = took;

    sentTotal += took;
    return true;
}







bool PacketLayer::process(SessionPacket& sessionPacket, Packet& out) {
    if(sessionPacket.size < sizeof(PacketHeader)) return false;

    PacketHeader header;
    memcpy(&header, sessionPacket.data, sizeof(PacketHeader));
    // printf("packet %d %d\n", header.broadcastSize + header.directSize + sizeof(PacketHeader), sessionPacket.size);
    if(header.broadcastSize + header.directSize + sizeof(PacketHeader) != sessionPacket.size) return false;

    // printf("packet size: %d\n", header.broadcastSize + header.directSize);
    out.data = sessionPacket.data + sizeof(PacketHeader);
    out.size = header.broadcastSize + header.directSize;
    out.senderID = sessionPacket.peerID;
    out.id = header.id;
    out.tick = header.tick;
    out.directSize = header.directSize;
    out.broadcastSize = header.broadcastSize;
    return true;
}

bool PacketLayer::build(int peerID, OutSessionPacket& out) {
    directComponent& direct = ecs.GetComponent<directComponent>(peerID);
    size_t globalSize = globalSection.size();
    size_t directSize = direct.directSection.size();

    if (globalSize == 0 && directSize == 0) return false;

    direct.header.tick = Time::tick;
    direct.header.id = packetId++;
    direct.header.broadcastSize = globalSize;
    direct.header.directSize = directSize;


    out.peerID = peerID;
    out.part1 = &direct.header;
    out.size1 = (unsigned long)sizeof(PacketHeader);

    out.part2 = globalSection.data();
    out.size2 = (unsigned long)globalSection.size();

    out.part3 = direct.directSection.data();
    out.size3 = (unsigned long)direct.directSection.size();
    return true;
}

void PacketLayer::writeToPeer(int peerID, const char* header, int headerSize, const char* data, int dataSize) {
    // if (!data || dataSize == 0) return;
    directComponent& direct = ecs.GetComponent<directComponent>(peerID);

    const size_t oldSize = direct.directSection.size();
    const size_t totalSize = oldSize + headerSize + dataSize;
    if (totalSize > direct.directSection.max_size()) return;

    direct.directSection.resize(totalSize);
    char* dst = direct.directSection.data() + oldSize;

    std::memcpy(dst, header, headerSize);
    if(dataSize > 0) std::memcpy(dst + headerSize, data, dataSize);
}

void PacketLayer::writeGlobal(const char* header, int headerSize, const char* data, int dataSize) {
    // if (!data || dataSize == 0) return;

    size_t oldSize = globalSection.size();
    size_t totalSize = oldSize + headerSize + dataSize;
    if (totalSize > globalSection.max_size()) return;

    globalSection.resize(totalSize);
    char* dst = globalSection.data() + oldSize;

    std::memcpy(dst, header, headerSize);
    if(dataSize > 0) std::memcpy(dst + headerSize, data, dataSize);
}






bool MessageLayer::pull(MessageView& out) {
    if (remaining <= 0) return false;
    if (remaining < sizeof(MessageHeader)) return false;

    MessageHeader header;
    memcpy(&header, cursor, sizeof(MessageHeader));
    // printf("MessageHeader size: %d, reamining: %d\n", header.size, remaining);
    if (header.size < 0) return false;
    if (remaining < sizeof(MessageHeader) + header.size) return false;
    // printf("MessageHeader2 size: %d, reamining: %d\n", header.size, remaining);

    out.header = header;
    out.data = cursor + sizeof(MessageHeader);
    out.senderID = currentPacket.senderID;

    int messageSize = sizeof(MessageHeader) + header.size;
    cursor += messageSize;
    remaining -= messageSize;

    return true;
}

void MessageLayer::begin(Packet& packet) {
    currentPacket = packet;
    cursor = packet.data;
    remaining = packet.size;
}

void MessageLayer::buildHeader(int size, char userData, MessageHeader& out) {
    out.size = size + sizeof(ProtocolHeader);
    out.type = userData;
}






ProcessResult ProtocolLayer::process(MessageView& out, ProtocolSideEffect& side) {
    if (out.header.size < sizeof(ProtocolHeader)) return ProcessResult::Consume;
    const ProtocolHeader* header = reinterpret_cast<const ProtocolHeader*>(out.data);

    out.data += sizeof(ProtocolHeader);
    out.header.size -= sizeof(ProtocolHeader);

    switch (header->type) {
        case MessageType::Confirm: {
            ReliableComponent& reliableComponent = ecs.GetComponent<ReliableComponent>(out.senderID);
            for (size_t i = 0; i < reliableComponent.messages.size(); ++i) {
                if (reliableComponent.messages[i].protocol.reliableId == header->reliableId) {
                    side.reliableDelivered = true;
                    side.deliveredPeerID = out.senderID;
                    side.deliveredReliableID = header->reliableId;

                    reliableComponent.messages[i] = std::move(reliableComponent.messages.back());
                    reliableComponent.messages.pop_back();
                    break;
                }
            }
            return ProcessResult::Consume;
        }

        case MessageType::Reliable: {
            side.sendConfirm = true;
            side.deliveredReliableID = header->reliableId;

            int diff = header->reliableId - baseId;
            if (diff <= 0) {
                return ProcessResult::Consume;
            }
            if (diff >= RELIABLE_INBOX_WINDOW) {
                // return ProcessResult::Consume;
            }
            uint64_t bit = 1ULL << diff;
            if (receivedMask & bit) {
                return ProcessResult::Consume;
            }
            receivedMask |= bit;

            return ProcessResult::Pass;
        }

        default: break;
    }

    return ProcessResult::Pass;
}

void ProtocolLayer::buildHeader(MessageHeader& message, MessageType type, int peerID, const char* data, int size, ProtocolHeader& out) {
    out.type = type;
    out.reliableId = ++nextReliableId;

    if(type == MessageType::Reliable && peerID != -1) {
        ReliableMessage reliableMessage;
        reliableMessage.protocol = out;
        reliableMessage.message = message;
        reliableMessage.lastSendTick = Time::tick;
        reliableMessage.attemptNumber = 1;
        reliableMessage.peerID = peerID;
        reliableMessage.data.resize(size);
        memcpy(reliableMessage.data.data(), data, size);

        auto& comp = ecs.GetComponent<ReliableComponent>(peerID);
        comp.messages.push_back(reliableMessage);
    }
}

bool ProtocolLayer::popResend(ProtocolMessage& out) {
    auto components = ecs.GetComponents<ReliableComponent>();
    const size_t compCount = components.size();
    if (compCount == 0) return false;
    size_t startComp = rrComponentIndex;

    for (size_t c = rrComponentIndex; c < compCount; c++) {
        auto& messages = components[c].messages;
        if (messages.empty()) continue;

        size_t startMsg = (c == 0) ? rrMessageIndex : 0;
        for (size_t m = startMsg; m < messages.size(); m++) {
            auto& msg = messages[m];
            if (Time::tick - msg.lastSendTick > RELIABLE_TIMEOUT) {
                msg.attemptNumber++;
                msg.lastSendTick = Time::tick;

                out.attemps = msg.attemptNumber;
                out.size = msg.data.size();
                out.data = msg.data.data();
                out.sendToPeerID = msg.peerID;
                out.message = msg.message;
                out.protocol = msg.protocol;

                rrComponentIndex = c;
                rrMessageIndex = m;
                return true;
            }
        }
    }
    return false;
}






void NetworkManager::receive() {
    RawPacket raw;
    raw.data = recvBuffer;
    while (transportLayer.receive(raw)) {
        SessionPacket session;
        if (sessionLayer.process(raw, session)) {
            pushEvent(NetworkEventType::Connected, session.peerID, -1);
        }

        fragmentLayer.processFragment(session);
    }
}

bool NetworkManager::pullMessage(MessageView& out) {
    while (true) {
        if (!messageLayer.pull(out)) {
            SessionPacket sessionPacket;
            if (!fragmentLayer.pullPacket(sessionPacket)) return false;

            Packet packet;
            if (!packetLayer.process(sessionPacket, packet)) return false;
            
            messageLayer.begin(packet);
            continue;
        }

        ProtocolSideEffect side;
        ProcessResult action = protocolLayer.process(out, side);
        // printf("protocolLayer size: %d, side.reliableDelivered %d, side.sendConfirm %d\n", out.header.size, side.reliableDelivered, side.sendConfirm);
        if (side.reliableDelivered) {
            pushEvent(NetworkEventType::ReliableDelivered, side.deliveredPeerID, side.deliveredReliableID);
            side.reliableDelivered = false;
        }
        if (side.sendConfirm) {
            HeaderBuffer headerBuffer;
            messageLayer.buildHeader(0, 0, headerBuffer.message);
            protocolLayer.buildHeader(headerBuffer.message, MessageType::Confirm, -1, nullptr, 0, headerBuffer.protocol);
            headerBuffer.protocol.reliableId = side.deliveredReliableID;

            packetLayer.writeToPeer(out.senderID, (char*)&headerBuffer, sizeof(HeaderBuffer), nullptr, 0);
            side.sendConfirm = false;
        }
        // printf("Message perfin size: %d\n", out.header.size);
        if (action == ProcessResult::Consume) continue;
        // printf("Message final size: %d\n", out.header.size);
        return true;
    }
}

void NetworkManager::pushMessage(const char* data, int size, char userData) {
    HeaderBuffer headerBuffer;
    messageLayer.buildHeader(size, userData, headerBuffer.message);
    protocolLayer.buildHeader(headerBuffer.message, MessageType::None, -1, data, size, headerBuffer.protocol);

    packetLayer.writeGlobal((char*)&headerBuffer, sizeof(HeaderBuffer), (char*)data, size);
}

void NetworkManager::pushDirectMessage(const char* data, int size, char userData, int peerID) {
    HeaderBuffer headerBuffer;
    messageLayer.buildHeader(size, userData, headerBuffer.message);
    protocolLayer.buildHeader(headerBuffer.message, MessageType::None, peerID, data, size, headerBuffer.protocol);

    packetLayer.writeToPeer(peerID, (char*)&headerBuffer, sizeof(HeaderBuffer), (char*)data, size);
}

int NetworkManager::pushReliableMessage(const char* data, int size, char userData, int peerID) {
    HeaderBuffer headerBuffer;
    messageLayer.buildHeader(size, userData, headerBuffer.message);
    protocolLayer.buildHeader(headerBuffer.message, MessageType::Reliable, peerID, data, size, headerBuffer.protocol);

    packetLayer.writeToPeer(peerID, (char*)&headerBuffer, sizeof(HeaderBuffer), (char*)data, size);
    return headerBuffer.protocol.reliableId;
}

int NetworkManager::pushReliableMessage(const char* data, int size, char userData, const char* id, int port) {
    bool isNew;
    int peerID = sessionLayer.createPeer(id, port, isNew);
    if(isNew) pushEvent(NetworkEventType::Connected, peerID, -1);
    return pushReliableMessage(data, size, userData, peerID);
}

void NetworkManager::send() {
    Span<directComponent> directs = ecs.GetComponents<directComponent>();
    for (auto& direct: directs) {
        int peerID = ecs.GetEntity(direct);
        
        OutSessionPacket outSessionPacket;
        if(!packetLayer.build(peerID, outSessionPacket)) continue;

        fragmentLayer.setOutSessionPacket(outSessionPacket);
        
        OutSessionPacket frag;
        while (fragmentLayer.getFragmentPacket(frag)) {
            OutPacket netPacket;
            sessionLayer.process(frag, netPacket);
            transportLayer.send(netPacket);
        }

        direct.directSection.clear();
    }
    packetLayer.clearGlobal();
}

void NetworkManager::update() {
    int peerToDelete;
    while(sessionLayer.popPeer(peerToDelete)) {
        pushEvent(NetworkEventType::Disconnected, peerToDelete, -1);
        ecs.destroy(peerToDelete);
    }

    protocolLayer.beginResendCycle();
    ProtocolMessage protocolMessage;
    while(protocolLayer.popResend(protocolMessage)) {
        if(protocolMessage.attemps > RELIABLE_MAX_ATTEMPS) {
            ecs.destroy(protocolMessage.sendToPeerID);
            continue;
        }
        pushEvent(NetworkEventType::ReliableFailed, protocolMessage.sendToPeerID, protocolMessage.protocol.reliableId);

        HeaderBuffer headerBuffer;
        headerBuffer.message = protocolMessage.message;
        headerBuffer.protocol = protocolMessage.protocol;
        packetLayer.writeToPeer(protocolMessage.sendToPeerID, (char*)&headerBuffer, sizeof(HeaderBuffer), (char*)protocolMessage.data, protocolMessage.size);
    }

    // int count = 0;
    // for(int i = 0; i < 32; i++) {
    //     if(fragmentLayer.slots[i].packetId != -1) count++;
    // }
    // if(count > 0) printf(" fragmentLayer.slots.size() %d\n", count);
}

bool NetworkManager::pullEvent(NetworkEvent& out) {
    if(events.empty()) return false;

    out = std::move(events.front());
    events.pop_front();
    return true; 
}

void NetworkManager::pushEvent(NetworkEventType type, int peerID, int reliableID) {
    events.push_back({type, peerID, reliableID});
}