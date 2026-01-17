#include "snake.h"

void GameStart(Snake* ps)
{
    Prepare();
    WelcomeToGame(ps);
    CreateMap();
    SnakeInit(ps);
}

void GameRun(Snake* ps)
{
    PrintInfo(ps);
    Pause();
    while(ps->_gameState == OK)
    {
        SnakeMove(ps);
        PrintInfo(ps);
        KillByWall(ps);
        KillBySelf(ps);
        Sleep(ps->_sleepTime);
    }
}

void GameEnd(Snake* ps)
{
    PrintErrorInfo(ps);
    DestroySnake(ps);
    Pause();
}

int main()
{
    Snake snake;
    GameStart(&snake);
    GameRun(&snake);
    GameEnd(&snake);
    return 0;
}