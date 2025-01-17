//==============================================================================
//
// Title:		Tank_Game.c
// Purpose:		Main game client that run all necessery function to run the game
//
// Created on:	10/08/2020 at 19:59:41 by Barack Samuni and Alex Bordeaux.
// Copyright:	Afeka academic college of engineering. All Rights Reserved.
//
//==============================================================================



//===============================================================================
// Include files
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "Tank_Game.h"
#include "Tank_Object.h"
#include <Windows.h>
#include "toolbox.h"
#include "Sound.h"
#include "Projectile.h"
#include "Ground_Object.h"
#include "Animation.h"
//===============================================================================
///////////////////////////Functions/////////////////////
void InitializeGame();
void DiscardAll();
int DrawAllScene();
int CVICALLBACK KeyupCallback(int panel, int message,unsigned int* wParam,unsigned int* lParam,void* callbackData);
//--------------------------------------------------------------------------------------------------------------------
//==============================================================================
// Global variables
CmtThreadPoolHandle MY_THREAD_POOL , RenderingID ,AnimationID ;
int menuPanel,gamePanel,controlsPanel,wmp_Panel,optionsPanel,gameOverPanel ,gameOver;
int turn,pause;
char*  LeftBarrel[15] ;
char*  RightBarrel[15] ;
char*  Explosion[15] ;
double velocity ,windPower;
PROJECTILE* projectile;
TANK* tanks[2];
GROUND* ground;
//==============================================================================
// Static variables
static char windText[20] ;
static int postinghandle;
//==============================================================================
//Enums and constants 
enum status {First_Tank_Fire,Second_Tank_Fire,No_One_Fire};
enum status turn;
//==============================================================================
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((menuPanel = LoadPanel (0, "Tank_Game.uir", Menu_Panel)) < 0)
		return -1;
	if ((gamePanel = LoadPanel (0, "Tank_Game.uir", Game_Panel)) < 0)
		return -1;
	if ((controlsPanel = LoadPanel (0, "Tank_Game.uir", Controls)) < 0)
		return -1;
	if ((wmp_Panel = LoadPanel (0, "Tank_Game.uir", WMP_Panel)) < 0)
		return -1;
	if ((optionsPanel = LoadPanel (0, "Tank_Game.uir", OptionsScr)) < 0)
		return -1;
	if ((gameOverPanel = LoadPanel (0, "Tank_Game.uir", GameOver)) < 0)
		return -1;
	InstallWinMsgCallback (gamePanel, WM_KEYUP, KeyupCallback,
							VAL_MODE_IN_QUEUE, NULL, &postinghandle);
	InstallWinMsgCallback (gamePanel, WM_KEYDOWN, KeyupCallback,
							VAL_MODE_IN_QUEUE, NULL, &postinghandle);
	InstallWinMsgCallback (optionsPanel, WM_KEYDOWN, KeyupCallback,VAL_MODE_IN_QUEUE, NULL, &postinghandle);	//so ESC wiil work for options menu as well(need to be checked later)
	DisplayPanel (menuPanel);
	CmtNewThreadPool (4, &MY_THREAD_POOL);
	//------------------------Sound Configuration---------------------------------------------
																						
	Create_WMP_Handle();
	PlaySound(ThemeSong);
	SetVolume(InitialVolume);
	SetCtrlVal (menuPanel, Menu_Panel_NUMERICSLIDE, InitialVolume);
	SetCtrlVal (optionsPanel, OptionsScr_NUMERICSLIDE, InitialVolume);
	
//---------------------------------------------------------------------------------------------
	
	RunUserInterface ();
	DiscardAll();
	return 0;
}


int CVICALLBACK Start_Game (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			InitializeGame();
			StopSound();
			HidePanel(menuPanel);
			HidePanel(gameOverPanel);
			DisplayPanel(gamePanel);
			windPower = Random(-0.50,0.50);
			CmtScheduleThreadPoolFunction (MY_THREAD_POOL, DrawAllScene, NULL, &RenderingID);
			
			break;
	}
	return 0;
}

int CVICALLBACK Show_Controls (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			int visible;
			HidePanel(menuPanel);					
			DisplayPanel (controlsPanel);
			GetPanelAttribute (gamePanel, ATTR_VISIBLE, &visible);
			
			if(!visible)		//gamePanel is hidden
			{
				SetCtrlAttribute (controlsPanel, Controls_CloseControls, ATTR_VISIBLE, 0);		//hide Resume Game button(you can't resume the game if you haven't started one)
				SetCtrlAttribute (controlsPanel, Controls_Back_To_Options, ATTR_VISIBLE, 0);	//options menu is avaliable only from gamePanel
				SetCtrlAttribute (controlsPanel, Controls_Back_Button, ATTR_VISIBLE, 1);		//show back to main menu button again
			}
			else
			{
				SetCtrlAttribute (controlsPanel, Controls_CloseControls, ATTR_VISIBLE, 1);			//show Resume game butoon back
				SetCtrlAttribute (controlsPanel, Controls_Back_To_Options, ATTR_VISIBLE, 1);		//options menu is now available
				SetCtrlAttribute (controlsPanel, Controls_Back_Button, ATTR_VISIBLE, 0);			//hide back to main menu button
			}
				
			break;
	}
	return 0;
}



