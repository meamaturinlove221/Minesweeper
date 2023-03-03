#include "Game.h"

using namespace sf;
using namespace std;

Game::Game()
{
	Window_width = 860;
	Window_height = 600;

	gamelvl = 2;
	imgBGNo = 1;
	imgSkinNo = 1;
	window.create(sf::VideoMode(Window_width, Window_height), L"ɨ�� by ������ᤢ");

	mouse_RL_ClkReady = 0;
	RL_ClkJudge_flag = false;
}


Game::~Game()
{
}

void Game::Initial()
{
	window.setFramerateLimit(10);//֡Ƶ
	gridSize = GRIDSIZE; //�����λ�õĿ�Ĵ�С

	switch (gamelvl)
	{
	case 1:
		stageWidth = LVL1_WIDTH;
		stageHeight = LVL1_HEIGHT;
		mMineNum = LVL1_NUM;
		gridSize = GRIDSIZE * LVL2_WIDTH / LVL1_WIDTH; //���Ѷȵĸ���Ҫ�Ŵ����Ѷ��б���һ��
		break;
	case 2:
		stageWidth = LVL2_WIDTH;
		stageHeight = LVL2_HEIGHT;
		mMineNum = LVL2_NUM;
		break;
	case 3:
		stageWidth = LVL3_WIDTH;
		stageHeight = LVL3_HEIGHT;
		mMineNum = LVL3_NUM;
		break;
	default:
		break;
	}
	gameOver = false;
	gameQuit = false;

	isGameOverState = ncNo;//��ʼ����Ϸ�Ľ���״̬
	mFlagCalc = 0;         //��ʼ�����ӵ�����
	isGameBegin = false;   //��ʼ����Ϸ�Ƿ�ʼ
	mTime = 0;             //��ʼ����Ϸ���е�ʱ��


	mCornPoint.x = (Window_width - stageWidth * GRIDSIZE) / 2;          //������̨�����Ͻ�����
	mCornPoint.y = (Window_height - stageHeight * GRIDSIZE) / 2;
	IniData();      //��ʼ������
	LoadMediaData();    //�����ز�
}
void Game::IniData()
{
	int i, j;

	for(j =0; j<stageHeight;j++)        //���п���Ϊ����δ���
		for (i = 0; i < stageWidth; i++)
		{
			mGameData[j][i].mState = ncUNDOWN;
			mGameData[j][i].isPress = false;
		}
}
void Game::LoadMediaData()
{
	std::stringstream ss;
	ss << "data/images/BK0" << imgBGNo << ".jpg";
	if (!tBackground.loadFromFile(ss.str()))
	{
		std::cout<<"BK image û���ҵ�" << std::endl;
	}

	ss.str("");//����ַ���
	ss << "data/images/Game" << imgSkinNo << ".jpg";
	if (!tTiles.loadFromFile(ss.str()))
	{
		std::cout << "Game Skin image û���ҵ�" << std::endl;
	}

	if (!tNum.loadFromFile("data/images/num.jpg"))
	{
		std::cout << "num.jpg û���ҵ�" << std::endl;
	}
	if (!tTimer.loadFromFile("data/images/jishiqi.jpg"))
	{
		std::cout << "jishiqi.jpg û���ҵ�" << std::endl;
	}
	if (!tCounter.loadFromFile("data/images/jishuqi.jpg"))
	{
		std::cout << "jishuqi.jpg û���ҵ�" << std::endl;
	}
	if (!tButtons.loadFromFile("data/images/button.jpg"))
	{
		std::cout << "button.jpg û���ҵ�" << std::endl;
	}
	if (!tGameOver.loadFromFile("data/images/gameover.jpg"))
	{
		std::cout << "gameover.jpg û���ҵ�" << std::endl;
	}

	sBackground.setTexture(tBackground);
	sTiles.setTexture(tTiles);
	if (gamelvl == 1)
	{
		 float scale = 1.0 * LVL2_WIDTH / LVL1_WIDTH;
		 sTiles.setScale(scale, scale); //�Ѷ� 1 ʱ��դ��ߴ�Ҫ�Ŵ�
	}
	else
	{ 
		 sTiles.setScale(1.0, 1.0);//դ��ߴ�ȡ������
    }
	sButtons.setTexture(tButtons);
	sNum.setTexture(tNum);
	sTimer.setTexture(tTimer);
	sCounter.setTexture(tCounter);
	sGameOver.setTexture(tGameOver);
}
void Game::MineSet(int Py, int Px)          //����
{
	int mCount, i, j, k, l;
	mCount = 0;

	srand(time(NULL));    //�õ�ǰϵͳʱ����Ϊ�����������������
	//�������
    do {
		bool flag = true;
		k = rand() % stageHeight;//���������
		l = rand() % stageWidth;
		for (i = Py - 1; i < Py + 2; i++) //����һ�µ��������Χ8����
			 for (j = Px - 1; j < Px + 2; j++)
			  if (i >= 0 && i < stageHeight && j >= 0 && j < stageWidth)//9����ȡ������̨�еĲ��ֽ��к����ж�
			    if (k == i && l == j)
			        flag = false;//������������� 9 ���񸲸Ƿ�Χ����Ҫ����
		if (flag && mGameData[k][l].mState == ncUNDOWN)
		{
			 mGameData[k][l].mState = ncMINE;
			 mGameData[k][l].mStateBackup = ncMINE; //����״̬
			 mCount++;
		}
	} while (mCount != mMineNum);
	

	//����ֵ
	for (i = 0; i < stageHeight; i++)
		for (j = 0; j < stageWidth; j++)
		{
			if (mGameData[i][j].mStateBackup != ncMINE)
			{
				mCount = 0;
				for (k = i - 1;k < i + 2;k++)
					for  (l = j - 1; l < j + 2;l++)
						if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
						{
							if (mGameData[k][l].mStateBackup == ncMINE)
								mCount++;
						}//���㣨i,j)��Χ�׵���Ŀ
				
				switch (mCount)//����״̬
				{
				    case 0:
						mGameData[i][j].mStateBackup = ncNULL;
						break;
					case 1:
						mGameData[i][j].mStateBackup = ncONE;
						break;
					case 2:
						mGameData[i][j].mStateBackup = ncTWO;
						break;
					case 3:
						mGameData[i][j].mStateBackup = ncTHREE;
						break;
					case 4:
						mGameData[i][j].mStateBackup = ncFOUR;
					    break;
					case 5:
						mGameData[i][j].mStateBackup = ncFIVE;
						break;
					case 6:
						mGameData[i][j].mStateBackup = ncSIX;
						break;
					case 7:
						mGameData[i][j].mStateBackup = ncSEVEN;
						break;
					case 8:
						mGameData[i][j].mStateBackup = ncEIGHT;
						break;
				/*default:
					break;*/
				}
				/*if (mGameData[i][j].mState != ncFLAG && mGameData[k][l].mState != ncQ)
				   mGameData[i][j].mState = mGameData[i][j].mStateBackup;*/
			}
			
		}
}
void Game::RL_ButtonDown(Vector2i mPoint) //������һ�
 {
	 int i, j, k, l;
	 i = (mPoint.x - mCornPoint.x) / gridSize;
	 j = (mPoint.y - mCornPoint.y) / gridSize;
	
	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight)
	{
		if (mGameData[j][i].isPress == true)//����ѱ����
		{
			if (mGameData[j][i].mState != ncFLAG && mGameData[j][i].mState != ncQ)
			 for (k = j - 1; k < j + 2; k++)
			  for (l = i - 1; l < i + 2; l++)//������Χ 8 ������
				if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
				{
				  if (mGameData[k][l].isPress == false) 
				  {
					  mGameData[k][l].isPress = true;
					//mGameData[k][l].mStateBackUp = mGameData[k][l].mState;
					  mGameData[k][l].mState = ncX;
				  }
				}
		}
		else//΢�����ʹ�� RL ˫�������ȼ������񡱸��ڵ���
		{
			  for (k = j - 1; k < j + 2; k++)
				 for (l = i - 1; l < i + 2; l++)//������Χ 8 ������
				 if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
				 {
				     if (mGameData[k][l].isPress == false)
					 {
					     mGameData[k][l].isPress = true;
					    //mGameData[k][l].mStateBackUp = mGameData[k][l].mState;
						 mGameData[k][l].mState = ncX;
					 }
				 }
			
			
				 mGameData[j][i].isPress = false;//΢����򣬵����״̬����
		}
    }
	RL_Point = mPoint;
	RL_ClkJudge_flag = true;
}
void Game::RL_ClkJudge()//���Ҽ�˫�����ж� 
 {
	 int i, j, k, l, mineNum = 0, flagNum = 0;
	 i = (RL_Point.x - mCornPoint.x) / gridSize;
	 j = (RL_Point.y - mCornPoint.y) / gridSize;
	
		 if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight)
		 {
		   if (mGameData[j][i].isPress == true)//����ѱ����
		   {
			 if (mGameData[j][i].mState != ncFLAG)//�����ǰ�鲻������
				 for (k = j - 1; k < j + 2; k++)
				 for (l = i - 1; l < i + 2; l++)//������Χ 8 ������
				 if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
				 {
				   if (mGameData[k][l].mState == ncFLAG)
					 flagNum++;
				   if (mGameData[k][l].mState == ncX)
				   {
					 mGameData[k][l].isPress = false;
					 mGameData[k][l].mState = mGameData[k][l].mStateBackup;
				   }
				 }
		   }
		   if (mGameData[j][i].mState == flagNum + 2)//���� 1-8 ��Ӧ����� 3-10
			   LButtonDblClk(RL_Point);
		 }
	
		 RL_ClkJudge_flag = false;
}
void Game::Input()
{
  sf::Event event;//event types ����indow,Keyboard,Mouse,Joystick, 4����Ϣ
                    //ͨ�� bool Window :: polEvent (sf :: Event&event) �Ӵ���˳��ѯ��( polled )�¼���
                    //�����һ���¼��ȴ������ú���������true, �����¼����������(filled) �¼����ݡ�
                    //������ǣ���ú�������false. ͬ����Ҫ����Ҫע�⣬һ�ο����ж���¼�:������Ǳ���ȷ

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			gameQuit = true;
		}
		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
		{
			window.close();
			gameQuit = true;
		}
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			 if (isGameOverState == ncNo)
			 {
				 if (mouseClickTimer.getElapsedTime().asMilliseconds() > 300)
					 LButtonDown(Mouse::getPosition(window));//�����ε���ļ������ 300 ���룬���ж�Ϊ��굥��
				 else
					 LButtonDblClk(Mouse::getPosition(window));//�����ε���ļ��С�� 300 ���룬���ж�Ϊ���˫��
			 }
		}
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
			if (isGameOverState == ncNo)
			    mouseClickTimer.restart();
	    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
		    if (isGameOverState == ncNo)
			    RButtonDown(Mouse::getPosition(window));//-----������һ�
	}	
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			P2 = Mouse::getPosition(window);
			if (isGameOverState == ncNo)
			{
				if (mouseClickTimer.getElapsedTime().asMicroseconds() < 500 && P2.x - P1.x < gridSize / 4 && P2.y - P1.y < gridSize / 4 && mouseDlbClkReady)
			    {
				    LButtonDblClk(P2);//�����ε���ļ������300���룬���ж�Ϊ��굥��
					mouseDlbClkReady = false;
				}
			    else
			    {
					LButtonDown(P2);//�����ε���ļ������300���룬���ж�Ϊ��굥��
					mouseDlbClkReady = true;
					mouse_RL_ClkReady++; //���������RL˫��׼��
					if (mouse_RL_ClkReady == 2)
						RL_ButtonDown(Mouse::getPosition(window));//-----��������һ�
			    }
			}
        }
		
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
		{
			if (isGameOverState == ncNo)
			{
				mouseClickTimer.restart();//SFML��clock�����������getElapsedTime()��releaseElapsedTime()
			    //��ť�ж�
					if (isGameBegin == false)
					{
						if (ButtonRectEasy.contains(event.mouseButton.x,event.mouseButton.y))
							gamelvl = 1;
						if (ButtonRectNormal.contains(event.mouseButton.x, event.mouseButton.y))
							gamelvl = 2;
						if (ButtonRectHard.contains(event.mouseButton.x, event.mouseButton.y))
							gamelvl = 3;
						Initial();//��ʱˢ����̨
					}
			}
			if (ButtonRectBG.contains(event.mouseButton.x,event.mouseButton.y))
			{
				imgBGNo++;
				if (imgBGNo > 7)//���ڱ���ͼ������ʱ��
					imgBGNo = 1;//�����ֻ�����ͼ
				LoadMediaData();
			}
			if (ButtonRectSkin.contains(event.mouseButton.x, event.mouseButton.y))
            {
				imgSkinNo++;
				if (imgSkinNo > 6)//����Ƥ��ͼ������ʱ��
					imgSkinNo = 1;//�����ֻ�Ƥ��ͼ
				LoadMediaData();
            }
			if (ButtonRectRestart.contains(event.mouseButton.x, event.mouseButton.y))
			{
				Initial();
			}
			if (ButtonRectQuit.contains(event.mouseButton.x, event.mouseButton.y))
			{
				window.close();
				gameQuit = true;
			}

	    }
	    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
	    {
		  if (isGameOverState == ncNo)
			  RButtonDown(Mouse::getPosition(window)); // ----����һ�
	    }
	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
	{
		 if (isGameOverState == ncNo)
		 {
			 mouse_RL_ClkReady++;
			
			 if (mouse_RL_ClkReady == 2)
				 RL_ButtonDown(Mouse::getPosition(window));//-----��������һ�
			 else
				 RButtonDown(Mouse::getPosition(window));//-----������һ�
		 }
	}

	if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
	{
		if (isGameBegin == false)
		{
			if (ButtonRectQuit.contains(event.mouseMove.x,event.mouseMove.y))
			{
				ButtonQuitState = true;
			} 
			else
			{
				ButtonQuitState = false;
			}
		}

	}
	
	if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
	{
		mouse_RL_ClkReady = 0;
	}
}

