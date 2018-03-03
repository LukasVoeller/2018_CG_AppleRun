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
#include "CollisionHandler.h"
#include "Coin.h"

class Game {
public:
	typedef std::list<Coin*> CoinList;
	Game();
	~Game();

	bool start(Character* character, CoinList coins);
	
	/* Getter */
	CollisionHandler getCollisionHandler() {return collHandler;};
	
	int getCollectedCoins() const {return collectedCoins; };
	
	/* Setter */
	void setCollectedCoins(unsigned int c) {collectedCoins = c; };
	void foundCoin() { collectedCoins++; };
	

private:
	CollisionHandler collHandler;
	
	unsigned int allCoins = ALLCOINS;
	unsigned int collectedCoins = 0;

};

#endif /* Game_h */
