#pragma once
#include "Mesh.h"

class MeshManager {
public:
    inline static MeshManager& getInstance() { 
        static MeshManager meshManager;
        return meshManager; 
    }

    inline static int addMesh(Mesh&& mesh) {
        getInstance().meshes.emplace_back(std::move(mesh));
        return getInstance().meshes.size() - 1;
    }

    inline static int addMesh(Mesh &mesh) {
        getInstance().meshes.emplace_back(std::move(mesh));
        return getInstance().meshes.size() - 1;
    }

    inline static Mesh& getMeshByID(const int id) {
        return getInstance().meshes[id];
    } 

    inline static int setMeshById(int id) {
        getInstance().meshes[id].setMeshOnPipeline();
        return getInstance().meshes[id].getNumberOfMaterials();
    }

    inline static unsigned int getNumberOfPolygonsByMaterialID(int meshID, int materalID) {
        return getInstance().meshes[meshID].getMaterialSize(materalID);
    }
private:
    std::vector<Mesh> meshes;
};