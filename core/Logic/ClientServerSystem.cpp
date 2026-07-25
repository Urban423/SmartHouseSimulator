#include "ClientServerSystem.h"
#include "Physic.h"
#include "prefabs.h"

void ClientServerSystem::Start() {
    
}

struct SnapshotHeader {
    int tick;
    int entityCount;
};

struct SnapshotPacket {
    std::vector<char> data;
};

SnapshotPacket buildSnapshot(long long mask) {
    SnapshotPacket snap;
    snap.data.clear();
    Span<NetworkIdentity> identities = ECS::GetComponents<NetworkIdentity>();

    SnapshotHeader header;
    header.tick = Time::tick;
    header.entityCount = identities.size();

    snap.data.insert(snap.data.end(), (char*)&header, (char*)&header + sizeof(header));
    for (int i = 0; i < identities.size(); i++) {
        Object entity = identities[i].object;
        append(snap.data, identities[i].getID());
        ECS::SerializeEntity(entity, snap.data, mask);
    }

    return std::move(snap);
}



void dumpHex(const char* data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%02X ", (unsigned char)data[i]);

        if ((i + 1) % 16 == 0)
            printf("\n");
    }

    if (size % 16 != 0)
        printf("\n");
}





void ClientServerSystem::handleHostMessage(MessageView& msg) {
    switch ((MSGType)msg.header.type) {

        case MSGType::Connect: {
            Object newPlayer = PrefabSystem::getInstance().createPlayer(false);
            printf("Connect \n");
            int playerId = newPlayer.GetComponent<NetworkIdentity>().getID();
            NetworkManager::getInstance().pushReliableMessage((char*)&playerId, sizeof(playerId), (char)MSGType::ConnectAccepted, msg.senderID);
            break;
        }

        case MSGType::RequestScene: {
            SnapshotPacket snapshot = buildSnapshot(~0ULL);
            // dumpHex(snapshot.data.data(), snapshot.data.size());
            printf("RequestScene size %d\n", snapshot.data.size());
            NetworkManager::getInstance().pushReliableMessage((char*)snapshot.data.data(), snapshot.data.size(), (char)MSGType::LoadScene, msg.senderID);
            break;
        }

        case MSGType::Input: {
            NetInput ni;
            memcpy(&ni, msg.data, sizeof(NetInput));
            Input input = ToInput(ni);
            Span<NetworkIdentity> identities = ECS::GetComponents<NetworkIdentity>();
            for (auto& n : identities) {
                if (n.getID() != ni.netID) continue;
                if (n.object.HasComponent<InputComponent>()) {
                    n.object.GetComponent<InputComponent>().input = input;
                    n.object.transform.rotation = ni.rotation;
                }
                break;
            }
            break;
        }

        default:
            break;
    }
}


