#include "Voxels.h"
#include "Physic.h"

std::vector<VoxelGroup> Voxels::findComponents() {
    bool visited[8][8][8]{};
    std::vector<VoxelGroup> result;
    int dirs[6][3] = {
        { 1,0,0},
        {-1,0,0},
        {0, 1,0},
        {0,-1,0},
        {0,0, 1},
        {0,0,-1}
    };
    for(int z=0; z<8; z++)
    for(int y=0; y<8; y++)
    for(int x=0; x<8; x++) {
        if(visited[x][y][z]) continue;
        if(!isVoxel(x,y,z)) continue;
        VoxelGroup group;
        int queue[512][3];
        int head = 0;
        int tail = 0;
        queue[tail][0]=x;
        queue[tail][1]=y;
        queue[tail][2]=z;
        tail++;
        visited[x][y][z]=true;
        while(head < tail) {
            int cx=queue[head][0];
            int cy=queue[head][1];
            int cz=queue[head][2];
            head++;
            group.voxels[cx][cy][cz]=true;
            group.count++;
            for(auto& d:dirs) {
                int nx=cx+d[0];
                int ny=cy+d[1];
                int nz=cz+d[2];
                if(nx<0||nx>=8|| ny<0||ny>=8|| nz<0||nz>=8) continue;
                if(visited[nx][ny][nz]) continue;
                if(!isVoxel(nx,ny,nz)) continue;
                visited[nx][ny][nz]=true;
                queue[tail][0]=nx;
                queue[tail][1]=ny;
                queue[tail][2]=nz;
                tail++;
            }
        }
        result.push_back(group);
    }
    return result;
}


void Voxels::explodeVoxels(Vector3 hit, float radius) {
    Vector3 size = { (maxP.x - minP.x) / 8.0f, (maxP.y - minP.y) / 8.0f, (maxP.z - minP.z) / 8.0f };
    bool remove[8][8][8] = {};
    for(int z = 0; z < 8; z++) {
        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++) {
                if(!isVoxel(x,y,z)) continue;
                Vector3 voxelPos( minP.x + (x + 0.5f) * size.x, minP.y + (y + 0.5f) * size.y, minP.z + (z + 0.5f) * size.z );
                Vector3 delta = voxelPos - hit;
                float dist = delta.sqrMagnitude();
                if(dist > radius * radius) continue;
                Vector3 dir = -delta.normalized();
                int dx = 0;
                int dy = 0;
                int dz = 0;
                float ax = fabs(dir.x);
                float ay = fabs(dir.y);
                float az = fabs(dir.z);
                if(ax > ay && ax > az) dx = dir.x > 0 ? 1 : -1;
                else if(ay > az) dy = dir.y > 0 ? 1 : -1;
                else dz = dir.z > 0 ? 1 : -1;
                int nx = x + dx;
                int ny = y + dy;
                int nz = z + dz;
                bool surface = false;
                if(nx < 0 || nx >= 8 || ny < 0 || ny >= 8 || nz < 0 || nz >= 8) {
                    surface = true;
                }
                else if(!isVoxel(nx, ny, nz)) {
                    surface = true;
                }
                if(surface) {
                    float facing = Vector3::Dot(Vector3(dx,dy,dz), dir);
                    if(facing > 0.5f)  remove[x][y][z] = true;
                }
            }
        }
    }

    for(int z = 0; z < 8; z++)
        for(int y = 0; y < 8; y++)
            for(int x = 0; x < 8; x++)
                if(remove[x][y][z]) {
                    setVoxel(x,y,z,false);
                    if(object.HasComponent<RenderView>()) {
                        RenderView& rv = object.GetComponent<RenderView>();
                        Object shard = ShardsManager::getInstance().createShard(rv.mesh_index, rv.materals[0], size, x, y, z);
                        Vector3 offset = minP + Vector3( (x + 0.5f) * size.x, (y + 0.5f) * size.y, (z + 0.5f) * size.z );
                        shard.transform.position = object.transform.position;
                        shard.GetComponent<SphereCollider>().offset = offset;
                        shard.GetComponent<Rigidbody>().velocity = object.transform.rotation * offset * 70 * Time::fixedDeltaTime;
                    }
                }
    
    auto parts = findComponents();
    if(parts.size() <= 1) return;

    std::sort(parts.begin(), parts.end(), [](const VoxelGroup& a, const VoxelGroup& b) {
        return a.count > b.count;
    });
    
    memset(data,0,sizeof(data));
    for(int z=0; z<8; z++)
    for(int y=0; y<8; y++)
    for(int x=0; x<8; x++) {
        if(parts[0].voxels[x][y][z])
            setVoxel(x,y,z,true);
    }

    for(size_t i = 1; i < parts.size(); i++) {
        if(parts[i].count <= 4) continue;
        Object chunk = ECS::cloneObject(object);
        Voxels& chunkVoxels = ECS::GetComponent<Voxels>(chunk.getID());
        memset(chunkVoxels.data, 0, sizeof(chunkVoxels.data));
        for(int z=0; z<8; z++)
        for(int y=0; y<8; y++)
        for(int x=0; x<8; x++)
        {
            if(parts[i].voxels[x][y][z]) chunkVoxels.setVoxel(x,y,z,true);
        }
    }
}

