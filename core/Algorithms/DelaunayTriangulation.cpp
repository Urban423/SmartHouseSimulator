#include "Triangulation.h"
#include <algorithm>
#include <optional>
#include <cstdio>
#include <cmath>
#include <queue>
#include <list>
#include <stdio.h>

// void* operator new(std::size_t sz) {
//     static int allocs = 0;
//     ++allocs;
//     fprintf(stderr, "Alloc #%d size %zu\n", allocs, sz);

//     if (void* ptr = std::malloc(sz)) {
//         return ptr;
//     }
//     throw std::bad_alloc();
// }

struct EventCompare {
    bool operator()(const Event* a, const Event* b) const {
        if (a->point.y != b->point.y) return a->point.y < b->point.y;
        if (a->type != b->type) return a->type == EventType::Circle;
        return a->point.x < b->point.x;
    }
};

double intersectX(const Arc& a, const Arc& b, double ly) {
    const Vector2& p = a.site;
    const Vector2& q = b.site;
    if (p.y == q.y) return (p.x + q.x) * 0.5;
    if (p.y == ly) return p.x;
    if (q.y == ly) return q.x;

    double z1 = 2.0 * (p.y - ly);
    double z2 = 2.0 * (q.y - ly);

    double a1 = 1.0 / z1;
    double a2 = 1.0 / z2;

    double b1 = -2.0 * p.x / z1;
    double b2 = -2.0 * q.x / z2;

    double c1 = (p.x * p.x + p.y * p.y - ly * ly) / z1;
    double c2 = (q.x * q.x + q.y * q.y - ly * ly) / z2;

    double A = a1 - a2;
    double B = b1 - b2;
    double C = c1 - c2;

    double disc = B * B - 4 * A * C;
    if (disc < 0) disc = 0;

    double x1 = (-B + std::sqrt(disc)) / (2 * A);
    double x2 = (-B - std::sqrt(disc)) / (2 * A);
    return (p.y < q.y) ? std::max(x1, x2) : std::min(x1, x2);
}

std::list<Arc>::iterator findArcAbove(std::list<Arc>& beachline, double x, double sweepY ) {
    auto it = beachline.begin();
    auto next = std::next(it);
    if (next == beachline.end()) return it;
    for (; next != beachline.end(); ++it, ++next) {
        double breakX = intersectX(*it, *next, sweepY);
        if (x < breakX)
            return it;
    }
    return it; 
}

std::optional<Vector2> circleBottom(const Vector2& a, const Vector2& b, const Vector2& c) {
    double det = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
    if (det >= 0) return std::nullopt;
    
    double A = b.x - a.x, B = b.y - a.y;
    double C = c.x - a.x, D = c.y - a.y;
    double E = A*(a.x + b.x) + B*(a.y + b.y);
    double F = C*(a.x + c.x) + D*(a.y + c.y);
    double G = 2.0 * (A*(c.y - b.y) - B*(c.x - b.x));
    if (G == 0) return std::nullopt;

    double cx = (D*E - B*F) / G;
    double cy = (A*F - C*E) / G;

    double radius = std::hypot(a.x - cx, a.y - cy);
    return Vector2{cx, cy - radius};
}

void handleSiteEvent(std::list<Arc>& beachline, std::priority_queue<Event*, std::vector<Event*>, EventCompare>& events, const Event& site, VoronoiDiagram& diagram) {
    Arc newArc{site.point, site.pointIndex, nullptr, nullptr, nullptr};
    if (beachline.empty()) {
        beachline.push_back(newArc);
        return;
    }

    auto it = findArcAbove(beachline, site.point.x, site.point.y);
    Arc& arcAbove = *it;
    if(arcAbove.circleEvent != nullptr) {
        arcAbove.circleEvent->valid = false;
    }
    
    Arc leftArc = arcAbove;
    Arc rightArc = arcAbove;

    it = beachline.erase(it);
    it = beachline.insert(it, rightArc);
    it = beachline.insert(it, newArc);
    it = beachline.insert(it, leftArc);
    
    auto leftIt = it;
    auto middleIt = std::next(it);
    auto rightIt = std::next(middleIt);

    diagram.edges.push_back({Vector2{0,0}, Vector2{0,0}, leftIt->siteIndex, middleIt->siteIndex});
    diagram.edges.push_back({Vector2{0,0}, Vector2{0,0}, middleIt->siteIndex, rightIt->siteIndex});
    middleIt->leftEdge  = &diagram.edges[diagram.edges.size()-2];
    middleIt->rightEdge = &diagram.edges[diagram.edges.size()-1];

    // printf("site: %d(%d) %d\n", &*beachline.end(), beachline.size(), &*std::next(rightIt));
    // for(auto& a: beachline){
    //     printf("%d ", &a);
    // } 
    // printf("\n");
    if (leftIt != beachline.begin()) {
        auto leftLeftIt = std::prev(leftIt);
        if (auto bottom = circleBottom(leftLeftIt->site, leftIt->site, middleIt->site)) {
            Event e{EventType::Circle, *bottom, -1, true};
            leftIt->circleEvent = new Event(e);
            events.push(leftIt->circleEvent);
        }
    }
    auto rightRightIt = std::next(rightIt);
    if (rightRightIt != beachline.end()) {
        if (auto bottom = circleBottom(middleIt->site, rightIt->site, rightRightIt->site)) {
            Event e{EventType::Circle, *bottom, -1, true};
            rightIt->circleEvent = new Event(e);
            events.push(rightIt->circleEvent);
        }
    }
}

