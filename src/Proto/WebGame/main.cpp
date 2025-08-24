#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <winsock2.h>
#include <Windows.h>
#include <atomic>
#include <ctime>
#pragma comment(lib, "ws2_32.lib")

#define width  20
#define height 10


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

#define AButton 65
#define DButton 68
#define SButton 83
#define WButton 87
#define LArrow 	37
#define TArrow 	38
#define RArrow 	39
#define BArrow 	40
#define Enter  	13
#define ECSAPE	27

std::atomic<bool> server_running(false);
int StartPower = 3;

const char* menu[] = {
	"Host game",
	"Connect",
	"Exit",
};

const char* map ={
	"_Strategy____________________________________\n"
	"#############################################\n"
	"#       #   4     #       #           #     #\n"
	"# ##### #   ###9###     # ### #####   # ###9#\n"
	"#       #   #     #     #     #   #     #   #\n"
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

struct client {
	sockaddr_in addr;
};

struct Entity {
	int x;
	int y;
	char apperance;
	char color;
};

struct Player {
	char id;
	Entity transform;
};

void changeMenu(int& menuSelect, int delta) {
	printf("\033[%d;0H%s", 3 + menuSelect, menu[menuSelect]);
	menuSelect += delta;
	printf("\033[%d;0H\033[47;30m%s\033[0m", 3 + menuSelect, menu[menuSelect]);
}

void writeInt32ToBuffer(char* buffer, int offset, int value) {
	value = htonl(value);
	memcpy(buffer + offset, &value, sizeof(value));
}

int readInt32ToBuffer(char* buffer, int offset) {
	int value;
	memcpy(&value, buffer + offset, sizeof(value));
	return ntohl(value);
}

void startIpSelect(char* ipAddress, size_t size) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    // Полная очистка буфера ввода
    FlushConsoleInputBuffer(hIn);

    printf("\033[4;15HInput Host Ip Address: ");
    
    if (fgets(ipAddress, size, stdin)) {
        // Убираем символ новой строки
        size_t len = strlen(ipAddress);
        if (len > 0 && ipAddress[len - 1] == '\n') {
            ipAddress[len - 1] = '\0';
        }
    }
}

