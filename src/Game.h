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
#include "Coin.h"

class Game {
public:
	typedef std::list<Coin*> CoinList;
	Game();
	~Game();

	bool start(Character* character, CoinList coins);
private:

};

#endif /* Game_h */