void handleCircleEvent(std::list<Arc>& beachline, std::priority_queue<Event*, std::vector<Event*>, EventCompare>& events, Event* circleEvent, VoronoiDiagram& diagram) {
    if (beachline.size() < 3) return;
    auto it = beachline.begin();
    for (; it != beachline.end(); ++it) { if (it->circleEvent == circleEvent) break; }
    if (it == beachline.begin() || it == beachline.end()) return;
    
    Arc& disappearing = *it;
    auto leftIt = std::prev(it);
    auto rightIt = std::next(it);

    Vector2 vertex = circleEvent->point;

    // Update edges
    //if (leftIt->rightEdge) leftIt->rightEdge->end = vertex;
    //if (rightIt->leftEdge) rightIt->leftEdge->end = vertex;

    // Create a new edge between left and right arcs
    diagram.edges.push_back({vertex, {0, 0}, leftIt->siteIndex, rightIt->siteIndex});
    // printf("circle: %d(%d) %d\n", &*it, beachline.size(), &*rightIt);
    // for(auto& a: beachline){
    //     printf("%d ", &a);
    // } 
    // printf("\n");
     auto newEdge = &diagram.edges.back();
    leftIt->rightEdge  = newEdge;
    rightIt->leftEdge  = newEdge;

    // Remove disappearing arc from beachline
    beachline.erase(it);

    // // Invalidate any pending circle events for neighbors
    if (leftIt->circleEvent) {
        leftIt->circleEvent->valid = false;
    }
    if (rightIt->circleEvent) {
        rightIt->circleEvent->valid = false;
    }

    // // Check for new circle events with new neighbor triples
    if (leftIt != beachline.begin()) {
        auto leftLeftIt = std::prev(leftIt);
        if (auto bottom = circleBottom(leftLeftIt->site, leftIt->site, rightIt->site)) {
            Event* e = new Event{EventType::Circle, *bottom, -1, true};
            leftIt->circleEvent = e;
            events.push(e);
        }
    }
    auto rightRightIt = std::next(rightIt);
    if (rightRightIt != beachline.end()) {
        if (auto bottom = circleBottom(leftIt->site, rightIt->site, rightRightIt->site)) {
            Event* e = new Event{EventType::Circle, *bottom, -1, true};
            rightIt->circleEvent = e;
            events.push(e);
        }
    }
}

VoronoiDiagram voronoi(std::vector<Vector2>& points) {
    std::vector<Event*> eventsBuffer;
    eventsBuffer.reserve(2 * points.size());
    for (int i = 0; i < points.size(); i++) {
        eventsBuffer.push_back(new Event{EventType::Site, points[i], i, true});
    }
    VoronoiDiagram diagram;
    std::list<Arc> beachline;
    std::priority_queue<Event*, std::vector<Event*>, EventCompare> events(EventCompare(), std::move(eventsBuffer));

    while(!events.empty()) {
        Event* event = events.top();
        events.pop();
        if(event->valid == false) { 
            delete event; 
            continue;
        }
        if(event->type == EventType::Site) {
            handleSiteEvent(beachline, events, *event, diagram);
        } else {
            handleCircleEvent(beachline, events, event, diagram);
        }
        delete event; 
    }
    return diagram;
}


std::vector<int> DelaunayTriangulation(std::vector<Vector2>& points) {
    std::vector<int> indexArray;
    indexArray.reserve(6* points.size());
    auto diagram = voronoi(points);

    //printf("\n");
    for(int i = 0; i < diagram.edges.size(); i++) {
        auto& edge = diagram.edges[i];
        indexArray.push_back(edge.leftSiteIndex);
        indexArray.push_back(edge.rightSiteIndex);
        //printf("%d %d\n", edge.leftSiteIndex, edge.rightSiteIndex);
    }
    //printf("size: %d\n", diagram.edges.size());

    return indexArray;
}