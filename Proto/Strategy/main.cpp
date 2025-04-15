#include <stdio.h>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include <conio.h>
#include <Graphs/Graphs/Graph.h>

#define PlaceBarriers		0x01 //should place barriers
#define RayPLayer			0x02 //see player by dist
#define NoWallBreak			0x04 //create unbreakeble walls
#define ControlWalls		0x08 //check if player broke some walls

#define Patrol				0x10 //???
#define UpgradeBarierrsMin	0x20 //upgrade till 5
#define UpgradeBarierrsMax	0x40 // upgrade till 9


#define WIDTH  		 		46
#define HEIGTH 		 		16
#define BLUE_COLOR   		36
#define DARK_BLUE_COLOR   	34
#define PURPLE_COLOR   		35
#define YELLOW_COLOR 		33
#define GREEN_COLOR 		32
#define RED_COLOR	 		31
#define WHITE_COLOR	 		37
#define SPACE 		 		' '

const char* map ={
	"_Strategy____________________________________\n"
	"#############################################\n"
	"#       #   4     #       #           #     #\n"
	"# ##### #   ###9###     # ### #####   # ###9#\n"
	"#       #   #     #     #E    #   #     #   #\n"
	"#   ###4### #     ##### #     #   ##### #   #\n"
	"#   #       #9#####     #     #         #   #\n"
	"# G # ##### #     9     ###4###     #   9   #\n"
	"#   7 #     #     #           #     #   #   #\n"
	"##### #   ### ### #####3########### ### #####\n"
	"#     #       #                 #           #\n"
	"#4### ## ##   # ##9####8####    #   ###9## ##\n"
	"#   #         # #     #    #    #   # G #   #\n"
	"#   # ##### ### #     #    ####3#####8###   #\n"
	"#   6   #       #           2               #\n"
	"#############################################\n"
}; 


struct Vector2 {
	int x;
	int y;
};

Vector2 FlagList[] = {
	{20, 3},
	{8, 9},  //
	{43, 2}, 
	{18, 14}, //
	{1, 14},
	{31, 5}, //
	{15, 5},
	{43, 5}, //
	{1, 2},
};

struct Object {
	Vector2 position;
	int  power;
	char appearance;
	char color;
};

struct Plan_step {
	Vector2 pos;
	int state;
};

struct Plan {
	std::vector<Plan_step> steps;
	int step = 0;
	int state;
	std::vector<Vector2> control_walls;
	std::vector<Vector2> patrol;
	int partrol_step = 0;
	Vector2 flag;
};

Plan plan;
char* map_copy;
int radius = 4;
int StartPower = 3;
int SkipStep = 4;
int SeeDist	= 1000000;
int step = 1;
Vector2 last_player_pos = {0, 0};

inline int distSqrt(Vector2 a, Vector2 b) {
	a.x -= b.x;
	a.y -= b.y;
	return (a.y * a.y) + (a.x * a.x);
}

inline bool checkDigit(char digit) {
	return 47 < digit && digit < 58;
}

char* restult = new char[2];
const char* getCharacter(char input) {
	switch(input) {
		case(SPACE): 	{ return "\xE2\x80\xA2";};
		case('#'): 		{ return "\xE2\x96\x88";};
	};
	restult[0] = input;
	restult[1] = 0;
	return restult;
}

int getCharacterColor(char input) {
	switch(input) {
		case('G'): 		{ return YELLOW_COLOR;};
		case('P'): 		{ return GREEN_COLOR ;};
		case('E'): 		{ return RED_COLOR   ;};
	};
	if(checkDigit(input)) {
		return BLUE_COLOR;
	}
	return WHITE_COLOR;
}

int getPower(char input) {
	switch(input) {
		case('E'): 		{ return StartPower;};
	};
	return 0;
}

inline void drawObject(const Object& obj) {
	printf("\033[%d;%dH\033[%dm%s\033[0m", obj.position.y + 1, obj.position.x + 1, (obj.color), getCharacter(obj.appearance));
}

inline void clearByPos(int x, int y) {
	int index = y * WIDTH + x;
	printf("\033[%dm\033[%d;%dH%s", getCharacterColor(map_copy[index]), y + 1, x + 1, getCharacter(map_copy[index]));
}