int CVICALLBACK QuitCallback (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			if(GenericMessagePopup ("Quit Confirmation", "Are you sure you want to quit?", "Yes", "No", 0, 0, 0, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN2)==VAL_GENERIC_POPUP_BTN1)//Yes
						QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK Back_To_Main (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			HidePanel(controlsPanel);					
			DisplayPanel(menuPanel);
			break;
	}
	return 0;
}
int CVICALLBACK KeyupCallback(int panel, int message,unsigned int* wParam,unsigned int* lParam, void* callbackData)
{
	
	switch ( message)	
	{
		case WM_KEYUP:
		
			switch(*wParam)
			{
				case VK_SPACE:		//space key
					if(!pause&&!gameOver)		//0 is unpaused
					{
				
						if(!turn)
						{
							turn = First_Tank_Fire;
							SetCtrlAttribute (gamePanel, Game_Panel_TIMER, ATTR_ENABLED, 1);
							Fire_Projectile(projectile,tanks[0]);
							pause =1 ;
							
						}	
							
						
						else
						{
							turn = Second_Tank_Fire;
							SetCtrlAttribute (gamePanel, Game_Panel_TIMER, ATTR_ENABLED, 1);
							Fire_Projectile(projectile,tanks[1]);
							pause =1 ;
							
								
						}
					turn=!turn;
					velocity = 0.00;
					windPower = Random(-0.50,0.50);
					}
				break;
			}
			break;
	
		case WM_KEYDOWN:
		
			switch (*wParam)
			{

				case VK_SPACE:		//space Key 
					if(!pause&&!gameOver)
						velocity+=5.00;	// When you hold space key velocity will increase 
					break;
				
				case VK_ESCAPE:								//Esc KEY
					if(!gameOver)
					{
						pause=!pause;
						if(pause)						//the game has paused
							DisplayPanel(optionsPanel);	//show options menu
						else							//unpause the game
							HidePanel(optionsPanel);
					}
					break;
					
				case 0x57:								//'w' Windows Virtual Key Code(it is not case-sensitive of course because it is the same key)
					if(!turn&&!pause&&!gameOver)		//active only at the turn of the left tank and when unpaused and when game is not over
					{
						tanks[0]->UpperBarrel(tanks[0]);
						tanks[0]->SetBarrelImage(tanks[0],0);
						DrawAllScene();
						
						
					}
					break;
					
				case 0x53:							//'s' Windows Virtual Key Code
					if(!turn&&!pause&&!gameOver)
					{
						tanks[0]->LowerBarrel(tanks[0]);
						tanks[0]->SetBarrelImage(tanks[0],0);
						DrawAllScene();
						
					}
					break;
					
		    	case 0x41:							//'a' Windows Virtual Key Code
					if(!turn&&!pause&&!gameOver)
					{
						tanks[0]->Move_NegX(tanks[0]);
						DrawAllScene();
					}
					break;
					
				case 0x44:							//'d' Windows Virtual Key Code
					if(!turn&&!pause&&!gameOver)
					{
						tanks[0]->Move_PosX(tanks[0]);
						DrawAllScene();
					}
					break;
							
				case 0x26:								//Arrow Up Vkey
					if(turn&&!pause&&!gameOver)
					{
						tanks[1]->LowerBarrel(tanks[1]);
						tanks[1]->SetBarrelImage(tanks[1],1);
						DrawAllScene();
					}
					break;
					
				case 0x28: 							//Arrow Down Vkey
					if(turn&&!pause&&!gameOver)
					{
						tanks[1]->UpperBarrel(tanks[1]);
						tanks[1]->SetBarrelImage(tanks[1],1);
						DrawAllScene();
						
					}
					break;
					
				case 0x25:								//Arrow Left Vkey
					if(turn&&!pause&&!gameOver)
					{
						tanks[1]->Move_NegX(tanks[1]);
						DrawAllScene();
					}
					break;
					
				case 0x27:								//Arrow Right Vkey
					if(turn&&!pause&&!gameOver)
					{
						tanks[1]->Move_PosX(tanks[1]);
						DrawAllScene();
					}
					break;
					
				case 0x4D:								//'M' key
					if(!pause&&!gameOver)
						ToggleMute();
					break;
			
			}
		break;
	
	
	}
	return 0;
}
int CVICALLBACK MyTimer (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	 
	switch (event)
	{
		case EVENT_TIMER_TICK:
			Draw_Projectile(projectile);
			
			
			break;
	}
	return 0;
}