void ClientServerSystem::handleClientMessage(MessageView& msg) {
    switch ((MSGType)msg.header.type) {

        case MSGType::ConnectAccepted: {
            printf("ConnectAccepted");
            memcpy(&clientID, msg.data, sizeof(int));
            printf("ConnectAccepted \n");
            NetworkManager::getInstance().pushReliableMessage(nullptr, 0, (char)MSGType::RequestScene, msg.senderID);
            break;
        }

        case MSGType::LoadScene: {
            printf("LoadScene \n");
            Span<NetworkIdentity> identities = ECS::GetComponents<NetworkIdentity>();
            for (int i = 0; i < identities.size(); i++) {
                ECS::deleteObject(identities[i].object);
            }
            // dumpHex(msg.data, msg.header.size);

            Span<char> buffer(msg.data, msg.header.size);
            size_t offset = 0;

            SnapshotHeader header;
            read(buffer, offset, header);

            int netID;
            bool found = false;
            for(int i = 0; i < header.entityCount; i++) {
                Object newObject = ECS::createObject();
                read(buffer, offset, netID);
                ECS::DeserializeEntity(newObject, buffer, offset);
                if (netID == clientID) {
                    localPlayer = newObject;
                    found = true;
                }
            }

            if (found) {
                PrefabSystem::getInstance().createCamera(localPlayer);
                netState = NetState::InGame;
            }
            break;
        }

        case MSGType::Snapshot: { //read
            // printf("Snapshot size: %d \n", msg.header.size);
            if(netState != NetState::InGame) break;

            Span<NetworkIdentity> identities = ECS::GetComponents<NetworkIdentity>();
            netToEntity.clear();
            netToEntity.init<int>();
            for(int i = 0; i < identities.size(); ++i) {
                netToEntity.add<int>(identities[i].getID()) = identities[i].object.getID();
            }

            Span<char> buffer(msg.data, msg.header.size);
            size_t offset = 0;

            SnapshotHeader header;
            read(buffer, offset, header);

            Quaternion playerRot;
            if(localPlayer.valid()) playerRot = localPlayer.transform.rotation;
            int netID;
            Object entity;
            for(int i = 0; i < header.entityCount; i++) {
                read(buffer, offset, netID);
                if(netToEntity.hasByID(netID)) entity = ECS::getObjectByID(netToEntity.getByID<int>(netID));
                else   entity = ECS::createObject();
                ECS::DeserializeEntity(entity, buffer, offset);
            }
            if(localPlayer.valid()) localPlayer.transform.rotation = playerRot;
            break;
        }

        default:
            break;
    }
}








void ClientServerSystem::FixedUpdate() {
    if(isOffline()) return;

    NetworkEvent networkEvent;
    while(NetworkManager::getInstance().pullEvent(networkEvent)) {

        const char* types[] = {
            "connected",
            "Disconnected",
            "ReliableDelivered",
            "ReliableFailed",
        };
        printf("event type: %s\n", types[(int)networkEvent.type]);
        switch (networkEvent.type)
        {
            case(NetworkEventType::Connected): {
                break;
            }

            case(NetworkEventType::Disconnected): {
                break;
            }

            case(NetworkEventType::ReliableDelivered): {
                if(networkEvent.reliableID == connectMessageID) {
                    setState(NetState::LoadingScene);
                }
                break;
            }
            
            default: break;
        }
    }

    MessageView msg;
    while (NetworkManager::getInstance().pullMessage(msg)) {
        if (networkMode == NetworkMode::Host) {
            handleHostMessage(msg);
        }
        else if (networkMode == NetworkMode::Client) {
            handleClientMessage(msg);
        }
    }

    // //send
    if(isHost()) {
        long long mask =
            (1u << ECS::GetComponentID<Rigidbody>()) |
            (1u << ECS::GetComponentID<Transform>());
        
        SnapshotPacket snapshot = buildSnapshot(mask);
        NetworkManager::getInstance().pushMessage((char*)snapshot.data.data(), snapshot.data.size(), (char)MSGType::Snapshot);
    } else {
        if(netState != NetState::InGame) return;
        if(OverlayManager::IsPaused()) return;
        NetInput ni = ToNetInput(IOSystem::getInput(), localPlayer.transform.rotation);
        ni.netID = localPlayer.GetComponent<NetworkIdentity>().getID();
        NetworkManager::getInstance().pushMessage((char*)&ni, sizeof(NetInput), (char)MSGType::Input);
    }
}

void ClientServerSystem::host(int port) {
    if (networkMode != NetworkMode::Offline) disconect();
    auto& network = NetworkManager::getInstance();
    network.open();
    network.bind(port);
    networkMode = NetworkMode::Host;
    setState(NetState::Idle);
}

void ClientServerSystem::connect(const char* ip, int port) {
    if (networkMode != NetworkMode::Offline) disconect();
    auto& network = NetworkManager::getInstance();
    network.open();
    connectMessageID = network.pushReliableMessage(nullptr, 0, (char)MSGType::Connect, ip, port);
    networkMode = NetworkMode::Client;
    setState(NetState::Connecting);
}

void ClientServerSystem::disconect() {
    if (networkMode == NetworkMode::Offline) return;
    NetworkManager::getInstance().close();
    setState(NetState::Idle);
    networkMode = NetworkMode::Offline;
}