inline void moveDrawObject(const Object& obj, int old_x, int old_y) {
	clearByPos(old_x, old_y);
	drawObject(obj);
}

inline void moveObject(Object& obj, int new_x, int new_y) {
	obj.position.x = new_x;
	obj.position.y = new_y;
}

inline char getWeight(char input) {
	switch(input) {
		case('G'): { }
		case('P'): { }
		case('E'): { }
		case(SPACE): { return 1; }
	};
	if(checkDigit(input)) {
		return  1 + (input - '0') / StartPower;
	}
	return 0;
}

inline void recalculateGraphPointWeight(MatrixGraph<int>& graph, int x, int y, bool unbreakable_for_bots) {
	if(y < 0 || x < 0) { return;}
	int index = y * WIDTH + x;
	char c = map_copy[index];
	char sanya_lox = 0;
	c = getWeight(c);
	if(c == false) { return; }
	if(x < WIDTH - 1) {
		int neighbor_index = index + 1;
		if(sanya_lox = getWeight(map_copy[neighbor_index])) {
			if(unbreakable_for_bots) {
				graph.addEdge(neighbor_index, index, inf);
			}
			else {
				graph.addEdge(neighbor_index, index, c + sanya_lox);
			}
		}
	}
	if(y < HEIGTH - 1) {
		int neighbor_index = index + WIDTH;
		if(sanya_lox = getWeight(map_copy[neighbor_index])) {
			if(unbreakable_for_bots) {
				graph.addEdge(neighbor_index, index, inf );
			}
			else {
				graph.addEdge(neighbor_index, index, c + sanya_lox);
			}
		}
	}
	if(x > 0) {
		int neighbor_index = index - 1;
		if(sanya_lox = getWeight(map_copy[neighbor_index])) {
			if(unbreakable_for_bots) {
				graph.addEdge(neighbor_index, index, inf);
			}
			else {
				graph.addEdge(neighbor_index, index, c + sanya_lox);
			}
		}
	}
	if(y > 0) {
		int neighbor_index = index - WIDTH;
		if(sanya_lox = getWeight(map_copy[neighbor_index])) {
			if(unbreakable_for_bots) {
				graph.addEdge(neighbor_index, index, inf );
			}
			else {
				graph.addEdge(neighbor_index, index, c + sanya_lox);
			}
		}
	}
}

inline void moveObjectWithCollsioionCheck(MatrixGraph<int>& graph, Object& obj, int new_x, int new_y) {
	char c = map_copy[new_y * WIDTH + new_x];
	if(c == SPACE) {
		moveObject(obj, new_x, new_y);
	}
	else if(checkDigit(c)) {
		int digit = c - 48;
		digit -= obj.power;
		if(digit <= 0) {
			map_copy[new_y * WIDTH + new_x] = SPACE;
			drawObject({{new_x, new_y}, 0, SPACE, WHITE_COLOR});
			//moveObject(obj, new_x, new_y);
		}
		else {
			map_copy[new_y * WIDTH + new_x] = (char)(48 + digit);
			drawObject({{new_x, new_y}, 0, (char)(48 + digit), BLUE_COLOR});
		}
		recalculateGraphPointWeight(graph, new_x, new_y, 	 false);
	}
	else if(c == 'G') {
		obj.power++;
		map_copy[new_y * WIDTH + new_x] = SPACE;
		moveObject(obj, new_x, new_y);
		printf("\033[%d;%dH%d", HEIGTH + 1 + (obj.appearance == 'E'), 17, obj.power);
	}
}

inline char Clamp(char a, char b, char x) {
	if(x < a) {
		return a;
	}
	else if(x > b) {
		return b;
	}
	return x;
}

void createBarrier(MatrixGraph<int>& graph, std::vector<Object>& objects, int self_index, bool unbreakable_for_bots) {
	Object& obj = objects[self_index];
	if(obj.power == 0) {return;}
	int index = obj.position.y * WIDTH + obj.position.x;
	char c = map_copy[index];
	char digit = 48 + ((c == SPACE)? obj.power: Clamp(0, 9, c - 48 + obj.power % 10)) % 10;
	Object new_bar = {{obj.position.x, obj.position.y}, 0, digit, obj.color};
	map_copy[index] = digit;
	drawObject(new_bar);
	
	recalculateGraphPointWeight(graph, obj.position.x, obj.position.y, unbreakable_for_bots);
}
	
