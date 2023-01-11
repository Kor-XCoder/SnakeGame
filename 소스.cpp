#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <thread>
#include <queue>
#include <windows.h>
#include <string.h>
#include <random>
#include <fstream>
#include <deque>
#include <string>
#include <stdlib.h>
/*#include <WinSock2.h>

#pragma comment(lib, "ws2_32");

#define PORT 4578
#define PACKET_SIZE 1024*/

#define stdHandle GetStdHandle(STD_OUTPUT_HANDLE)
using namespace std;
struct location
{
    int x;
    int y;
};
struct player
{
    char* name;
    int score;
    long long elapsed_time;
};
struct compare
{
    bool operator()(player a, player b)
    {
        return a.score < b.score;
    }
};

const int Right = 0;
const int Left = 1;
const int Up = 2;
const int Down = 3;
/*mt19937 engine((unsigned int)time(NULL));
uniform_int_distribution<int> dis(2, 39);
auto generator = bind(dis, engine);*/

random_device rd;
mt19937 mt(rd());
uniform_int_distribution<int> dis(2, 39);
const location dir[4] = { {1, 0}, {-1, 0}, {0, -1}, {0, 1} };
queue<int> rq;

bool isOutOfRange(location here)
{
    return (here.x <= 0 || here.y <= 0 || here.x > 39 || here.y > 39);
}
void write(SHORT x, SHORT y)
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(stdHandle, pos);
}
location DIRtoLOC(location here, int going)
{
    location mve = { here.x + dir[going].x, here.y + dir[going].y };
    return mve;
}
int ReverseDirection(int d)
{
    switch (d)
    {
    case Right:
        return Left;
    case Left:
        return Right;
    case Up:
        return Down;
    case Down:
        return Up;
    }
}
int makeRandom(void)
{
    return dis(mt);
}

class SnakeGame
{
private:
    location here;
    int height = 40, weight = 40;
    deque<location> block;
    int Facing, score, moveDistance;

public:
    bool isGameOvered;
    int speed;
    char map[41][41];
    SnakeGame()
    {
        isGameOvered = false;
        score = 0;
        speed = 300;
        moveDistance = 0;
        // 시작 위치 선정
        here = { 10, 20 };
        block.push_back(here);
        block.push_back({ 9, 20 });
        block.push_back({ 8, 20 });
        block.push_back({ 7, 20 });
        block.push_back({ 6, 20 });
        Facing = Right;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < weight; j++)
            {
                map[i][j] = '.';
            }
        }
        map[20][10] = 'O';
        map[20][9] = 'o';
        map[20][8] = 'o';
        map[20][7] = 'o';
        map[20][6] = 'o';

        map[0][0] = '1';
        map[0][40] = '2';
        map[40][40] = '3';
        map[40][0] = '4';
        for (int i = 1; i < 40; i++)
        {
            map[0][i] = '5';
            map[40][i] = '5';
        }

        for (int i = 1; i < 40; i++)
        {
            map[i][0] = '6';
            map[i][40] = '6';
        }
    }

    void setFacing(int going)
    {
        if (ReverseDirection(Facing) == going) return;
        Facing = going;
    }

    bool move()
    {
        location there = DIRtoLOC(here, Facing);
        if (isOutOfRange(there)) return false;
        if (map[there.y][there.x] != 'A' && map[there.y][there.x] != '.' && map[there.y][there.x] != 'B') return false;
        if (map[there.y][there.x] == 'A')
        {
            score++;
            block.push_front(there);
            map[there.y][there.x] = 'O';
            map[here.y][here.x] = 'o';
            write(here.x * 2 - 1, here.y);
            printf("□");
            write(there.x * 2 - 1, there.y);
            printf("■");

            location r;
            do
            {
                r.x = makeRandom();
                r.y = makeRandom();
            } while (map[r.y][r.x] != '.');
            makeApple(r);
            do
            {
                r.x = makeRandom();
                r.y = makeRandom();
            } while (map[r.y][r.x] != '.');
            write(r.x * 2 - 1, r.y);
            SetConsoleTextAttribute(stdHandle, 12);
            printf("●");
            map[r.y][r.x] = 'B';
            SetConsoleTextAttribute(stdHandle, 7);

            write(120, 3);
            if (speed > 200) speed -= 10;
            printf("현재 속도: %dms/block", speed);
            write(120, 4);
            printf("현재 스코어: %d\n", score);

            here = there;
        }
        else if (map[there.y][there.x] == 'B')
        {
            write(120, 3);
            if (speed > 70) speed -= 50;
            printf("현재 속도: %dms/block\n", speed);
            block.push_front(there);
            location removing = block.back();
            block.pop_back();

            map[there.y][there.x] = 'O';
            map[here.y][here.x] = 'o';
            map[removing.y][removing.x] = '.';

            write(removing.x * 2 - 1, removing.y);
            printf("  ");
            write(here.x * 2 - 1, here.y);
            printf("□");
            write(there.x * 2 - 1, there.y);
            printf("■");

            here = there;
        }
        else
        {
            block.push_front(there);
            location removing = block.back();
            block.pop_back();

            map[there.y][there.x] = 'O';
            map[here.y][here.x] = 'o';
            map[removing.y][removing.x] = '.';

            write(removing.x * 2 - 1, removing.y);
            printf("  ");
            write(here.x * 2 - 1, here.y);
            printf("□");
            write(there.x * 2 - 1, there.y);
            printf("■");

            here = there;
        }
        return true;
    }

    void printMap()
    {
        for (int i = 0; i <= height; i++)
        {
            for (int j = 0; j <= weight; j++)
            {
                if (map[i][j] == '.')
                {
                    printf("  ");
                }
                else if (map[i][j] == 'o') {
                    printf("□");
                }
                else if (map[i][j] == 'O') {
                    printf("■");
                }
                else if (map[i][j] == '1')
                {
                    printf("┌");
                }
                else if (map[i][j] == '2')
                {
                    printf("┐");
                }
                else if (map[i][j] == '3')
                {
                    printf("┘");
                }
                else if (map[i][j] == '4')
                {
                    printf("└");
                }
                else if (map[i][j] == '5')
                {
                    printf("──");
                }
                else if (map[i][j] == '6')
                {
                    printf("│");
                }
            }
            puts("");
        }
    }

    void makeApple(location A)
    {
        map[A.y][A.x] = 'A';
        write(A.x * 2 - 1, A.y);
        SetConsoleTextAttribute(stdHandle, 6);
        printf("★");
        SetConsoleTextAttribute(stdHandle, 7);
    }

    int getScore()
    {
        return score;
    }

    void makeKillTriangle(location A)
    {
        map[A.y][A.x] = 'C';
        write(A.x * 2 - 1, A.y);
        SetConsoleTextAttribute(stdHandle, 4);
        printf("▲");
        SetConsoleTextAttribute(stdHandle, 7);
    }

    void addDistance()
    {
        moveDistance++;
        write(120, 5);
        printf("현재 이동 거리: %d", moveDistance);
        if (moveDistance % 30 == 0)
        {
            location r;
            do
            {
                r.x = makeRandom();
                r.y = makeRandom();
            } while (map[r.y][r.x] != '.');
            makeKillTriangle(r);
        }
    }
};

