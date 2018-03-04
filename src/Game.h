//
//  Game.h
//  Spiellogik
//
//  Created by Sandra Tieben on 20.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Game_h
#define Game_h

#include "Constants.h"
#include "Character.h"
#include "Control.h"
#include "CollisionHandler.h"
#include "Coin.h"

class Game {
public:
	typedef std::list<Coin*> CoinList;
	Game();
	Game(GLFWwindow* pWindow);
	~Game();

	bool start(Character* character, CoinList coins);
	
	/* Getter */
	CollisionHandler* getCollisionHandler() {return collHandler; };
	Control* getControl() {return playerControl;};
	Character* getCharacter() { return pCharacter; };
	
	int getCollectedCoins() const {return collectedCoins; };
	
	/* Setter */
	void setCollectedCoins(unsigned int c) {collectedCoins = c; };
	void setControl(Control* c) {playerControl = c;}
	void foundCoin() { collectedCoins++; };
	
	void setCharacter(Character* c) {pCharacter = c; };
	//void setWindow(GLFWwindow* pW) {pWindow = pW; }
	

private:
	GLFWwindow* pWindow;
	Character* pCharacter;
	Control* playerControl;
	CollisionHandler* collHandler;
	
	unsigned int allCoins = ALLCOINS;
	unsigned int collectedCoins = 0;

};

#endif /* Game_h */