void Game::RButtonDown(Vector2i mPoint)     //����һ�
{
	int i, j;
	i = (mPoint.x - mCornPoint.x) / gridSize;
	j = (mPoint.y - mCornPoint.y) / gridSize;
	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight)//������������̨��Χ��
	{
		if (isGameBegin == false)//�����Ϸδ��ʼ
		{
			 isGameBegin = true;//��Ϸ��ʼ
			 gameClock.restart();
		}
		if (mGameData[j][i].isPress == false)
		{
			mGameData[j][i].isPress = true;
			mGameData[j][i].mStateBackup = mGameData[j][i].mState;
			mGameData[j][i].mState = ncFLAG;
			mFlagCalc++;
		}
		else
		{
			if (mGameData[j][i].mState == ncFLAG)
			{
				mGameData[j][i].isPress = true;
				mGameData[j][i].mState = ncQ;
				mFlagCalc--;
			}
			else if (mGameData[j][i].mState == ncQ)
			{
				mGameData[j][i].isPress = false;
				mGameData[j][i].mState = mGameData[j][i].mStateBackup;
			}
		}
	}
}



void Game::LButtonDown(Vector2i mPoint)   //������
{
	int i, j;
	i = (mPoint.x - mCornPoint.x) / gridSize;//��ȡ��굱ǰ����Ŀ��λ��
	j = (mPoint.y - mCornPoint.y) / gridSize;
	
	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight) //������������̨
	{
		if (isGameBegin == false)//�����Ϸδ��ʼ
		{
			isGameBegin = true;//��Ϸ��ʼ
			gameClock.restart();
		}
		if (isMineSetBegin == false)//���δ����
		{
			isMineSetBegin = true;
			MineSet(j, i); //��ʼ�������
		}
		if (mGameData[j][i].mState != ncFLAG)//���״̬��������
			if (mGameData[j][i].isPress == false)
			{
				mGameData[j][i].isPress = true; //��ǰ�鱻���
				if (mGameData[j][i].mState == ncMINE) //�����ǰΪ��
				{
					isGameBegin = false;
					isGameOverState = ncLOSE;
					mGameData[j][i].mState = ncBOMBING;
					unCover(); //�ҿ�ʣ��δ���ҵ�����
				}
			
			    if (mGameData[j][i].mState == ncNULL)//�����ǰ��Ϊ��
				    NullClick(j, i);         //����δ������Ŀտ�            
			}
	}
}
void Game::NullClick(int j, int i)    //���ҿտ�
{
	int k, l;
	for(k = j - 1;k < j + 2;k++)
		for(l = i - 1;l < i + 2; l++)
			if (k >= 0 && k < stageHeight && l >= 0 && l <stageWidth)
			{
				if (mGameData[k][l].isPress == false)
				{	
					mGameData[k][l].isPress = true;
					if (mGameData[k][l].mState == ncNULL)   //���״̬Ϊ��
						NullClick(k,l);                     //�ݹ���ã���������
				}
			}
}
void Game::LButtonDblClk(Vector2i mPoint)   //������˫��
{
	int i, j, k, l;
	i = (mPoint.x - mCornPoint.x) / gridSize;
	j = (mPoint.y - mCornPoint.y) / gridSize;

	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight) //���������ڷ�Χ��
	{
		if (mGameData[j][i].isPress == true)//����ѱ����
		{
			if (mGameData[j][i].mState != ncFLAG)//�����ǰ�鲻������
				for (k = j - 1;k < j + 2;k++)
					for(l = i-1;l < i+2;l++)//������Χ8������
						if (k >= 0 && k < stageHeight && 1 >= 0 && 1 < stageWidth)
						{
							if (mGameData[k][l].mState == ncFLAG) //���״̬������
							{
								if (mGameData[k][l].mStateBackup != ncMINE)//���ԭ��״̬������
								{
									isGameOverState = ncLOSE;
									isGameBegin = false;
									unCover();
								}
							}
							else //���״̬��������
							{
								if (mGameData[k][1].isPress == false)
								{
									mGameData[k][1].isPress = true;
									if (mGameData[k][1].mState == ncMINE)//���Ϊ��
									{
										isGameOverState = ncLOSE;
										isGameBegin = false;
										mGameData[k][1].mState = ncBOMBING;
										unCover();
									}
									if (mGameData[k][l].mState == ncNULL)//���Ϊ�տ�
										NullClick(k, l);
								}
							}
						}
		}
	}
}