MatrixGraph<int> createGraph() {
	MatrixGraph<int> graph(HEIGTH * WIDTH, true);
	for(int y = 0; y < HEIGTH; y++) {
		for(int x = 0; x < WIDTH; x++) {
			recalculateGraphPointWeight(graph, x, y, false);
		}
	}
	return graph;
}

std::vector<Object> createMap() {
	int size = WIDTH * HEIGTH + 1;
	map_copy = new char[size];
	memcpy(map_copy, map, size);
	char* ptr = (char*)map_copy;
	std::vector<Object> objects;
	objects.push_back({{1, 2}, 0, 'P', GREEN_COLOR});
	int index = 0;
	while(1) {
		if(*ptr == 0) {
			break;
		}
		char color = getCharacterColor(*ptr);
		if(color == WHITE_COLOR) {
			printf("\033[%dm%s", WHITE_COLOR, getCharacter(*ptr));
			++ptr;
			++index;
			continue;
		}
		printf("\033[%dm%s", WHITE_COLOR, getCharacter(SPACE));
		objects.push_back({{index % WIDTH, index / WIDTH}, getPower(*ptr), *ptr, color});
		if(*ptr == 'E') {
			*ptr = SPACE;
			++ptr;
			++index;
			continue;
		}
		drawObject({{index % WIDTH, index / WIDTH}, getPower(*ptr), *ptr, color});
		++ptr;
		++index;
	}
	return objects;
}

template<class T>
T Manghattan(int a, int b) {
	int x1 = a % WIDTH ;
    int y1 = a / WIDTH;
    int x2 = b % WIDTH ;
    int y2 = b / WIDTH;
	return std::abs(x1 - x2) + std::abs(y1 - y2);
}

inline char getSidesState(int x, int y) {
	return 	((map_copy[(y - 1) 	* WIDTH + x		] != '#') << 3) |
			((map_copy[(y + 1) 	* WIDTH + x		] != '#') << 2) |
			((map_copy[y 		* WIDTH + x - 1 ] != '#') << 1) |
			((map_copy[y 		* WIDTH + x + 1 ] != '#') << 0);
}

bool checkPLacingBarrier(int x, int y, char state) {
	char c = map_copy[y * WIDTH + x];
	switch(state) {
		case(0): { return (c == SPACE); }
		case(1): { return (c == SPACE) || (checkDigit(c) &&  ((c - 48) < 5)); }
		case(2): { }
		case(3): { return (c == SPACE) || (checkDigit(c) &&  ((c - 48) < 9)); }
	}
	return 0;
}

int countBits(char number) {
	int counter = 0;
	for(int i = 0; i < 4; i++) {
		if((number >> i) & 0x1) counter++;
	}
	return counter;
}

inline int checkMapPoint(Vector2 point) {
	int index  = point.y * WIDTH + point.x;
	char up    = ((map_copy[index - WIDTH - 1] != '#') << 2) |  ((map_copy[index - WIDTH    ] != '#') << 1) | (map_copy[index - WIDTH + 1] != '#');
	char down  = ((map_copy[index + WIDTH - 1] != '#') << 2) |  ((map_copy[index + WIDTH    ] != '#') << 1) | (map_copy[index + WIDTH + 1] != '#');
	char left  = ((map_copy[index - WIDTH - 1] != '#') << 2) |  ((map_copy[index         - 1] != '#') << 1) | (map_copy[index + WIDTH - 1] != '#');
	char right = ((map_copy[index - WIDTH + 1] != '#') << 2) |  ((map_copy[index         + 1] != '#') << 1) | (map_copy[index + WIDTH + 1] != '#');
	return (up << 24) | (down << 16) | (left << 8) | (right << 0);
}

