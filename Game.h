#ifndef GAME_H
#define GAME_H
#include <bits/stdc++.h>
class Game{
public:
	bool START;
	int playerFd[2];
	int playerCnt;
	std::map<int,int> id;

	Game();
	~Game();
	void init();
	void prepare(int fd);
	void startGame();
	void doWork();
	void endGame();
	bool getStart();

};

#endif
