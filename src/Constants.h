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

//Physics
#define GRAVITY -15.0f
#define DOWNFORCE 0.0f
#define TERRAIN_HEIGHT 0.6f

#define RUNSPEED 4.0f
#define ROTATIONSPEED 2.0f;
//
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
////Start
//#define PLAYER_START_X 0.1f
//#define PLAYER_START_Y 1.0f
//#define PLAYER_START_Z 3.0f


#endif /* Constants_h */