void findExits(std::vector<bool>& visited, std::vector<Vector2>& exits, Vector2 pos) {
	if(map_copy[pos.y * WIDTH + pos.x] == '#') {return;}
	if(visited[pos.y * WIDTH + pos.x]) { return; }
	visited[pos.y * WIDTH + pos.x] = true;
	
	int state = checkMapPoint(pos);
	char* byteArray = (char*)(&state);
	if(byteArray[0] == 2 || byteArray[1] == 2 || byteArray[2] == 2 || byteArray[3] == 2) {
		exits.push_back(pos);
		return;
	}
	state = countBits(getSidesState(pos.x, pos.y));
	if(state == 2) {return;}
	
	findExits(visited, exits, {pos.x - 1, pos.y});
	findExits(visited, exits, {pos.x + 1, pos.y});
	findExits(visited, exits, {pos.x, pos.y - 1});
	findExits(visited, exits, {pos.x, pos.y + 1});
}

inline char getEntersAndExits(
	std::vector<Vector2>& enters, std::vector<Vector2>& exits,
	std::vector<Vector2>& almostEnters, std::vector<Vector2>& almostExits,
	Vector2 startSearch) 
{
	std::vector<bool>    visited;
	std::vector<Vector2>    exits_fake;
	visited.resize(WIDTH * HEIGTH);
	findExits(visited, exits_fake, startSearch);
	for(int i = 0; i < exits_fake.size(); i++) {
		//Object obj = {exits_fake[i], 0, 'O', RED_COLOR}; drawObject(obj);
		
		int state = checkMapPoint(exits_fake[i]);
		char* byteArray = (char*)(&state);
		Vector2 dirs[4] = {
			{ 1,  0},
			{-1,  0},
			{ 0,  1},
			{ 0, -1},
		};
		
		for(int j = 3; j > -1; j--) {
			if(byteArray[j] != 2) { continue; }
			Vector2 last_pos = exits_fake[i];
			Vector2 pos = {exits_fake[i].x + dirs[j].x, exits_fake[i].y + dirs[j].y};
			Vector2 start_pos = pos;
			while(1) {
				int tState = getSidesState(pos.x, pos.y);
				int tCount = countBits(tState);
				if(tCount != 2) {
					//Object objs = {pos, 0, 'I', GREEN_COLOR}; drawObject(objs);
					exits.push_back(exits_fake[i]);
					almostExits.push_back(start_pos);
					almostEnters.push_back(last_pos);
					enters.push_back(pos);
					break;
				}
				for(int k = 0; k < 4; k++) {
					if(((tState >> k) & 0x1) == 0) { continue; }
					Vector2 next_pos = {pos.x + dirs[k].x, pos.y + dirs[k].y};
					if(next_pos.x == last_pos.x && next_pos.y == last_pos.y) { continue; }
					last_pos = pos;
					pos = next_pos;
					break;
				}
			}
		}
	}
	
	return -1;
};

void writeBotText(int state, int planNumber) {
	printf("\033[2;60Hbot's state for plan%c: ", 65 + planNumber);
	const char* text[] = {
		"PlaceBarriers:\t",
		"Ray PLayer:\t\t",
		"UnBreakable Wall:\t",
		"ControlWalls:\t",
		"Patrol:\t\t",
		"UpgradeBarierrsMin:\t",
		"UpgradeBarierrsMax:\t",
		"PlaceBarriers:\t",
	};
	for(int i = 0; i < 7; i++) {
		printf("\033[%d;60H%s%d", i + 3, text[i], 0x1 & (state >> i));
	}
}

char AttackPLayer(MatrixGraph<int>& graph, std::vector<Object>& objects, int aim_index, int index) {
	int first_index = objects[aim_index].position.y * WIDTH + objects[aim_index].position.x;
	int last_index  = objects[index].position.y * WIDTH + objects[index].position.x;
	PathData<int> path = graph.Astar(first_index, last_index, Manghattan);
	if(path.path == nullptr) { return 0;}
	int new_index = path.path[last_index];
	if(path.dist[last_index] > (radius << 2)) {
		last_player_pos = {0, 0};
		return 0;
	}
	
	moveObjectWithCollsioionCheck(graph, objects[index], new_index % WIDTH, new_index / WIDTH);
	if(path.dist[last_index] <= 2) {
		system("cls");
		printf("You're the loser!!! ");
	}
	//printf("'%d %d'", objects[index].position.x,objects[aim_index].position.x );
	return 1;
}