void drawMenu() {
	system("cls");
	printf("SUPER Multiplayer:\n");
	printf("--------------\n");
	printf("\033[47;30m%s\033[0m\n", menu[0]);
	printf("%s\n", menu[1]);
	printf("%s", menu[2]);
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

inline bool checkDigit(char digit) {
	return 47 < digit && digit < 58;
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

inline void drawObject(const Entity obj) {
	printf("\033[%d;%dH\033[%dm%s\033[0m", obj.y + 1, obj.x + 1, WHITE_COLOR, getCharacter(obj.apperance));
}

void drawMap() {
	system("cls");
	int size = WIDTH * HEIGTH + 1;
	char* ptr = (char*)map;
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
		if(*ptr == 'E') {
			*ptr = SPACE;
			++ptr;
			++index;
			continue;
		}
		drawObject({index % WIDTH, index / WIDTH, *ptr, color});
		++ptr;
		++index;
	}
}

DWORD WINAPI server_thread(LPVOID lpParam) {
	srand(time(NULL));
	SOCKET serverSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	unsigned long mode = 1;
	ioctlsocket(serverSock, FIONBIO, &mode);
	char buffer[1024];
	std::vector<client> clients;
	char id = 0;
	SOCKADDR_IN clientAddr;
	int clientSize = sizeof(clientAddr);
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(12345);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
	std::vector<Player> players;
	
	while(server_running) {
		int bytes = recvfrom(serverSock, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientSize);
		if(bytes > 0) {
			buffer[bytes] = '\0';
			if(buffer[0] == 0) { //register
				Player pl;
				pl.id = id;
				pl.transform.x = 1 + rand() % (width - 2);
				pl.transform.y = 1 + rand() % (height - 2);
				pl.transform.apperance = 'A' + id;
				//printf("new user: %d (%d %d) ", pl.id, pl.transform.x, pl.transform.y);
				id++;
				players.push_back(pl);
				clients.push_back({ clientAddr });
				sendto(serverSock, (char*)&pl, sizeof(pl), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
			} 
			else { //new update for character
				int pid = readInt32ToBuffer(buffer, 1);
				int dx = readInt32ToBuffer(buffer, 5);
				int dy = readInt32ToBuffer(buffer, 9);
				int newX = players[pid].transform.x - dx;
				int newY = players[pid].transform.y + dy;
				if(map[((newY) * WIDTH + newX)] != '#') {
					players[pid].transform.x = newX;
					players[pid].transform.y = newY;
				}
			}
		}
		//update logic
		//Update();
		
		//create packets
		int size = 0;
		for(int i = 0; i < players.size(); i++) {
			memcpy(buffer + size, &players[i], sizeof(Player));
			size += sizeof(Player);
		}
		
		//send new states
		for(int i = 0; i < clients.size(); i++) {
			sendto(serverSock, buffer, size, 0, (sockaddr*)&clients[i].addr, sizeof(clients[i].addr));
		}
		Sleep(13);
	}
	closesocket(serverSock);
	
	return 0;
}




Player connectToSrever(SOCKET& clientSocket, sockaddr_in& serverAddr) {
	char buffer[8];
	buffer[0] = 0;
	sendto(clientSocket, buffer, 8, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
	sockaddr_in fromAddr;
	int fromLen = sizeof(fromAddr);
	Player pl;
	Sleep(1000);
	int bytes = recvfrom(clientSocket, (char*)&pl, sizeof(pl), 0, (sockaddr*)&fromAddr, &fromLen);
	return pl;
}

int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	system("chcp 65001");
	drawMenu();
	
	SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	unsigned long mode = 1;
	ioctlsocket(clientSocket, FIONBIO, &mode);
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(12345);
	
	Player player;
	unsigned char* oldKeyBoard = new unsigned char[256];
	unsigned char* newKeyBoard = new unsigned char[256];
	memset(oldKeyBoard, 0, 256);
	memset(newKeyBoard, 0, 256);
	char ipAddress[256];
	char buffer[1024];
	int sceneSelect = 0;
	int menuSelect = 0;
	HANDLE hServer, hClient;
	std::vector<Player> oldPlayers;
	std::vector<Player> players;
	
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prevMode;
    GetConsoleMode(hIn, &prevMode);
    SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT);
	
	INPUT_RECORD bufferKeys[128];
    DWORD events;
	while(1) {
		//input update
		unsigned char* temp = oldKeyBoard; oldKeyBoard = newKeyBoard; newKeyBoard = temp;
		memset(newKeyBoard, 0, 256);
		if (PeekConsoleInput(hIn, bufferKeys, 128, &events) && events > 0) {
            ReadConsoleInput(hIn, bufferKeys, events, &events);
            for (DWORD i = 0; i < events; ++i) {
                if (bufferKeys[i].EventType == KEY_EVENT) {
                    KEY_EVENT_RECORD &ker = bufferKeys[i].Event.KeyEvent;
                    newKeyBoard[ker.wVirtualKeyCode] = ker.bKeyDown;
                }
            }
		}
		//for(int i =0; i < 256; i++) { if((newKeyBoard[i])) { printf("%d %d\n", i, newKeyBoard[i]); } }
		
		switch(sceneSelect) {
			case(0): {	//menu
				if(( newKeyBoard[TArrow] || newKeyBoard[WButton]) && menuSelect > 0) { changeMenu(menuSelect, -1); }
				else if(( newKeyBoard[BArrow] || newKeyBoard[SButton]) && menuSelect < (sizeof(menu) / sizeof(unsigned char*)) - 1) { changeMenu(menuSelect, 1); }
				else if(newKeyBoard[Enter]) { 
					switch(menuSelect) {
						case(0): {  // host
							server_running = true;
							hServer = CreateThread(NULL, 0, server_thread, NULL, 0, NULL);
							if(!hServer) { printf("Failed to create server thread!\n"); return 1; }
							serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
							Sleep(300);
							drawMap();
							player = connectToSrever(clientSocket, serverAddr);
							sceneSelect = 1; 
							printf("player: %d (%d %d) ", player.id, player.transform.x, player.transform.y);
							break;
						}
						case(1): { 
							SetConsoleMode(hIn, prevMode);
							startIpSelect(ipAddress, sizeof(ipAddress));
							SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT);
							serverAddr.sin_addr.s_addr = inet_addr(ipAddress);
							player = connectToSrever(clientSocket, serverAddr);
							sceneSelect = 1;
							drawMap();
							printf("player: %d (%d %d) ", player.id, player.transform.x, player.transform.y);
							break;
						}
						case(2): { return 0; }
					}
				}
				break;
			}
			case(1): {  //game scene main thread
				//input
				if(newKeyBoard[ECSAPE]) {
					server_running = false;
					WaitForSingleObject(hServer, INFINITE);
					drawMenu();
					menuSelect = 0;
					sceneSelect = 0;
					break;
				}
				
				//send
				int deltaX = 0;
				int deltaY = 0;
				if( newKeyBoard[TArrow] || newKeyBoard[WButton]) {
					deltaY = -1;
				}
				else if( newKeyBoard[BArrow] || newKeyBoard[SButton]) {
					deltaY = 1;
				}
				else if( newKeyBoard[LArrow] || newKeyBoard[AButton]) {
					deltaX = 1;
				}
				else if( newKeyBoard[RArrow] || newKeyBoard[DButton]) {
					deltaX = -1;
				}
				if(deltaX != 0 || deltaY != 0) {
					buffer[0] = 0xff;
					writeInt32ToBuffer(buffer, 1, player.id);
					writeInt32ToBuffer(buffer, 5, deltaX);
					writeInt32ToBuffer(buffer, 9, deltaY);
					sendto(clientSocket, buffer, 64, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
				}
				
				
				for(int i = 0; i < oldPlayers.size(); i++) {
					oldPlayers[i] = players[i];
				}
				//recive
				int bytes;
				sockaddr_in fromAddr;
				int fromLen = sizeof(fromAddr);
				while(1) {
					int newBytes = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (sockaddr*)&fromAddr, &fromLen);
					if(newBytes <= 0) break;
					bytes = newBytes;
				}
				if(bytes > 0) {
					bytes = bytes / sizeof(Player);
					if(bytes > players.size()) {
						players.resize(bytes);
						oldPlayers.resize(bytes);
					}
					char* bufferCopy = (char*)buffer;
					for(int i = 0; i < bytes; i++) {
						memcpy(&players[i], bufferCopy, sizeof(Player));
						bufferCopy += sizeof(Player);
					}
				}
				
				//render
				for(int i = 0; i < bytes; i++) {
					//printf("\033[%d;30H %c: %d %d   ", 1 + i, players[i].transform.apperance, players[i].transform.x, players[i].transform.y);
					if(oldPlayers[i].transform.x == players[i].transform.x && oldPlayers[i].transform.y == players[i].transform.y) continue;
					printf("\033[%d;%dH\xE2\x80\xA2", 1 + oldPlayers[i].transform.y, 1 + oldPlayers[i].transform.x);
					printf("\033[%d;%dH%c", 1 + players[i].transform.y, 1 + players[i].transform.x, players[i].transform.apperance);
				}
				
				break;
			}
		}
	}
	return 0;
}