void Game::Logic()
{
	if (mouse_RL_ClkReady == 0 && RL_ClkJudge_flag == true)
		RL_ClkJudge();

	isWin();

}
void Game::undownOpen()
{
	int i, j;
	for (j = 0; j < stageHeight; j++)
		for (i = 0; i < stageWidth; i++)
		{
		  if (mGameData[j][i].isPress == false)
		  {
			  mGameData[j][i].isPress = true;
			 if (mGameData[j][i].mState == ncMINE)
				 mGameData[j][i].mState = ncFLAG;
		  }
		
		}
}
void Game::isWin()
{
	int i, j, c = 0;
	if (isGameOverState != ncLOSE)
	{
		for (i = 0; i < stageWidth; i++)
			for (j = 0; j < stageHeight; j++)
			{
				if (mGameData[j][i].isPress == false || mGameData[j][i].mState == ncFLAG)
					c++;
			}
	}
	
	if (c == mMineNum)//����������Ӷ�Ӧ�����ף���Ϸʤ��������
	{
		 isGameBegin = false;
		 mFlagCalc = mMineNum;
		 undownOpen();
		 isGameOverState = ncWIN;
	}
}
void Game::unCover()
{
	
	for (int j = 0;j < stageHeight;j++)
		for (int i = 0; i < stageWidth; i++)
		{
			if(mGameData[j][i].isPress == false)
				if (mGameData[j][i].mState == ncMINE)
				{
					mGameData[j][i].isPress = true;
					mGameData[j][i].mState = ncUNFOUND;
				}
		}

}
void Game::Draw()
{
	window.clear();//����

	//���Ʊ���
	sBackground.setPosition(0, 0);
	window.draw(sBackground);

	//������̨
	DrawGrid();
	DrawButton();
	DrawTimer();
	DrawScore();

	if (isGameOverState)
		DrawGameEnd();

	window.display();//����ʾ�����������ݣ���ʾ����Ļ�ϡ�SFML���õ���˫�������
}
void Game::DrawGrid()
{
	int i, j;
	for (j = 0;j < stageHeight; j++)
		for (i = 0; i < stageWidth; i++)
		{
			if (mGameData[j][i].isPress == true)
			{
				sTiles.setTextureRect(IntRect(mGameData[j][i].mState * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
				sTiles.setPosition(mCornPoint.x + i * GRIDSIZE, mCornPoint.y + j * GRIDSIZE);
			}
			else
			{
				sTiles.setTextureRect(IntRect(ncUNDOWN * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));				
			    sTiles.setPosition(mCornPoint.x + i * GRIDSIZE, mCornPoint.y + j * GRIDSIZE);
			}
			window.draw(sTiles);
		}

}
void Game::DrawButton()
{
	Vector2i LeftCorner;
	int ButtonWidth = 60;
	int ButtonHeight = 36;
	int detaX;
	detaX = (Window_width - ButtonWidth * 7) / 8;//7����ť�ڽ����ϵȷֿ��
	LeftCorner.y = Window_height - GRIDSIZE * 3;       //ָ���߶�

	//ButtonRectEasy
	LeftCorner.x = detaX;
	sButtons.setTextureRect(IntRect(0 * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(LeftCorner.x, LeftCorner.y);
	ButtonRectEasy.left = LeftCorner.x;
	ButtonRectEasy.top = LeftCorner.y;
	ButtonRectEasy.width = ButtonWidth;
	ButtonRectEasy.height = ButtonHeight;
	window.draw(sButtons);
	//ButtonRectNormal
	LeftCorner.x = detaX*2 + ButtonWidth;
	sButtons.setTextureRect(IntRect(1 * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(LeftCorner.x, LeftCorner.y);
	ButtonRectNormal.left = LeftCorner.x;
	ButtonRectNormal.top = LeftCorner.y;
	ButtonRectNormal.width = ButtonWidth;
	ButtonRectNormal.height = ButtonHeight;
	window.draw(sButtons);
	//ButtonRectHard
	LeftCorner.x = detaX*3 + ButtonWidth*2;
	sButtons.setTextureRect(IntRect(2 * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(LeftCorner.x, LeftCorner.y);
	ButtonRectHard.left = LeftCorner.x;
	ButtonRectHard.top = LeftCorner.y;
	ButtonRectHard.width = ButtonWidth;
	ButtonRectHard.height = ButtonHeight;
	window.draw(sButtons);
	//ButtonRectBG
	LeftCorner.x = detaX * 4 + ButtonWidth*3;
	sButtons.setTextureRect(IntRect(3 * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(LeftCorner.x, LeftCorner.y);
	ButtonRectBG.left = LeftCorner.x;
	ButtonRectBG.top = LeftCorner.y;
	ButtonRectBG.width = ButtonWidth;
	ButtonRectBG.height = ButtonHeight;
	window.draw(sButtons);
	//ButtonRectSkin
	LeftCorner.x = detaX * 5 + ButtonWidth*4;
	sButtons.setTextureRect(IntRect(4 * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(LeftCorner.x, LeftCorner.y);
	ButtonRectSkin.left = LeftCorner.x;
	ButtonRectSkin.top = LeftCorner.y;
	ButtonRectSkin.width = ButtonWidth;
	ButtonRectSkin.height = ButtonHeight;
	window.draw(sButtons);
	//ButtonRectRestart
	LeftCorner.x = detaX * 6 + ButtonWidth*5;
	sButtons.setTextureRect(IntRect(5 * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(LeftCorner.x, LeftCorner.y);
	ButtonRectRestart.left = LeftCorner.x;
	ButtonRectRestart.top = LeftCorner.y;
	ButtonRectRestart.width = ButtonWidth;
	ButtonRectRestart.height = ButtonHeight;
	window.draw(sButtons);
	//ButtonRectQuit
	LeftCorner.x = detaX * 7 + ButtonWidth*6;
	sButtons.setTextureRect(IntRect(6 * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(LeftCorner.x, LeftCorner.y);
	ButtonRectQuit.left = LeftCorner.x;
	ButtonRectQuit.top = LeftCorner.y;
	ButtonRectQuit.width = ButtonWidth;
	ButtonRectQuit.height = ButtonHeight;
	window.draw(sButtons);
}
void Game::DrawScore()
{
	Vector2i LeftCorner;
	LeftCorner.x = Window_width - sCounter.getLocalBounds().width *  1.25;
	LeftCorner.y = sCounter.getLocalBounds().height * 0.5;
	sCounter.setPosition(LeftCorner.x, LeftCorner.y);   //�������������ͼλ��
	window.draw(sCounter);

	int NumSize = sNum.getLocalBounds().height;
	LeftCorner.x = LeftCorner.x + sCounter.getLocalBounds().width - NumSize;
	LeftCorner.y = LeftCorner.y + sCounter.getLocalBounds().height * 0.5 - NumSize*0.5;

	int mScore = mMineNum - mFlagCalc;
	//���Ƹ�λ��������
	int a = mScore % 10;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));        //����ͼ��
	sNum.setPosition(LeftCorner.x, LeftCorner.y);
	window.draw(sNum);
	//����ʮλ��������
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));        //����ͼ��
	sNum.setPosition(LeftCorner.x, LeftCorner.y);
	window.draw(sNum);
	//���ư�λ��������
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));        //����ͼ��
	sNum.setPosition(LeftCorner.x, LeftCorner.y);
	window.draw(sNum);
}
void Game::DrawTimer()
{
	Vector2i LeftCorner;
	LeftCorner.x = sTimer.getLocalBounds().width * 0.25;
	LeftCorner.y = sTimer.getLocalBounds().height * 0.5;
	sTimer.setPosition(LeftCorner.x, LeftCorner.y); //�������������ͼλ��
	window.draw(sTimer);
	if (isGameBegin)
		mTime = gameClock.getElapsedTime().asSeconds();
	int mScore = mTime;
	if (mScore > 999)
		mScore = 999;
	int NumSize = sNum.getLocalBounds().height;
	LeftCorner.x = LeftCorner.x + sTimer.getLocalBounds().width - NumSize * 1.5;
	LeftCorner.y = LeftCorner.y + sTimer.getLocalBounds().height * 0.5 - NumSize * 0.5;
	//���Ƹ�λ��������
	int a = mScore % 10;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));//������ȡ��������
	sNum.setPosition(LeftCorner.x, LeftCorner.y);//�ں�λ��
	window.draw(sNum);
	//����ʮλ��������
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));//������ȡ��������
	sNum.setPosition(LeftCorner.x, LeftCorner.y);//�ں�λ��
	window.draw(sNum);
	//���ư�λ��������
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));//������ȡ��������
	sNum.setPosition(LeftCorner.x, LeftCorner.y);//�ں�λ��
	window.draw(sNum);
}
void Game::DrawGameEnd()
{
	Vector2i LeftCorner;
	int ButtonWidth = 200;
	int ButtonHeight = sGameOver.getLocalBounds().height;
	LeftCorner.x = (Window_width - ButtonWidth) / 2; //ָ����������
	LeftCorner.y = (Window_height - ButtonHeight) / 2;//ָ����������
	
	sGameOver.setPosition(LeftCorner.x, LeftCorner.y); //���ð�ť��λ������
	
	if (isGameOverState == ncWIN)
		sGameOver.setTextureRect(IntRect(0 * ButtonWidth, 0, ButtonWidth, ButtonHeight)); //��ȡ��ť����������
	if (isGameOverState == ncLOSE)
		sGameOver.setTextureRect(IntRect(1 * ButtonWidth, 0, ButtonWidth, ButtonHeight)); //��ȡ��ť����������
	
	window.draw(sGameOver);
}
void Game::Run()
{
	do 
	{
		Initial();
		while (window.isOpen() && gameOver == false)
		{
			Input();

			Logic();

			Draw();
		}
	} while (!gameQuit);

}