int CVICALLBACK Mute_Callback (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			ToggleMute();
			break;
	}
	return 0;
}

int CVICALLBACK ResumeGame (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			pause=0;					//unpause the game
			HidePanel(optionsPanel);
			HidePanel(controlsPanel);	//show only game panel
			break;
	}
	return 0;
}

int CVICALLBACK ChangeVolume (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int volume;
			int visible;
			GetPanelAttribute (menuPanel, ATTR_VISIBLE, &visible);
			if(visible)														//user wants to change volume from main panel
				GetCtrlVal (menuPanel, Menu_Panel_NUMERICSLIDE, &volume);
			else															//user wants to change vokume from options menu							
				GetCtrlVal (optionsPanel, OptionsScr_NUMERICSLIDE, &volume);
			
			SetVolume(volume);
			break;
	}
	return 0;
}


int CVICALLBACK Back_To_Options (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			HidePanel(controlsPanel);
			DisplayPanel(optionsPanel);
			break;
	}
	return 0;
}




void InitializeGame()														//sets up all objects neccessary for the game
{
	InitilalizeAnimation();
	//--------------initialize paths for assets---------------------------------------------//
	for(int i=0;i<15;i++)
	{
		if(i<10)
		{
			LeftBarrel[i]=calloc(strlen("Assets\\Animation\\Tank_Left\\Tank_Get_Ready\\0_Tank_Get_Ready.png")+1,sizeof(char));
			RightBarrel[i]=calloc(strlen("Assets\\Animation\\Tank_Right\\Tank_Get_Ready\\0_Tank_Get_Ready.png")+1,sizeof(char));
			Explosion[i]=calloc(strlen("Assets\\Animation\\Explosion\\0_Explosion.png")+1,sizeof(char));
		}
		
		else
		{
			LeftBarrel[i]=calloc(strlen("Assets\\Animation\\Tank_Left\\Tank_Get_Ready\\10_Tank_Get_Ready.png")+1,sizeof(char));
			RightBarrel[i]=calloc(strlen("Assets\\Animation\\Tank_Right\\Tank_Get_Ready\\10_Tank_Get_Ready.png")+1,sizeof(char));
			Explosion[i]=calloc(strlen("Assets\\Animation\\Explosion\\10_Explosion.png")+1,sizeof(char));
		}
		sprintf(LeftBarrel[i],"Assets\\Animation\\Tank_Left\\Tank_Get_Ready\\%d_Tank_Get_Ready.png",i);
		sprintf(RightBarrel[i],"Assets\\Animation\\Tank_Right\\Tank_Get_Ready\\%d_Tank_Get_Ready.png",i);
		sprintf(Explosion[i],"Assets\\Animation\\Explosion\\%d_Explosion.png",i);
	}
	gameOver=0;
	pause=0;
	turn=0;
	tanks[0]=new_TANK(new_POSITION(10.00,390.00),0.0,100,new_Image("Assets//Animation//Tank_Left//Tank_Mouvement//01_Tank.png"));
	tanks[1]=new_TANK(new_POSITION(1700.00,390.00),180.0,100,new_Image("Assets//Animation//Tank_Right//Tank_Mouvement//01_Tank.png"));
	projectile = new_PROJECTILE(new_POSITION(2000,2000));
	ground = new_Ground(new_Image("Assets//Animation//Ground//01_Ground.jpg"));
	
}

void DiscardAll()
{
	//-----------------Discard Panels------------------------------//
	CmtDiscardThreadPool (MY_THREAD_POOL);
	DiscardPanel (menuPanel);
	DiscardPanel (gamePanel);
	DiscardPanel (controlsPanel);
	DiscardPanel (wmp_Panel);
	DiscardPanel (optionsPanel);
	DiscardPanel(gameOverPanel);
	//--------------------------------------------------------------//
	for(int i=0;i<2;i++)		//Discard tanks
		free(tanks[i]);
	
	free(projectile);			//Discrad projectile
	
	for(int i=0;i<15;i++)		//Discard path strings
	{
			free(LeftBarrel[i]);
			free(RightBarrel[i]);
			free(Explosion[i]);
	}
	
	DiscardAnimation();
	
	
}


int DrawAllScene()
{
	
	
	ground->Draw_Ground(ground);
	for(int i=0;i<2;i++)					
	{
		tanks[i]->Draw_Tank(tanks[i]);
		tanks[i]->DrawHealthBar(tanks[i]);
	}
	CanvasDefaultPen (gamePanel, Game_Panel_CANVAS);
	sprintf(windText,"Wind : %f",windPower);
	CanvasDrawTextAtPoint (gamePanel, Game_Panel_CANVAS, windText, VAL_APP_META_FONT, MakePoint(1850,50), VAL_CENTER);
	return 0;
}