void Voxels::fireVoxels(Vector3 source, Vector3 direction) {
    Vector3 size = { (maxP.x - minP.x) / 8.0f, (maxP.y - minP.y) / 8.0f, (maxP.z - minP.z) / 8.0f };
    
    float tmin = 0.0f;
    float tmax = 1e9f;
    for (int i = 0; i < 3; i++) {
        float s = source[i];
        float d = direction[i];
        float mn = minP[i];
        float mx = maxP[i];
        if (fabs(d) < 1e-6f) {
            if (s < mn || s > mx) return; 
        }
        else {
            float t1 = (mn - s) / d;
            float t2 = (mx - s) / d;
            if (t1 > t2) std::swap(t1, t2);
            tmin = Math::Max(tmin, t1);
            tmax = Math::Min(tmax, t2);
            if (tmin > tmax) return;
        }
    }
    
    float step = Math::Min(size.x, Math::Min(size.y, size.z)) * 0.25f;
    for (float t = tmin; t <= tmax; t += step) {
        Vector3 p = source + direction * t;
        int x = (int)((p.x - minP.x) / size.x);
        int y = (int)((p.y - minP.y) / size.y);
        int z = (int)((p.z - minP.z) / size.z);
        if(x < 0 || x >= 8 ||
           y < 0 || y >= 8 ||
           z < 0 || z >= 8) continue;
        if(isVoxel(x, y, z)) {
            setVoxel(x, y, z, false);
            if(object.HasComponent<RenderView>()) {
                RenderView& rv = object.GetComponent<RenderView>();
                Object shard = ShardsManager::getInstance().createShard(rv.mesh_index, rv.materals[0], size, x, y, z);
                Vector3 offset = minP + Vector3( (x + 0.5f) * size.x, (y + 0.5f) * size.y, (z + 0.5f) * size.z );
                shard.transform.position = object.transform.position;
                shard.GetComponent<SphereCollider>().offset = offset;
                shard.GetComponent<Rigidbody>().velocity = object.transform.rotation * offset * 700 * Time::fixedDeltaTime;
            }
        }
    }


    auto parts = findComponents();
    if(parts.size() <= 1) return;

    std::sort(parts.begin(), parts.end(), [](const VoxelGroup& a, const VoxelGroup& b) {
        return a.count > b.count;
    });
    
    memset(data,0,sizeof(data));
    for(int z=0; z<8; z++)
    for(int y=0; y<8; y++)
    for(int x=0; x<8; x++) {
        if(parts[0].voxels[x][y][z]) {
            setVoxel(x,y,z,true);
        }
    }

    for(size_t i = 1; i < parts.size(); i++) {
        if(parts[i].count <= 4) continue;
        Object chunk = ECS::cloneObject(object);
        Voxels& chunkVoxels = ECS::GetComponent<Voxels>(chunk.getID());
        memset(chunkVoxels.data, 0, sizeof(chunkVoxels.data));
        for(int z=0; z<8; z++)
        for(int y=0; y<8; y++)
        for(int x=0; x<8; x++)
        {
            if(parts[i].voxels[x][y][z]) chunkVoxels.setVoxel(x,y,z,true);
        }
    }
}

void ShardsManager::init() {
    start = 0;
    for(int i = 0; i < shardsCount; i++) {
        Object shard = ECS::createObject();
        shard.transform.position = Vector3(-110, -110, i * 3);
        int id = shard.getID();
        if(i == 0) startID = id;
        else if(i == shardsCount - 1) endID = id; 
        shard.AddComponent<RenderView>();
        shard.AddComponent<SphereCollider>();
        shard.AddComponent<Voxels>().destructible = false;
        shard.AddComponent<Rigidbody>().isKinematic = false;
    }
}

Object ShardsManager::createShard(int meshID, int materal, Vector3 size, int x, int y, int z) {
    Object shard = ECS::getObjectByID(startID + start);
    start++;
    if(start >= shardsCount) start = 0;
    // shard.transform.scale = size;
    shard.GetComponent<RenderView>().mesh_index = meshID;
    shard.GetComponent<RenderView>().materals[0] = materal;
    shard.GetComponent<RenderView>().materals[1] = materal;
    shard.GetComponent<SphereCollider>().radius = size.x / 2;
    memset(shard.GetComponent<Voxels>().data, 0, 16 * sizeof(int));
    shard.GetComponent<Voxels>().setVoxel(x, y, z, true);
    return shard;
}