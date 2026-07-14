#pragma once
#include "GraphicsEngine.h"
#include "Mesh.h"

class MeshManager
{
public:
    inline static MeshManager *getPtr() { return meshManager; }

    inline static int addMesh(const Mesh &mesh)
    {
        getPtr()->indicies.push_back(GraphicsEngine::createIndexArrayObject({(unsigned int *)mesh.index, (unsigned int)mesh.index_size, mesh.number_of_materials, mesh.materials}));
        getPtr()->vertexes.push_back(GraphicsEngine::createVertexArrayObject({mesh.vertex, sizeof(Vertex), (unsigned int)mesh.vertex_size}));
        return getPtr()->vertexes.size() - 1;
    }

    inline static int setMeshById(int id)
    {
        GraphicsEngine::setVertexArrayObject(getPtr()->vertexes[id]);
        GraphicsEngine::setIndexArrayObject(getPtr()->indicies[id]);
        return getPtr()->indicies[id]->getNumberOfMaterials();
    }

    inline static unsigned getNumberOfPolygonsByMaterialID(int meshID, int materalID)
    {
        return getPtr()->indicies[meshID]->getMaterialSize(materalID);
    }

private:
    static MeshManager *meshManager;

    std::vector<VertexArrayObject *> vertexes;
    std::vector<IndexArrayObject *> indicies;
};