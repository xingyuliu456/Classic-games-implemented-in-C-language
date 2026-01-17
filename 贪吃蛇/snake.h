#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>

#define KEY_PRESS(vk) ((GetAsyncKeyState(vk) & 1) ? 1 : 0)

#define FOOD_SIGN L'☆'
#define SPECIAL_FOOD_SIGN L'★'
#define WALL_SIGN L'□'
#define BODY_SIGN L'●'

typedef struct SnakeNode
{
    COORD pos;
    struct SnakeNode* next;
}SnakeNode;

typedef enum GameState
{
    OK,
    KILL_BY_WALL,
    KILL_BY_SELF,
    END_NORMAL
}GameState;

typedef enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
}Direction;

typedef struct Snake
{
    SnakeNode* _phead;//指向蛇头的指针
    GameState _gameState;//游戏状态
    Direction _dir;//方向
    int _sleepTime;//休息时间，代表蛇的速度
    COORD _foodPos;//食物位置
    COORD _specialFoodPos;
    bool _haveSpecialFood;
    int _score;//总分
}Snake;

void Prepare();

void WelcomeToGame();

void CreateMap();

void SnakeInit(Snake* ps);

//头插法
void FrontPush(SnakeNode** pps, COORD pos);

void PopBack(SnakeNode** pps);

void CreateFood(Snake* ps);

void CreateSpecialFood(Snake* ps);

void PrintInfo(Snake* ps);

void Pause();

void KillByWall(Snake* ps);

void KillBySelf(Snake* ps);

void SnakeMove(Snake* ps);

void PrintErrorInfo(Snake* ps);

void DestroySnake(Snake* ps);