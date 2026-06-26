#pragma once
#include <algorithm>
#include "Transform.h"
#include "ECS.h"
#include "DirtyValue.h"

#define UI_BOTTOM_LEFT  0
#define UI_BOTTOM       1
#define UI_BOTTOM_RIGHT 2
#define UI_LEFT         3
#define UI_CENTER       4
#define UI_RIGHT        5
#define UI_TOP_LEFT     6
#define UI_TOP          7
#define UI_TOP_RIGHT    8

struct UIElement : public Component
{
public:
    int texture_index = 0;
    Color color = Color(1, 1, 1, 1);
    char anchor = UI_CENTER;
};


// private:
//     short zOrder;
//     friend class UIManager;
//     inline void setZOrder(short newZOrder)
//     {
//         ECS::GetUIManager().setZOrder(object.getID(), newZOrder);
//     }
//     inline void setGroup(short newGroup)
//     {
//         ECS::GetUIManager().setGroup(object.getID(), newGroup);
//     }
//     inline short getZOrder()
//     {
//         return zOrder;
//     }

// class UIManager
// {
// public:
//     void add(int objectID, short group, short zOrder)
//     {
//         if (group >= groups.size())
//         {
//             groups.resize(group + 1);
//             dirty.resize(group + 1, true);
//         }
//         if (objectID >= objectInfo.size())
//             objectInfo.resize(objectID + 1, {-1, -1});
//         objectInfo[objectID] = {group, (short)groups[group].size()};
//         groups[group].push_back(objectID);
//         dirty[group] = true;
//     }

//     void setGroup(int objectID, short newGroup)
//     {
//         auto [oldGroup, index] = objectInfo[objectID];
//         if (oldGroup == newGroup)
//             return;

//         if (newGroup >= groups.size())
//         {
//             groups.resize(newGroup + 1);
//             dirty.resize(newGroup + 1, true);
//         }

//         // remove from old (swap remove)
//         auto lastID = groups[oldGroup].back();
//         groups[oldGroup][index] = lastID;
//         objectInfo[lastID].second = index;
//         groups[oldGroup].pop_back();

//         // add to new
//         objectInfo[objectID] = {newGroup, (short)groups[newGroup].size()};
//         groups[newGroup].push_back(objectID);

//         dirty[oldGroup] = true;
//         dirty[newGroup] = true;
//     }

//     void setZOrder(int objectID, short newZ)
//     {
//         auto &e = ECS::GetComponent<UIElement>(objectID);
//         e.zOrder = newZ;
//         auto [group, index] = objectInfo[objectID];
//         dirty[group] = true;
//     }

//     std::vector<int> &getGroup(short group)
//     {
//         if (dirty[group])
//         {
//             auto &g = groups[group];
//             std::sort(g.begin(), g.end(), [&](int a, int b)
//                       { return ECS::GetComponent<UIElement>(a).zOrder < ECS::GetComponent<UIElement>(b).zOrder; });
//             dirty[group] = false;
//         }
//         return groups[group];
//     }

// private:
//     std::vector<std::vector<int>> groups;
//     std::vector<std::pair<short, short>> objectInfo; // objectID -> (group, index in group)
//     std::vector<bool> dirty;
// };
