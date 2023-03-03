#include"Game.h"

int main()
{
	Game Mine;
	while (Mine.window.isOpen())
	{
		Mine.Run();
	}
}