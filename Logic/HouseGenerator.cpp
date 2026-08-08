#include "HouseGenerator.h"
#include "Polygon.h"
#include "Random.h"
#include "umath.h"
#include <Graph.h>
#include <Triangulation.h>

Shapes::Polygon generateFloor(Layer& layer, int points_size = 2,float radius = 40) {
	float size = 0.5f;
	if(points_size < 1) { return Shapes::generatePlatonicSolid(3, size); }
	
	//generate points
	srand(time(NULL));
	points_size = 10;
	std::vector<Vector2> points(points_size); 
	generateRandomArrayVector2(points.begin(), points.end(), {-size, size}, {-size, size});
	std::sort(points.begin(), points.end(), [](const Vector2& a, const Vector2& b) {
		return a.y < b.y || (a.y == b.y && a.x < b.x);
	});

	//generate graph
	std::vector<int> indexArray = DelaunayTriangulation(points);
	ListGraph<float> graph(points_size, false);
	//printf("%d\n", indexArray.size());
	for(int i = 0; i < indexArray.size(); i+=2) {
		graph.addEdge(indexArray[i],  	indexArray[i+1], Vector2::Distance(points[indexArray[i]],   points[indexArray[i+1]]));
		//printf("%d %d\n", indexArray[i], indexArray[i + 1]);
		//graph.addEdge(indexArray[i],  	indexArray[i+2], Vector2::Distance(points[indexArray[i]],   points[indexArray[i+2]]));
		//graph.addEdge(indexArray[i+1],  indexArray[i+2], Vector2::Distance(points[indexArray[i+1]],   points[indexArray[i+2]]));
	}
	auto* graph1 = graph.clone();
	//graph.Dijkstra(0);

	//draw
	float minX = points[0].x, maxX = points[0].x;
	float minY = points[0].y, maxY = points[0].y;

	for(int i = 1; i < points.size(); i++) {
		if(points[i].x < minX) minX = points[i].x;
		if(points[i].x > maxX) maxX = points[i].x;
		if(points[i].y < minY) minY = points[i].y;
		if(points[i].y > maxY) maxY = points[i].y;
	}
	float scaleX = (maxX - minX) != 0 ? 0.8f / (maxX - minX) : 1.0f; // 0.8 = 0.4 - (-0.4)
	float scaleY = (maxY - minY) != 0 ? 0.8f / (maxY - minY) : 1.0f;

	for(int i = 0; i < points.size(); i++) {
		points[i].x = ((points[i].x - minX) * scaleX) - 0.4f;
		points[i].y = ((points[i].y - minY) * scaleY) - 0.4f;
	}
	for(int i = 0; i < points.size(); i++) {
		points[i] = points[i] + Vector2(0.5f, 0.5f);
		points[i].x *= layer.getWidth() * 0.7;
		points[i].y *= layer.getHeight() * 0.7;
	}

	// for(int y = 0; y < layer.getHeight(); y++) {
	// 	for(int x = 0; x < layer.getWidth(); x++) {
	// 		float minDistSq = 1e30f;
	// 		Vector2 p = { (float)x, (float)y };
	// 		for (unsigned int v = 0; v < graph.size(); v++) {
	// 			auto it = graph.neighbors(v);
	// 			while (it.hasNext()) {
	// 				auto [u, w] = it.next();
	// 				float dSq = Vector2::DistanceToSegment(points[v], points[u], p, ChebyshevDistance);
	// 				if(dSq < minDistSq) minDistSq = dSq;
	// 			}
	// 		}
	// 		if(minDistSq > radius) continue;
	// 		layer.drawPixel(x, y, 0xFF'FF'00'FE);
	// 	}
	// }

	for (unsigned int v = 0; v < graph1->size(); v++) {
		auto it = graph1->neighbors(v);
		while (it.hasNext()) {
			auto [u, w] = it.next();
			layer.drawLine2D(points[u], points[v], 0xFF'FF'FF'FF);
		}
	}
	for(int i = 0; i < points.size(); i++) {
		layer.drawPixel(points[i], 0xFF'FF'00'00);
	}


	//find and add interections as new points
	//genereate delauney triangulation
	//build a graph of triangles
	//find ost tree or some kind of it
	//that's it, it's the line, or better say main corridor 

	//boolean forms into finalForm i have verticles and edges so create form i want based on these
	//i do boolean cause if angle is too small there can be selfinterections
	//so how to solve it? maybe girst checnangles and dists?
	//they cant have selfinteractions
	//but they may be very close, thats the problem
	//filter funcion is must have. still it may be empty
	//the dist and radius are unpredictable in base version
	//very thin dist will look like a giant building or dungeion
	//maybe delauney is the soultion. maybe it cann not generate trinagles whic may cause this thing. still dist is unperdictable
	//if dist gonna be too big, then the final risult gonna be just a simple base form.
	//maybe sdf is a problem. maybe i can find something more stable and fast


	//the planno
	//1) do the graph
	//2) do a image tests of graph
	//3) maybe try to find something better than sdf
	//4) or maybe u can solve it by math cause, it fortuune there are 3d parabables, but here the root point is a line
	//5) boolean looks like the common solution but its still not the easiest one
	//6) if i knew that points do no interact i could just do it in O(n), just generate polygone points and connect them
	//7) maybe there is a wa to solve problem with self intereections in polygone? not boolean
	//8) 
	Shapes::Polygon finalForm = Shapes::generatePlatonicSolid(points_size, size);

	//optimizate polygone

	return finalForm;
}

Mesh generateBase(Layer& layer) {
	Shapes::Polygon baseFloor = generateFloor(layer, 10, 6);
	return baseFloor.convertToMesh();
}
	
void HouseGenerator::GenerateHouse(Layer& layer) {
	Mesh base = generateBase(layer);

}