int getStep() {
	if(step == sizeof(FlagList) / sizeof(Vector2)) {
		return -1;
	}
	if(step % 2) {
		return step;
	}
	else  {
		return 0;
	}
}

bool seeByRadius(Vector2 posA, Vector2 posB) {
	posA.x -= posB.x;
	posA.y -= posB.y;
	if(posA.x * posA.x + posA.y * posA.y < radius * radius) {
		return true;
	}
	return false;
}

bool seeByRay2(Vector2 posA, Vector2 posB) {
	if(posA.y > posB.y) {
		std::swap(posA, posB);
	}
	float k = (float)(posB.x - posA.x) / (posB.y - posA.y);
	float x = posA.x;
	for(int y = posA.y; y < posB.y; y++, x += k) {
		int index = (int)x + WIDTH * (int)y;
		char c = map_copy[index];
		if(c == '#' || checkDigit(c)) {
			return false;
		}
	}
	return true;
}

bool seeByRay(Vector2 posA, Vector2 posB) {
	int dx = std::abs(posA.x - posB.x);
	int dy = std::abs(posA.y - posB.y);
	if(dx < dy) {
		return seeByRay2(posA, posB);
	}
	if(posA.x > posB.x) {
		std::swap(posA, posB);
	}
	float k = (float)(posB.y - posA.y) / (posB.x - posA.x);
	float y = posA.y;
	for(int x = posA.x; x < posB.x; x++, y += k) {
		int index = x + WIDTH * (int)y;
		char c = map_copy[index];
		if(c == '#' || checkDigit(c)) {
			return false;
		}
	}
	return true;
}

void planA(Vector2 aim, int bot_state)  {
	std::vector<Vector2> enters;
	std::vector<Vector2> almostEnters;
	std::vector<Vector2> almostExits;
	std::vector<Vector2> exits;
	getEntersAndExits(enters, exits, almostEnters, almostExits, aim);
	
	plan.flag = aim;
	plan.state = bot_state;
	plan.step = 0;
	plan.steps.clear();
	plan.control_walls.clear();
	plan.patrol.clear();
	plan.steps.push_back({aim, bot_state});
	for(int i = 0; i < exits.size() - 1; i++) {
		plan.steps.push_back({almostEnters[i], bot_state});
		plan.steps.push_back({exits[i],  bot_state |   PlaceBarriers});
		plan.control_walls.push_back(almostEnters[i]);
		plan.patrol.push_back(enters[i]);
	}
	
	plan.steps.push_back({exits [exits.size()  - 1], bot_state});
	plan.steps.push_back({almostEnters[enters.size() - 1], bot_state |   PlaceBarriers});
	plan.steps.push_back({almostEnters[enters.size() - 1], bot_state});
	plan.control_walls.push_back(almostEnters[enters.size() - 1]);
	plan.patrol.push_back(enters[enters.size() - 1]);
}

void planB(Vector2 aim, int bot_state) {
	std::vector<Vector2> enters;
	std::vector<Vector2> almostEnters;
	std::vector<Vector2> almostExits;
	std::vector<Vector2> exits;
	getEntersAndExits(enters, exits, almostEnters, almostExits, aim);
	
	plan.state = bot_state;
	plan.step = 0;
	plan.steps.clear();
	plan.steps.push_back({aim, bot_state});
	plan.patrol.push_back(aim);
	for(int i = 0; i < exits.size(); i++) {
		plan.steps.push_back({enters[i], bot_state});
		plan.steps.push_back({exits[i],  bot_state |   PlaceBarriers});
		plan.control_walls.push_back(exits[i]);
	}
}

void fixWall(Vector2 wall) {
	plan.steps.push_back({wall, plan.state});
	plan.steps.push_back({wall, plan.state |   PlaceBarriers});
	plan.steps.push_back({wall, plan.state});
}

