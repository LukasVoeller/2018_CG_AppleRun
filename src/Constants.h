//
//  Constants.h
//  CGXcode
//
//  Created by Sandra Tieben on 05.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

// Assets
#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

// Shader
#ifdef WIN32
#define SHADER_DIRECTORY "../../shader/"
#else
#define SHADER_DIRECTORY "../shader/"
#endif

#define GRAVITY -5.0f
#define DOWNFORCE 0.0f
#define TERRAIN_HEIGHT 0.0f

#define RUNSPEED 20.0f 				// Normal speed
#define ADDSPEED 5.0 				// Additional speed for running
#define ROTATIONSPEED 4.0f
#define JUMPPOWER 30.0f

#define ALLCOINS 2;

#define START_POS_X 0.0f
#define START_POS_Y 0.0f
#define START_POS_Z 0.0f

#define GAME_TITLE "CG-2018"

// Window
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800
#define FULLSCREEN false

// Helper
#define DELTA 0.001

#define BORDER_MIN_X -200.0f
#define BORDER_MAX_X 200.0f
#define BORDER_MIN_Z -200.0f
#define BORDER_MAX_Z 200.0f

//View Controls
#define CAMERA_OFFSET_X 0.0f
#define CAMERA_OFFSET_Y 0.1f
#define CAMERA_OFFSET_Z 0.0f

#endif /* Constants_h */
