//
//  Constants.h
//  CGXcode
//
//  Created by Sandra Tieben on 05.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

//Assets
#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

//Shader
#ifdef WIN32
#define SHADER_DIRECTORY "../../shader/"
#else
#define SHADER_DIRECTORY "../shader/"
#endif

//Physics
#define GRAVITY -5.0f
#define DOWNFORCE 0.0f
#define TERRAIN_HEIGHT 0.6f

#define RUNSPEED 6.0f 	//normal speed
#define	ADDSPEED 4.0 	//additional speed for running
#define ROTATIONSPEED 2.0f
#define JUMPPOWER 15.0f

#define ALLCOINS 4;

#define START_POS_X 0.0f
#define START_POS_Y 0.2f
#define START_POS_Z 0.0f

#define GAME_TITLE "CG-2018"

//Window
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800
#define FULLSCREEN false


//helper
#define DELTA 0.001


//#define PHYSICS_PLAYERSPEED 1.0f
//#define PHYSICS_PLAYERROTATIONTIME 2.0f
//#define PHYSICS_SPRINTMULTIPLIER 2.0f
//
//#define PHYSICS_ADDITIONAL_COLLISIONTEST_MINIMUM 0.2f
//
//#define PHYSICS_RESET_HEIGHT -2.0f
//
////View Controls
//#define VIEW_DEADZONE 0.001f
//#define VIEW_SENSITIVITYX 40
//#define VIEW_SENSITIVITYY 30
//
//#define CAMERA_OFFSET_X 0.0f
//#define CAMERA_OFFSET_Y 0.1f
//#define CAMERA_OFFSET_Z 0.0f
//
//Start



#endif /* Constants_h */
