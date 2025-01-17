//==============================================================================
//
// Title:		Projectile.c
// Purpose:		Projectile class with drawing method and calcululating projectile starting position.
//
// Created on:	10/08/2020 at 8:48:35 by Alex Bordeaux.
// Copyright:	Student. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include "Sound.h"
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>
#include "Projectile.h"
#include "Physics_Engine.h"
#include "Tank_Game.h"
#include "Collison_Module.h"

//==============================================================================
// Static functions
static void CheckCollisionForProjectile(PROJECTILE* projectile);


PROJECTILE* new_PROJECTILE(POSITION* position)
{
	PROJECTILE* projectile=calloc(1,sizeof(PROJECTILE));
	projectile->position = position;
	projectile->Draw_Projectile = &Draw_Projectile ; 
	projectile->Fire_Projectile = &Fire_Projectile;
	return projectile;


}
void Fire_Projectile(PROJECTILE* projectile , TANK* tank)
{
	PlaySound(ShootingSFX);
	InitVelocety(tank);
	// Detect if left tank shoots or right .
	// Then we calculating projectile starting position depending on barrel angle .
	if ( tank->position->x < 940 ) //940 is the top of the hill, so left of it is the left tank.
	{
		projectile->position->x = tank->position->x + 160 + 80*cos(tank->angle);
		projectile->position->y = tank->position->y + 92 - 80*sin(tank->angle);
	}
	else
	{
		if(tank->angle < 100)
		{
			
			projectile->position->x = tank->position->x - 25  + 100 * sin((tank->angle*Pi())/180 );
			projectile->position->y = tank->position->y + 80 - 70 * sin((tank->angle*Pi())/180 );
		
		}
		else
		{
		
			projectile->position->x = tank->position->x - 40  + 80 * sin((tank->angle*Pi())/180 );
			projectile->position->y = tank->position->y + 80 - 70 * sin((tank->angle*Pi())/180 );
		}
	}

}
void Draw_Projectile(PROJECTILE* projectile)
{
	CanvasClear(gamePanel,Game_Panel_CANVAS,MakeRect(projectile->position->y,projectile->position->x,5,5));
	CalclTrace(projectile->position);
	DetectCollision(projectile);
	SetCtrlAttribute (gamePanel, Game_Panel_CANVAS, ATTR_PEN_COLOR, VAL_BLACK);
	SetCtrlAttribute (gamePanel, Game_Panel_CANVAS, ATTR_PEN_FILL_COLOR, VAL_BLACK);
	CanvasDrawRect (gamePanel, Game_Panel_CANVAS,MakeRect(projectile->position->y,projectile->position->x,5,5), VAL_DRAW_FRAME_AND_INTERIOR);
	
	
}
