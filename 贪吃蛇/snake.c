#include "snake.h"

//设置光标位置
void SetPos(short x, short y)
{
    HANDLE houtput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(houtput, pos);
}

void ChangeCursorVisible(bool x)
{
    //获取标准输出设备的句柄
    HANDLE houtput = GetStdHandle(STD_OUTPUT_HANDLE);

    //定义存储光标信息的结构体
    CONSOLE_CURSOR_INFO cursorInfo;

    //获取和houtput相关的控制台上的光标信息，将其存放在cursorInfo结构体中
    GetConsoleCursorInfo(houtput, &cursorInfo);

    //修改光标信息
    cursorInfo.bVisible = x;

    //设置和houtput相关的控制台上的光标信息
    SetConsoleCursorInfo(houtput, &cursorInfo);
}

//设置页面内容及其他必要准备
void Prepare()
{
    srand((unsigned int)time(NULL));
    setlocale(LC_ALL, "");
    system("title 贪吃蛇");
    system("mode con cols=120 lines=35");
    ChangeCursorVisible(false);
}

//欢迎界面
void WelcomeToGame(Snake* ps)
{
    SetPos(50, 15);
    wprintf(L"欢迎来到贪吃蛇游戏！");
    SetPos(54, 19);
    wprintf(L"请选择难度：");
    SetPos(50, 21);
    wprintf(L"1.简单 2.中等 3.高难");
    while(1)
    {
        if(KEY_PRESS(0x31) || KEY_PRESS(0x61))
        {
            ps->_sleepTime = 150;
            break;
        }
        else if(KEY_PRESS(0x32) || KEY_PRESS(0x62))
        {
            ps->_sleepTime = 100;
            break;
        }
        else if(KEY_PRESS(0X33) || KEY_PRESS(0x63))
        {
            ps->_sleepTime = 50;
            break;
        }
    }
    system("cls");
}

void CreateMap()
{
    //上
    SetPos(0, 0);
    for(int i = 0; i < 30; i++)
    {
        wprintf(L"%lc", WALL_SIGN);
    }
    //下
    SetPos(0, 29);
    for(int i = 0; i < 30; i++)
    {
        wprintf(L"%lc", WALL_SIGN);
    }
    //左
    for(int i = 1; i <= 28; i++)
    {
        SetPos(0, i);
        wprintf(L"%lc", WALL_SIGN);
    }
    //右
    for(int i = 1; i <= 28; i++)
    {
        SetPos(58, i);
        wprintf(L"%lc", WALL_SIGN);
    }
}

SnakeNode* CreateNewNode(COORD pos)
{
    SnakeNode* newnode = (SnakeNode*)malloc(sizeof(SnakeNode));
    if(newnode == NULL)
    {
        perror("malloc");
        exit(1);
    }
    newnode->pos = pos;
    newnode->next = NULL;
    return newnode;
}

void FrontPush(SnakeNode** pps, COORD pos)
{
    assert(pps);
    SnakeNode* newnode = CreateNewNode(pos);
    if(*pps == NULL)//空链表
    {
        *pps = newnode;
    }
    else
    {
        newnode->next = *pps;
        *pps = newnode;
    }
}

void PopBack(SnakeNode** pps)
{
    assert(pps && *pps);
    if((*pps)->next == NULL)//只有一个元素
    {
        free(*pps);
        *pps = NULL;
    }
    else
    {
        SnakeNode* ptail, *prev;
        ptail = prev = *pps;
        while(ptail->next)
        {
            prev = ptail;
            ptail = ptail->next;
        }
        free(ptail);
        ptail = NULL;
        prev->next = NULL;
    }
}

bool FoodPosIsValid(COORD pos, Snake* ps)
{
    SnakeNode* phead = ps->_phead;
    while(phead)
    {
        if(pos.X == phead->pos.X && pos.Y == phead->pos.Y)
        {
            return false;
        }
        phead = phead->next;
    }
    return true;
}

void CreateFood(Snake* ps)
{
    while(1)
    {
        COORD pos;
        pos.X = (rand() % 28 + 1) * 2;
        pos.Y = rand() % 28 + 1;
        if(FoodPosIsValid(pos, ps))
        {
            ps->_foodPos = pos;
            break;
        }
    }
}

void CreateSpecialFood(Snake* ps)
{
    //先消除上一个特殊食物
    if(ps->_haveSpecialFood)
    {
        SetPos(ps->_specialFoodPos.X, ps->_specialFoodPos.Y);
        wprintf(L"  ");
    }
    int x = rand() % 5 + 1;
    if(x == 1)
    {
        ps->_haveSpecialFood = true;
    }
    else
    {
        ps->_haveSpecialFood = false;
    }

    if(ps->_haveSpecialFood)
    {
        while(1)
        {
            COORD pos;
            pos.X = (rand() % 28 + 1) * 2;
            pos.Y = rand() % 28 + 1;
            if(FoodPosIsValid(pos, ps) && (pos.X != ps->_foodPos.X || pos.Y != ps->_foodPos.Y))
            {
                ps->_specialFoodPos = pos;
                break;
            }
        }
    }
}

void SnakeInit(Snake* ps)
{
    ps->_phead = NULL;
    for(int i = 0; i < 5; i++)
    {
        COORD pos = {24 + 2 * i, 10};
        FrontPush(&ps->_phead, pos);
    }

    ps->_gameState = OK;
    ps->_dir = RIGHT;
    CreateFood(ps);
    ps->_haveSpecialFood = false;
    ps->_score = 0;
}