void updatePlan() {
	int wok = getStep();
	if(wok == - 1) { plan.state = 0; return;}
	if(wok == 0)	 {return;}
	Vector2 aim = FlagList[wok];
	int r = rand();
	int random_state = (r & Patrol) | (r & UpgradeBarierrsMax) | (r & UpgradeBarierrsMin);
	int bot_state = random_state | RayPLayer | NoWallBreak | ControlWalls;
	
	plan.partrol_step = 0;
	plan.patrol.clear();
	plan.steps.clear();
	plan.control_walls.clear();
	void (*plans[])(Vector2, int) = {
		planA,
		planB,
	};
	int planNumber = rand() % (sizeof(plans) / sizeof(void*));
	writeBotText(bot_state, planNumber);
	plans[0](aim, bot_state);
}

void printCoord() {
	//printf("\033[1;60Hbot's aims is(%d, %d)", 65 + planNumber)
}

void moveEnemyToAim(MatrixGraph<int>& graph, std::vector<Object>& objects, Vector2 aim, int index) {
	int first_index = aim.y * WIDTH + aim.x;
	int last_index  = objects[index].position.y * WIDTH + objects[index].position.x;
	if(first_index == last_index) {
		plan.step++;
		if(plan.step == plan.steps.size()) {return;}
		plan.state = plan.steps[plan.step].state;
		return;
	}
	
	PathData<int> path = graph.Astar(first_index, last_index, Manghattan);
	if(path.path == nullptr) {return;}
	int new_index = path.path[last_index];
	moveObjectWithCollsioionCheck(graph, objects[index], new_index % WIDTH, new_index / WIDTH);
}

void UpdateEnemy(MatrixGraph<int>& graph, std::vector<Object>& objects, int aim_index, int index) {	
	//attack player most important
	if(distSqrt(objects[index].position, plan.flag) < 15 * 15) {
		Vector2 player = objects[aim_index].position;
		if(plan.state & RayPLayer) {
			if(seeByRadius(objects[index].position, player)) {
				last_player_pos = player;;
				if(AttackPLayer(graph, objects, aim_index, index)) return;
			}
		}
		else {
			if(seeByRay(objects[index].position, player)) {
				last_player_pos = player;;
				if(AttackPLayer(graph, objects, aim_index, index)) return;
			}
		}
	}
	
	//search player if he is close
	if(last_player_pos.x != 0) {
		moveEnemyToAim(graph, objects, last_player_pos, index);
		if(last_player_pos.x == objects[index].position.x && last_player_pos.y == objects[index].position.y) { last_player_pos = {0, 0};}
		return;
	}
	
	//place barierrs if its important
	if(plan.state & PlaceBarriers) {
		if(checkPLacingBarrier(objects[index].position.x, objects[index].position.y, ((plan.state & UpgradeBarierrsMin) != 0) | (((plan.state & UpgradeBarierrsMax) != 0) << 1))) {
			objects[index].color = BLUE_COLOR;
			createBarrier(graph, objects, index, plan.state & NoWallBreak);
			objects[index].color = RED_COLOR;
			return;
		};
	}

	//check if player broke some walls
	if(plan.step == plan.steps.size() && plan.state & ControlWalls) {
		for(int i = 0; i < plan.control_walls.size(); i++) {
			int wall_index = plan.control_walls[i].y * WIDTH + plan.control_walls[i].x;
			if(!checkDigit(map_copy[wall_index]) || ((map_copy[wall_index] - 48) < StartPower)) {
				fixWall(plan.control_walls[i]);
			}
		}
	}
	
	if((plan.step == plan.steps.size()) && (plan.state & Patrol)) {
		Vector2 aim = plan.patrol[plan.partrol_step];
		moveEnemyToAim(graph, objects, aim, index);
		Vector2 pos = objects[index].position;
		if(aim.x == pos.x && aim.y == pos.y) {
			plan.partrol_step++;
			plan.partrol_step = plan.partrol_step % plan.patrol.size();
		}
		return;
	}
	
	//do the plan 
	if(plan.step == plan.steps.size()) {return;}
	Vector2 aim = plan.steps[plan.step].pos;
	moveEnemyToAim(graph, objects, aim, index);
}

int main();

