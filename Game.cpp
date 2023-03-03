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
	window.create(sf::VideoMode(Window_width, Window_height), L"扫雷 by かぐらめあ");

	mouse_RL_ClkReady = 0;
	RL_ClkJudge_flag = false;
}


Game::~Game()
{
}

void Game::Initial()
{
	window.setFramerateLimit(10);//帧频
	gridSize = GRIDSIZE; //点击的位置的块的大小

	switch (gamelvl)
	{
	case 1:
		stageWidth = LVL1_WIDTH;
		stageHeight = LVL1_HEIGHT;
		mMineNum = LVL1_NUM;
		gridSize = GRIDSIZE * LVL2_WIDTH / LVL1_WIDTH; //简单难度的格子要放大，与难度中保持一致
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

	isGameOverState = ncNo;//初始化游戏的结束状态
	mFlagCalc = 0;         //初始化旗子的数量
	isGameBegin = false;   //初始化游戏是否开始
	mTime = 0;             //初始化游戏进行的时间


	mCornPoint.x = (Window_width - stageWidth * GRIDSIZE) / 2;          //设置舞台的左上角坐标
	mCornPoint.y = (Window_height - stageHeight * GRIDSIZE) / 2;
	IniData();      //初始化数据
	LoadMediaData();    //加载素材
}
void Game::IniData()
{
	int i, j;

	for(j =0; j<stageHeight;j++)        //所有块置为空且未点击
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
		std::cout<<"BK image 没有找到" << std::endl;
	}

	ss.str("");//清空字符串
	ss << "data/images/Game" << imgSkinNo << ".jpg";
	if (!tTiles.loadFromFile(ss.str()))
	{
		std::cout << "Game Skin image 没有找到" << std::endl;
	}

	if (!tNum.loadFromFile("data/images/num.jpg"))
	{
		std::cout << "num.jpg 没有找到" << std::endl;
	}
	if (!tTimer.loadFromFile("data/images/jishiqi.jpg"))
	{
		std::cout << "jishiqi.jpg 没有找到" << std::endl;
	}
	if (!tCounter.loadFromFile("data/images/jishuqi.jpg"))
	{
		std::cout << "jishuqi.jpg 没有找到" << std::endl;
	}
	if (!tButtons.loadFromFile("data/images/button.jpg"))
	{
		std::cout << "button.jpg 没有找到" << std::endl;
	}
	if (!tGameOver.loadFromFile("data/images/gameover.jpg"))
	{
		std::cout << "gameover.jpg 没有找到" << std::endl;
	}

	sBackground.setTexture(tBackground);
	sTiles.setTexture(tTiles);
	if (gamelvl == 1)
	{
		 float scale = 1.0 * LVL2_WIDTH / LVL1_WIDTH;
		 sTiles.setScale(scale, scale); //难度 1 时候，栅格尺寸要放大
	}
	else
	{ 
		 sTiles.setScale(1.0, 1.0);//栅格尺寸取消缩放
    }
	sButtons.setTexture(tButtons);
	sNum.setTexture(tNum);
	sTimer.setTexture(tTimer);
	sCounter.setTexture(tCounter);
	sGameOver.setTexture(tGameOver);
}
void Game::MineSet(int Py, int Px)          //布雷
{
	int mCount, i, j, k, l;
	mCount = 0;

	srand(time(NULL));    //用当前系统时问作为随机数生成器的种子
	//随机布雷
    do {
		bool flag = true;
		k = rand() % stageHeight;//生成随机数
		l = rand() % stageWidth;
		for (i = Py - 1; i < Py + 2; i++) //鼠标第一下点击处及周围8领域
			 for (j = Px - 1; j < Px + 2; j++)
			  if (i >= 0 && i < stageHeight && j >= 0 && j < stageWidth)//9宫格取处于舞台中的部分进行后续判定
			    if (k == i && l == j)
			        flag = false;//随机坐标若处于 9 宫格覆盖范围，则不要布雷
		if (flag && mGameData[k][l].mState == ncUNDOWN)
		{
			 mGameData[k][l].mState = ncMINE;
			 mGameData[k][l].mStateBackup = ncMINE; //备份状态
			 mCount++;
		}
	} while (mCount != mMineNum);
	

	//方格赋值
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
						}//计算（i,j)周围雷的数目
				
				switch (mCount)//保存状态
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
void Game::RL_ButtonDown(Vector2i mPoint) //鼠标左右击
 {
	 int i, j, k, l;
	 i = (mPoint.x - mCornPoint.x) / gridSize;
	 j = (mPoint.y - mCornPoint.y) / gridSize;
	
	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight)
	{
		if (mGameData[j][i].isPress == true)//如果已被点击
		{
			if (mGameData[j][i].mState != ncFLAG && mGameData[j][i].mState != ncQ)
			 for (k = j - 1; k < j + 2; k++)
			  for (l = i - 1; l < i + 2; l++)//遍历周围 8 个格子
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
		else//微软规则，使得 RL 双击的优先级“好像”高于单击
		{
			  for (k = j - 1; k < j + 2; k++)
				 for (l = i - 1; l < i + 2; l++)//遍历周围 8 个格子
				 if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
				 {
				     if (mGameData[k][l].isPress == false)
					 {
					     mGameData[k][l].isPress = true;
					    //mGameData[k][l].mStateBackUp = mGameData[k][l].mState;
						 mGameData[k][l].mState = ncX;
					 }
				 }
			
			
				 mGameData[j][i].isPress = false;//微软规则，点击格，状态不变
		}
    }
	RL_Point = mPoint;
	RL_ClkJudge_flag = true;
}
void Game::RL_ClkJudge()//左右键双击的判定 
 {
	 int i, j, k, l, mineNum = 0, flagNum = 0;
	 i = (RL_Point.x - mCornPoint.x) / gridSize;
	 j = (RL_Point.y - mCornPoint.y) / gridSize;
	
		 if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight)
		 {
		   if (mGameData[j][i].isPress == true)//如果已被点击
		   {
			 if (mGameData[j][i].mState != ncFLAG)//如果当前块不是旗子
				 for (k = j - 1; k < j + 2; k++)
				 for (l = i - 1; l < i + 2; l++)//遍历周围 8 个格子
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
		   if (mGameData[j][i].mState == flagNum + 2)//数字 1-8 对应编号是 3-10
			   LButtonDblClk(RL_Point);
		 }
	
		 RL_ClkJudge_flag = false;
}
void Game::Input()
{
  sf::Event event;//event types 包括indow,Keyboard,Mouse,Joystick, 4类消息
                    //通过 bool Window :: polEvent (sf :: Event&event) 从窗口顺序询问( polled )事件。
                    //如果有一个事件等待处理，该函数将返回true, 并且事件变量将填充(filled) 事件数据。
                    //如果不是，则该函数返回false. 同样重要的是要注意，一次可能有多个事件:因此我们必须确

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
					 LButtonDown(Mouse::getPosition(window));//当两次点击的间隔大于 300 毫秒，则判定为鼠标单击
				 else
					 LButtonDblClk(Mouse::getPosition(window));//当两次点击的间隔小于 300 毫秒，则判定为鼠标双击
			 }
		}
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
			if (isGameOverState == ncNo)
			    mouseClickTimer.restart();
	    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
		    if (isGameOverState == ncNo)
			    RButtonDown(Mouse::getPosition(window));//-----》鼠标右击
	}	
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			P2 = Mouse::getPosition(window);
			if (isGameOverState == ncNo)
			{
				if (mouseClickTimer.getElapsedTime().asMicroseconds() < 500 && P2.x - P1.x < gridSize / 4 && P2.y - P1.y < gridSize / 4 && mouseDlbClkReady)
			    {
				    LButtonDblClk(P2);//当两次点击的间隔大于300毫秒，则判定为鼠标单击
					mouseDlbClkReady = false;
				}
			    else
			    {
					LButtonDown(P2);//当两次点击的间隔大于300毫秒，则判定为鼠标单击
					mouseDlbClkReady = true;
					mouse_RL_ClkReady++; //左键按下则RL双击准备
					if (mouse_RL_ClkReady == 2)
						RL_ButtonDown(Mouse::getPosition(window));//-----》鼠标左右击
			    }
			}
        }
		
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
		{
			if (isGameOverState == ncNo)
			{
				mouseClickTimer.restart();//SFML的clock类就两个函数getElapsedTime()和releaseElapsedTime()
			    //按钮判断
					if (isGameBegin == false)
					{
						if (ButtonRectEasy.contains(event.mouseButton.x,event.mouseButton.y))
							gamelvl = 1;
						if (ButtonRectNormal.contains(event.mouseButton.x, event.mouseButton.y))
							gamelvl = 2;
						if (ButtonRectHard.contains(event.mouseButton.x, event.mouseButton.y))
							gamelvl = 3;
						Initial();//及时刷新舞台
					}
			}
			if (ButtonRectBG.contains(event.mouseButton.x,event.mouseButton.y))
			{
				imgBGNo++;
				if (imgBGNo > 7)//大于背景图的总数时候
					imgBGNo = 1;//重新轮换背景图
				LoadMediaData();
			}
			if (ButtonRectSkin.contains(event.mouseButton.x, event.mouseButton.y))
            {
				imgSkinNo++;
				if (imgSkinNo > 6)//大于皮肤图的总数时候
					imgSkinNo = 1;//重新轮换皮肤图
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
			  RButtonDown(Mouse::getPosition(window)); // ----鼠标右击
	    }
	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
	{
		 if (isGameOverState == ncNo)
		 {
			 mouse_RL_ClkReady++;
			
			 if (mouse_RL_ClkReady == 2)
				 RL_ButtonDown(Mouse::getPosition(window));//-----》鼠标左右击
			 else
				 RButtonDown(Mouse::getPosition(window));//-----》鼠标右击
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

void Game::RButtonDown(Vector2i mPoint)     //鼠标右击
{
	int i, j;
	i = (mPoint.x - mCornPoint.x) / gridSize;
	j = (mPoint.y - mCornPoint.y) / gridSize;
	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight)//如果点击是在舞台范围内
	{
		if (isGameBegin == false)//如果游戏未开始
		{
			 isGameBegin = true;//游戏开始
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



void Game::LButtonDown(Vector2i mPoint)   //鼠标左击
{
	int i, j;
	i = (mPoint.x - mCornPoint.x) / gridSize;//获取鼠标当前点击的块的位置
	j = (mPoint.y - mCornPoint.y) / gridSize;
	
	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight) //如果点击是在舞台
	{
		if (isGameBegin == false)//如果游戏未开始
		{
			isGameBegin = true;//游戏开始
			gameClock.restart();
		}
		if (isMineSetBegin == false)//如果未布雷
		{
			isMineSetBegin = true;
			MineSet(j, i); //开始随机布雷
		}
		if (mGameData[j][i].mState != ncFLAG)//如果状态不是旗子
			if (mGameData[j][i].isPress == false)
			{
				mGameData[j][i].isPress = true; //当前块被点击
				if (mGameData[j][i].mState == ncMINE) //如果当前为雷
				{
					isGameBegin = false;
					isGameOverState = ncLOSE;
					mGameData[j][i].mState = ncBOMBING;
					unCover(); //揭开剩下未被找到的雷
				}
			
			    if (mGameData[j][i].mState == ncNULL)//如果当前块为空
				    NullClick(j, i);         //查找未被点击的空块            
			}
	}
}
void Game::NullClick(int j, int i)    //查找空块
{
	int k, l;
	for(k = j - 1;k < j + 2;k++)
		for(l = i - 1;l < i + 2; l++)
			if (k >= 0 && k < stageHeight && l >= 0 && l <stageWidth)
			{
				if (mGameData[k][l].isPress == false)
				{	
					mGameData[k][l].isPress = true;
					if (mGameData[k][l].mState == ncNULL)   //如果状态为空
						NullClick(k,l);                     //递归调用，继续查找
				}
			}
}
void Game::LButtonDblClk(Vector2i mPoint)   //鼠标左键双击
{
	int i, j, k, l;
	i = (mPoint.x - mCornPoint.x) / gridSize;
	j = (mPoint.y - mCornPoint.y) / gridSize;

	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight) //如果点击是在范围内
	{
		if (mGameData[j][i].isPress == true)//如果已被点击
		{
			if (mGameData[j][i].mState != ncFLAG)//如果当前块不是旗子
				for (k = j - 1;k < j + 2;k++)
					for(l = i-1;l < i+2;l++)//遍历周围8个格子
						if (k >= 0 && k < stageHeight && 1 >= 0 && 1 < stageWidth)
						{
							if (mGameData[k][l].mState == ncFLAG) //如果状态是旗子
							{
								if (mGameData[k][l].mStateBackup != ncMINE)//如果原先状态不是雷
								{
									isGameOverState = ncLOSE;
									isGameBegin = false;
									unCover();
								}
							}
							else //如果状态不是旗子
							{
								if (mGameData[k][1].isPress == false)
								{
									mGameData[k][1].isPress = true;
									if (mGameData[k][1].mState == ncMINE)//如果为雷
									{
										isGameOverState = ncLOSE;
										isGameBegin = false;
										mGameData[k][1].mState = ncBOMBING;
										unCover();
									}
									if (mGameData[k][l].mState == ncNULL)//如果为空块
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
	
	if (c == mMineNum)//如果所有旗子对应都是雷，游戏胜利并结束
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
	window.clear();//清屏

	//绘制背景
	sBackground.setPosition(0, 0);
	window.draw(sBackground);

	//绘制舞台
	DrawGrid();
	DrawButton();
	DrawTimer();
	DrawScore();

	if (isGameOverState)
		DrawGameEnd();

	window.display();//把显示缓冲区的内容，显示在屏幕上。SFML采用的是双缓冲机制
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
	detaX = (Window_width - ButtonWidth * 7) / 8;//7个按钮在界面上等分宽度
	LeftCorner.y = Window_height - GRIDSIZE * 3;       //指定高度

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
	sCounter.setPosition(LeftCorner.x, LeftCorner.y);   //计数器纹理的贴图位置
	window.draw(sCounter);

	int NumSize = sNum.getLocalBounds().height;
	LeftCorner.x = LeftCorner.x + sCounter.getLocalBounds().width - NumSize;
	LeftCorner.y = LeftCorner.y + sCounter.getLocalBounds().height * 0.5 - NumSize*0.5;

	int mScore = mMineNum - mFlagCalc;
	//绘制个位数的数字
	int a = mScore % 10;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));        //在贴图上
	sNum.setPosition(LeftCorner.x, LeftCorner.y);
	window.draw(sNum);
	//绘制十位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));        //在贴图上
	sNum.setPosition(LeftCorner.x, LeftCorner.y);
	window.draw(sNum);
	//绘制百位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));        //在贴图上
	sNum.setPosition(LeftCorner.x, LeftCorner.y);
	window.draw(sNum);
}
void Game::DrawTimer()
{
	Vector2i LeftCorner;
	LeftCorner.x = sTimer.getLocalBounds().width * 0.25;
	LeftCorner.y = sTimer.getLocalBounds().height * 0.5;
	sTimer.setPosition(LeftCorner.x, LeftCorner.y); //计数器纹理的贴图位置
	window.draw(sTimer);
	if (isGameBegin)
		mTime = gameClock.getElapsedTime().asSeconds();
	int mScore = mTime;
	if (mScore > 999)
		mScore = 999;
	int NumSize = sNum.getLocalBounds().height;
	LeftCorner.x = LeftCorner.x + sTimer.getLocalBounds().width - NumSize * 1.5;
	LeftCorner.y = LeftCorner.y + sTimer.getLocalBounds().height * 0.5 - NumSize * 0.5;
	//绘制个位数的数字
	int a = mScore % 10;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));//纹理上取数字纹理
	sNum.setPosition(LeftCorner.x, LeftCorner.y);//摆好位置
	window.draw(sNum);
	//绘制十位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));//纹理上取数字纹理
	sNum.setPosition(LeftCorner.x, LeftCorner.y);//摆好位置
	window.draw(sNum);
	//绘制百位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect(a * NumSize, 0, NumSize, NumSize));//纹理上取数字纹理
	sNum.setPosition(LeftCorner.x, LeftCorner.y);//摆好位置
	window.draw(sNum);
}
void Game::DrawGameEnd()
{
	Vector2i LeftCorner;
	int ButtonWidth = 200;
	int ButtonHeight = sGameOver.getLocalBounds().height;
	LeftCorner.x = (Window_width - ButtonWidth) / 2; //指定顶点坐标
	LeftCorner.y = (Window_height - ButtonHeight) / 2;//指定顶点坐标
	
	sGameOver.setPosition(LeftCorner.x, LeftCorner.y); //设置按钮的位置坐标
	
	if (isGameOverState == ncWIN)
		sGameOver.setTextureRect(IntRect(0 * ButtonWidth, 0, ButtonWidth, ButtonHeight)); //读取按钮的纹理区域
	if (isGameOverState == ncLOSE)
		sGameOver.setTextureRect(IntRect(1 * ButtonWidth, 0, ButtonWidth, ButtonHeight)); //读取按钮的纹理区域
	
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