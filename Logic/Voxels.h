#pragma once
#include "ECS.h"

struct VoxelGroup {
    bool voxels[8][8][8]{};
    int count = 0;
};

struct Voxels : public Component {
	int data[16];
	Vector3 minP;
	Vector3 maxP;
	bool destructible = false;
	bool blowable = false;

	Voxels() {
		destructible = false;
		memset(data, 0xff, sizeof(data));
		minP = Vector3(-0.31f, -0.31f, -0.31f);
		maxP = Vector3( 0.31f,  0.31f,  0.31f);
	}

	inline void setVoxel(int x, int y, int z, bool value) {
        int index = x + y * 8 + z * 64;
        int word = index >> 5;
        int bit  = index & 31;
        if (value) data[word] |= (1u << bit);
        else data[word] &= ~(1u << bit);
    }

	inline bool isVoxel(int x, int y, int z) {
		int index = x + y * 8 + z * 64;
        int word = index >> 5;
        int bit  = index & 31;
		return (data[word] & (1u << bit)) != 0;
	}

	std::vector<VoxelGroup> findComponents();
	void explodeVoxels(Vector3 hit, float radius);
    void fireVoxels(Vector3 source, Vector3 direction);
};



#define shardsCount 450

class ShardsManager {
public:
	inline static ShardsManager& getInstance() {
		static ShardsManager instance;
		return instance;
	}

	void init();
	Object createShard(int meshID, int materal, Vector3 size, int x, int y, int z);
private:
	int startID;
	int endID;
	int start = 0;
};