void UpdatePlayer(char c, MatrixGraph<int>& graph, std::vector<Object>& objects, int flag_index, int player_index) {
	int old_x =  objects[player_index].position.x;
	int old_y =  objects[player_index].position.y;
	int new_x =  objects[player_index].position.x;
	int new_y =  objects[player_index].position.y;
	if(c == 72 || c == 119) { //up
		new_y--;
	}
	else if(c == 80 || c == 115) { //down
		new_y++;
	}
	else if(c == 77 || c == 100) { //right
		new_x++;
	}
	else if(c == 75 || c == 97) { //left
		new_x--;
	}
	else if(c == 101 || c == 32) { //E SPACE
		createBarrier(graph, objects, player_index, false);
	}
	else if(c == 9) { //TAB
		main();
	}
	if(objects[player_index].position.x == objects[flag_index].position.x && objects[player_index].position.y == objects[flag_index].position.y) {
		if(step == sizeof(FlagList) / sizeof(Vector2)) {
			system("cls");
			printf("You're not as cool as me, but ok!!!");
			return;
		}
		//map_copy[objects[flag_index].position.y * WIDTH + objects[flag_index].position.x] = SPACE;
		objects[flag_index].position = FlagList[step++];
		//map_copy[objects[flag_index].position.y * WIDTH + objects[flag_index].position.x] = 'F';
		drawObject(objects[flag_index]);
		updatePlan();
	}
	if(new_x != old_x || new_y != old_y) {
		moveObjectWithCollsioionCheck(graph, objects[player_index], new_x, new_y);
	}
	if(objects[player_index].position.x != old_x || objects[player_index].position.y != old_y) {
		moveDrawObject(objects[player_index], old_x, old_y);
	}
}

void drawEnemy(Object& enemy, Object& player) {
	static Vector2 pos;
	bool see = seeByRay(enemy.position, player.position) || (distSqrt(enemy.position, player.position) < SeeDist * 2 + 1);
	if(see) {
		moveDrawObject(enemy, pos.x, pos.y);
		pos = enemy.position;		
	} 
	else {
		Object obj = {pos, 0, (map_copy[pos.x + pos.y * WIDTH]), (char)getCharacterColor(map_copy[pos.x + pos.y * WIDTH])};
		drawObject(obj);
	}
}

int main() {
	//readFromFile("./settings.txt");
	
	last_player_pos = {0, 0};
	step = 1;
	srand(time(NULL));
	fputs("\e[?25l", stdout);
	system("chcp 65001");
	for(int i = 0; i < 80; ++i) { printf("   \n"); }
	system("cls");
	std::vector<Object> objects = createMap();
	objects.push_back({FlagList[0], 0, 'F', DARK_BLUE_COLOR});
	int flag_index = objects.size() - 1;
	MatrixGraph<int> graph = createGraph();
	int player_index = 0;
	for(int i = 0; i < objects.size(); i++) {
		if(objects[i].appearance == 'P') {
			player_index = 0;
			break;
		}
	}
	int enemy_index = 0;
	for(int i = 0; i < objects.size(); i++) {
		if(objects[i].appearance == 'E') {
			enemy_index = i;
			break;
		}
	}
	printf("Your  strength: %d\n", objects[player_index].power);
	printf("Enemy strength: %d\n", objects[enemy_index].power);
	printf("\nControl: movement by 'ARROWS', make barrier by 'E'\n");
	for(int i = 0; i < sizeof(objects) / sizeof(Object); ++i) {
		drawObject(objects[i]);
	}
	int tick_couter = 0;
	drawObject(objects[flag_index]);
	printf("\033[%d;%dH", 19, 0);
	updatePlan();
	while(1)
	{
		char c = getch();
		if(c == -32) { continue; }
		if(c == 3) { break; }
		UpdatePlayer(c, graph, objects, flag_index, player_index);
		if((SkipStep == 1) || (tick_couter % SkipStep != SkipStep - 1)) { UpdateEnemy(graph, objects, player_index, enemy_index); }
		drawEnemy(objects[enemy_index], objects[player_index]);
		printf("\033[%d;%dH", 19, 0);
		//printf("%u ", (unsigned int)c);
		tick_couter++;
	}
	return 0;
}
