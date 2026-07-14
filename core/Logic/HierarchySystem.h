#pragma once
#include "stdlibInclude.h"

class HierarchySystem
{
public:
    void addObject(int objectID);
    void setParent(int childID, int parentID);
    void updateDepth(int root);
    std::pair<int *, int> rebuild();
    inline std::pair<int *, int> getParents() { return { parents.data(), (int)parents.size() }; }
    inline int getParent(int objectID) { return parents[objectID]; }
    const std::vector<int>& getChildren(int objectID) const;

private:
    std::vector<int> parents;
    std::vector<std::vector<int>> children;
    std::vector<int> depth;
    std::vector<int> sortedByLevels;
    bool dirty = true;
};

inline const std::vector<int>& HierarchySystem::getChildren(int objectID) const {
    static const std::vector<int> empty;
    if (objectID < 0 || objectID >= children.size()) return empty;
    return children[objectID];
}

inline void HierarchySystem::addObject(int objectID)
{
    if (parents.size() <= objectID)
    {
        parents.resize(objectID + 1, -1);
    }
    if (children.size() <= objectID)
    {
        children.resize(objectID + 1);
    }
    if (depth.size() <= objectID)
    {
        depth.resize(objectID + 1, 0);
    }
    parents[objectID] = -1;
    children[objectID].clear();
    depth[objectID] = 0;
    dirty = true;
}
inline void HierarchySystem::setParent(int childID, int parentID)
{
    dirty = true;
    int oldParent = parents[childID];
    if (oldParent != -1)
    {
        auto &vec = children[oldParent];
        for (int i = 0; i < vec.size(); i++)
        {
            if (vec[i] == childID)
            {
                vec[i] = vec.back();
                vec.pop_back();
                break;
            }
        }
    }
    parents[childID] = parentID;
    if (parentID != -1)
    {
        children[parentID].push_back(childID);
    }
    updateDepth(childID);
}
inline void HierarchySystem::updateDepth(int root)
{
    std::stack<int> st;
    st.push(root);
    while (!st.empty())
    {
        int v = st.top();
        st.pop();
        int p = parents[v];
        depth[v] = (p == -1 ? 0 : depth[p] + 1);
        for (int c : children[v])
            st.push(c);
    }
}
inline std::pair<int *, int> HierarchySystem::rebuild()
{
    if (!dirty)
    {
        return {sortedByLevels.data(), (int)sortedByLevels.size()};
    }
    std::vector<int> levelsCounter;
    levelsCounter.resize(1);
    for (int i = 0; i < depth.size(); i++)
    {
        if (depth[i] >= levelsCounter.size())
        {
            levelsCounter.resize(depth[i] + 1, 0);
        }
        ++levelsCounter[depth[i]];
    }
    for (int i = 1; i < levelsCounter.size(); i++)
    {
        levelsCounter[i] += levelsCounter[i - 1];
    }
    sortedByLevels.resize(depth.size());
    for (int i = 0; i < depth.size(); i++)
    {
        int d = depth[i];
        int pos = --levelsCounter[d];
        sortedByLevels[pos] = i;
    }
    dirty = false;
    return {sortedByLevels.data(), (int)sortedByLevels.size()};
}