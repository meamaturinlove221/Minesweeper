#pragma once
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<windows.h>
#include<iostream>
#include<sstream>
#define GRIDSIZE 25
#define LVL1_WIDTH 9
#define LVL1_HEIGHT 9
#define LVL1_NUM 10
#define LVL2_WIDTH 16
#define LVL2_HEIGHT 16
#define LVL2_NUM 40
#define LVL3_WIDTH 30
#define LVL3_HEIGHT 16
#define LVL3_NUM 99
//ö�ٶ�������״̬
typedef enum GRIDSTATE
{
	ncNULL,          //�յ�
	ncUNDOWN,        //��������
	ncMINE,          //����
	ncONE,           //����1
	ncTWO,           //����2
	ncTHREE,         //����3
	ncFOUR,          //����4
	ncFIVE,          //����5
	ncSIX,           //����6
	ncSEVEN,         //����7
	ncEIGHT,         //����8
	ncFLAG,          //���
	ncQ,             //�ʺ�
	ncX,             //����
	ncBOMBING,       //��ը����
	ncUNFOUND        //δ����������
};
typedef enum GAMEOVERSTATE
{
	ncNo,       //��Ϸδ����
	ncWIN,      //��Ϸʤ��
	ncLOSE,     //��Ϸʧ��
};
using namespace sf;

class LEI
{
public:
	int mState;            //դ���״̬
	int mStateBackup;      //դ��ԭʼ״̬����
	bool isPress;          //դ���Ƿ񱻵��
	bool isPressBackUp;
};
class Game
{
public:
	sf::RenderWindow window;
	Game();
	 ~Game();
	 bool gameOver,gameQuit,ButtonQuitState;
	 bool mouseDlbClkReady, RL_ClkJudge_flag;
	 int Window_width, Window_height,stageWidth,stageHeight,mMineNum, mFlagCalc;
	 int gamelvl, mTime;  //��Ϸ�Ѷȣ���Ϸ��ʱ
	 int mouse_RL_ClkReady;
	 LEI mGameData[LVL3_HEIGHT][LVL3_WIDTH];    //����ȡ����Ѷȵ���̨�ߴ� 
	 bool isGameBegin;        //��Ϸ�Ƿ�ʼ
	 bool isMineSetBegin;
	 int isGameOverState;     //��Ϸ������״̬
	 sf::Vector2i mCornPoint;     //��̨���󶥵�����
	 Vector2i P1, P2;  //����λ�ÿ��ٵ������˫����BUG�޶�
	 Vector2i RL_Point;
	 
	 int gridSize;    //��Ĵ�С
	 int imgBGNo, imgSkinNo;

	 Texture tBackground, tTiles, tButtons, tNum,tTimer,tCounter, tGameOver;  //�����������
	 Sprite sBackground,sTiles, sButtons, sNum,sTimer, sCounter, sGameOver;    //�����������
     sf::IntRect ButtonRectEasy, ButtonRectNormal, ButtonRectHard, ButtonRectBG, ButtonRectSkin, ButtonRectRestart, ButtonRectQuit;
	 
	 SoundBuffer sbWin, sbBoom;
	 Sound soundWin, soundBoom;
	 Music bkMusic;

	 sf::Clock gameClock,mouseClickTimer;	

	 void Run();
	 void Initial();
	 void IniData();
	 void LoadMediaData();
	 void MineSet(int Py,int Px);          //����

	 void Input();
	 void RButtonDown(Vector2i mPoint);    //����һ�
	 void LButtonDown(Vector2i mPoint);    //������
	 void LButtonDblClk(Vector2i mPoint);   //������˫��
	 void RL_ButtonDown(Vector2i mPoint); //������һ�
	 void RL_ClkJudge();
	 void undownOpen();

	 void NullClick(int j, int i);    //���ҿտ�

	 void Logic();
	 void isWin();
	 void unCover();
	 
	 void Draw();
	 void DrawGrid();
	 void DrawButton();
	 void DrawScore();
	 void DrawTimer();
	 void DrawGameEnd();
};
