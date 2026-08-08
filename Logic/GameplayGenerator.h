#pragma once
#include <cstdint>

struct SOclass {
    int id;
    uint32_t mask;
};

std::string binary(unsigned int x) {
    std::string result;
    for (int bit = 31; bit >= 0; --bit) {
        result += ((x >> bit) & 1) ? '1' : '0';
    }
    return result;
}

inline std::vector<int> sortAndGroupSolutions(std::vector<SOclass>& solutions) {
    constexpr int BITS = 32;

    int solutionSize = solutions.size();
    std::vector<int> counter(solutionSize, 0);
    for(int i = 0; i < solutionSize; ++i) {
        int solMask = solutions[i].mask;
        for (int bit = 0; bit < BITS; bit++) {
            if (solMask & (1u << bit)) {
                counter[i]++;
            }
        }
    }

    int mask = 0;
    int sortedEnd = 0;
    std::vector<int> groupStarts;
    groupStarts.reserve(solutionSize);
    while(sortedEnd < solutionSize) {
    // for(int k = 0; k < 11; ++k) {
        //findmin counter
        int minSolCounter = 256;
        int minSolBit = 0;
        for (int i = sortedEnd; i < solutionSize; ++i) {
            int solCount = counter[i];
            // printf("%d ", solCount);
            if(solCount > 0 && solCount < minSolCounter) {
                minSolCounter = solCount;
                minSolBit = i;
            }
        }

        //create min group
        int foundMask = solutions[minSolBit].mask;
        int searchMask = (mask & foundMask) ^ foundMask; 
        // printf("\ncouner: %d(index %d)\n%s\n%s\n%s\n", minSolCounter, minSolBit, binary(foundMask).c_str(), binary(searchMask).c_str(), binary(mask).c_str());
        mask |= foundMask;
        // printf("sortedEnd %d(of %d)\n", sortedEnd, solutionSize);

        groupStarts.push_back(sortedEnd);
        for (int i = sortedEnd; i < solutionSize;) {
            int value = solutions[i].mask;
            unsigned int common = value & searchMask;
            if (common) {
                int c = counter[i];
                for (int bit = 0; bit < BITS; bit++) {
                    if (common & (1u << bit)) {
                        counter[i]--;
                    }
                }

                // printf("%s %d\n%s %d %d %d\n", binary(value).c_str(), solutions[i].id, binary(common).c_str(), i, c, counter[i]);
                if(counter[i] == 0) {
                    std::swap(solutions[i], solutions[sortedEnd]);
                    std::swap(counter[i], counter[sortedEnd]);

                    sortedEnd++;
                }
            }
            ++i;
        }
    }
    // printf("\n");
    return groupStarts;
}

inline std::vector<int> sortAndGroupObstacles(std::vector<SOclass>& obstacles, std::vector<SOclass>& solutions, std::vector<int>& solutionsGroupStarts) {
    constexpr int BITS = 32;
    const int obstaclesSize = obstacles.size();
    const int groupsCount = solutionsGroupStarts.size();
    std::vector<int> obstacleGroupStarts;
    obstacleGroupStarts.reserve(groupsCount);
    std::vector<int> position(obstaclesSize);
    std::vector<int> atPosition(obstaclesSize);
    for (int i = 0; i < obstaclesSize; ++i) {
        position[i] = i;
        atPosition[i] = i;
    }

    int sortedEnd = 0;
    uint32_t mask = 0;

    for (int g = 0; g < groupsCount; ++g) {
        obstacleGroupStarts.push_back(sortedEnd);
        int solBegin = solutionsGroupStarts[g];
        int solEnd = (g + 1 < groupsCount) ? solutionsGroupStarts[g + 1] : static_cast<int>(solutions.size());
        uint32_t groupMask = 0;
        for (int i = solBegin; i < solEnd; ++i) {
            groupMask |= static_cast<uint32_t>(solutions[i].mask);
        }
        uint32_t searchMask = groupMask & ~mask;
        mask |= groupMask;
        for (int bit = 0; bit < BITS; ++bit) {
            if (!(searchMask & (1u << bit))) {
                continue;
            }

            int from = position[bit];
            int to = sortedEnd;

            if (from != to) {
                int otherBit = atPosition[to];
                std::swap(obstacles[from], obstacles[to]);
                position[bit] = to;
                position[otherBit] = from;
                atPosition[to] = bit;
                atPosition[from] = otherBit;
            }

            ++sortedEnd;
        }
    }
    if (sortedEnd < obstaclesSize) {
        obstacleGroupStarts.push_back(sortedEnd);
    }
    return obstacleGroupStarts;
}