void PrintInfo(Snake* ps)
{
    SetPos(65, 10);
    wprintf(L"当前得分：%d", ps->_score);
    SetPos(65, 15);
    wprintf(L"按 ↑ ↓ ← → 控制蛇的移动");
    SetPos(65, 16);
    wprintf(L"按空格暂停，按ESC退出游戏");

    //打印蛇和食物信息
    SnakeNode* phead = ps->_phead;
    while(phead)
    {
        SetPos(phead->pos.X, phead->pos.Y);
        wprintf(L"%lc", BODY_SIGN);
        phead = phead->next;
    }

    SetPos(ps->_foodPos.X, ps->_foodPos.Y);
    wprintf(L"%lc", FOOD_SIGN);

    if(ps->_haveSpecialFood)
    {
        SetPos(ps->_specialFoodPos.X, ps->_specialFoodPos.Y);
        wprintf(L"%lc", SPECIAL_FOOD_SIGN);
    }
}

void Pause()
{
    while(1)
    {
        if(KEY_PRESS(VK_SPACE))
        {
            break;
        }
        Sleep(100);
    }
}

void KillByWall(Snake* ps)
{
    if(ps->_phead->pos.X < 2 || ps->_phead->pos.X > 56 || ps->_phead->pos.Y < 1 || ps->_phead->pos.Y > 28)
    {
        ps->_gameState = KILL_BY_WALL;
    }
}

void KillBySelf(Snake* ps)
{
    SnakeNode* cur = ps->_phead;
    while(cur)
    {
        if(cur != ps->_phead && cur->pos.X == ps->_phead->pos.X && cur->pos.Y == ps->_phead->pos.Y)
        {
            ps->_gameState = KILL_BY_SELF;
        }
        cur = cur->next;
    }
}

COORD NextPos(Snake* ps)
{
    COORD nextPos = ps->_phead->pos;
    if(ps->_dir == UP)
    {
        (nextPos.Y)--;
    }
    else if(ps->_dir == DOWN)
    {
        (nextPos.Y)++;
    }
    else if(ps->_dir == LEFT)
    {
        nextPos.X -= 2;
    }
    else
    {
        nextPos.X += 2;
    }
    return nextPos;
}

bool NextIsFood(Snake* ps)
{
    COORD nextPos = NextPos(ps);
    if(nextPos.X == ps->_foodPos.X && nextPos.Y == ps->_foodPos.Y)
    {
        return true;
    }
    return false;
}

bool NextIsSpecialFood(Snake* ps)
{
    if(ps->_haveSpecialFood == false)
    {
        return false;
    }
    COORD nextPos = NextPos(ps);
    if(nextPos.X == ps->_specialFoodPos.X && nextPos.Y == ps->_specialFoodPos.Y)
    {
        return true;
    }
    return false;
}

void EatFood(Snake* ps)
{
    //把食物的位置头插进去
    SetPos(ps->_foodPos.X, ps->_foodPos.Y);
    wprintf(L"  ");
    FrontPush(&ps->_phead, ps->_foodPos);
    (ps->_score)++;
    CreateFood(ps);
    CreateSpecialFood(ps);
}

void NoFood(Snake* ps)
{
    //头插
    COORD nextPos = NextPos(ps);
    FrontPush(&ps->_phead, nextPos);

    //最后一个位置打印空格
    SnakeNode* ptail = ps->_phead;
    while(ptail->next)
    {
        ptail = ptail->next;
    }
    SetPos(ptail->pos.X, ptail->pos.Y);
    wprintf(L"  ");
    //尾删
    PopBack(&ps->_phead);
}

void EatSpecialFood(Snake* ps)
{
    SetPos(ps->_specialFoodPos.X, ps->_specialFoodPos.Y);
    wprintf(L"  ");
    ps->_haveSpecialFood = false;
    NoFood(ps);
    ps->_score += 15;
}

void SnakeMove(Snake* ps)
{
    //检测按键
    if(KEY_PRESS(VK_UP) && ps->_dir != DOWN)
    {
        ps->_dir = UP;
    }
    else if(KEY_PRESS(VK_DOWN) && ps->_dir != UP)
    {
        ps->_dir = DOWN;
    }
    else if(KEY_PRESS(VK_LEFT) && ps->_dir != RIGHT)
    {
        ps->_dir = LEFT;
    }
    else if(KEY_PRESS(VK_RIGHT) && ps->_dir != LEFT)
    {
        ps->_dir = RIGHT;
    }
    else if(KEY_PRESS(VK_SPACE))
    {
        Pause();
    }
    else if(KEY_PRESS(VK_ESCAPE))
    {
        ps->_gameState = END_NORMAL;
    }
    else if(KEY_PRESS(VK_F3))
    {
        ps->_sleepTime = 50;
    }
    else if(KEY_PRESS(VK_F4))
    {
        ps->_sleepTime = 100;
    }

    if(NextIsFood(ps))
    {
        EatFood(ps);
    }
    else if(NextIsSpecialFood(ps))
    {
        EatSpecialFood(ps);
    }
    else
    {
        NoFood(ps);
    }
}

void PrintErrorInfo(Snake* ps)
{
    SetPos(65, 12);
    switch(ps->_gameState)
    {
        case OK:
            break;
        case END_NORMAL:
            wprintf(L"退出游戏！");
            break;
        case KILL_BY_SELF:
            wprintf(L"撞到自己了！游戏结束！");
            break;
        case KILL_BY_WALL:
            wprintf(L"撞墙了！游戏结束！");
            break;
    }
}

void DestroySnake(Snake* ps)
{
    while(ps->_phead)
    {
        PopBack(&ps->_phead);
    }
}