SnakeGame sk;
void lobby();

void RotateSnake()
{
    system("cls");
    system("color 07");
    sk.printMap();
    location r;
    do
    {
        r.x = makeRandom();
        r.y = makeRandom();
    } while (sk.map[r.y][r.x] != '.');
    sk.makeApple(r);

    while (!sk.isGameOvered)
    {
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        {
            sk.setFacing(Right);
        }
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            sk.setFacing(Left);
        }
        else if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            sk.setFacing(Up);
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            sk.setFacing(Down);
        }
    }
}

void MoveSnake()
{
    write(120, 7);
    while (!sk.isGameOvered)
    {
        if (!sk.move()) sk.isGameOvered = true;
        Sleep(sk.speed);
        sk.addDistance();
    }
}

void GameOver()
{
    system("cls");
    SetConsoleTextAttribute(stdHandle, 4);
    puts(" _______  _______  __   __  _______    _______  __   __  _______  ______   ");
    puts("|       ||   _   ||  |_|  ||       |  |       ||  | |  ||       ||    _ |  ");
    puts("|    ___||  |_|  ||       ||    ___|  |   _   ||  |_|  ||    ___||   | ||  ");
    puts("|   | __ |       ||       ||   |___   |  | |  ||       ||   |___ |   |_||_ ");
    puts("|   ||  ||       ||       ||    ___|  |  |_|  ||       ||    ___||    __  |");
    puts("|   |_| ||   _   || ||_|| ||   |___   |       | |     | |   |___ |   |  | |");
    puts("|_______||__| |__||_|   |_||_______|  |_______|  |___|  |_______||___|  |_|");
    SetConsoleTextAttribute(stdHandle, 7);
    long long finish_t = clock();
    printf("Your Score: %d\n", sk.getScore());
    puts("Continue?");
    system("pause");
    lobby();
}

void lobby()
{
    system("mode con cols=168 lines=42");
    sk = SnakeGame();
    puts("Initalizing...");
    Sleep(1000);

    write(70, 20);
    printf("> SINGLE PLAYER");
    write(70, 21);
    printf("MULTI PLAYER(TCP/IP)");

    bool isSinglePlayer = true;
    while (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
    {
        if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000))
        {
            isSinglePlayer = !isSinglePlayer;
            write(70, 20);
            printf("                         ");
            write(70, 21);
            printf("                         ");
            if (isSinglePlayer)
            {
                write(70, 20);
                printf("> SINGLE PLAYER");
                write(70, 21);
                printf("MULTI PLAYER(TCP/IP)");
            }
            else
            {
                write(70, 20);
                printf("SINGLE PLAYER");
                write(70, 21);
                printf("> MULTI PLAYER(TCP/IP)");
            }
            Sleep(500);
        }
    }
    if (isSinglePlayer)
    {
        thread t1(RotateSnake);
        thread t2(MoveSnake);
        t1.join();
        t2.join();
        GameOver();
    }
    
}



int main(void)
{
    lobby();
}
