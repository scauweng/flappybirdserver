#include "Game.h"
#include "libmessage/MessageBuffer.h"
#include <bits/stdc++.h>

using namespace std;

Game::Game(){
	START=false;
	id.clear();
	playerCnt=0;
}

Game::~Game(){
	
}

bool Game::getStart(){
	return START;
}

void Game::prepare(int fd){
	playerFd[playerCnt]=fd;
	id[fd]=playerCnt;
	if(++playerCnt==2)startGame();
}

void Game::startGame(){
	START=true;
}

void Game::doWork(){
	if(!START)return;
}
	
void Game::endGame(){

}
