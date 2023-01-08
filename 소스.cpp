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


#define stdHandle GetStdHandle(STD_OUTPUT_HANDLE)
using namespace std;
struct location
{
    int x;
    int y;
};
struct player
{
    string name;
    int score;
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
const location dir[4] = { {1, 0}, {-1, 0}, {0, -1}, {0, 1} };

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
int makeRandom(int a, int b)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(a, b);
    return dis(gen);
}

class SnakeGame
{
private:
    location here;
    int height = 40, weight = 40;
    deque<location> block;
    int Facing, score;

public:
    bool isGameOvered;
    char map[41][41];
    SnakeGame()
    {
        isGameOvered = false;
        score = 0;
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
        if (map[there.y][there.x] != 'A' && map[there.y][there.x] != '.') return false;
        if (map[there.y][there.x] == 'A')
        {
            block.push_front(there);
            map[there.y][there.x] = 'O';
            map[here.y][here.x] = 'o';
            write(here.x * 2 - 1, here.y + 1);
            printf("□");
            write(there.x * 2 - 1, there.y + 1);
            printf("■");

            location r;
            do
            {
                r.x = makeRandom(2, 39);
                r.y = makeRandom(2, 39);
            } while (map[r.y][r.x] != '.');
            makeApple(r);

            write(7, 0);
            printf("%d", ++score);

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

            write(removing.x * 2 - 1, removing.y + 1);
            printf("  ");
            write(here.x * 2 - 1, here.y + 1);
            printf("□");
            write(there.x * 2 - 1, there.y + 1);
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
                } else if (map[i][j] == 'o'){
                    printf("□");
                } else if (map[i][j] == 'O'){
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
        write(A.x * 2 - 1, A.y + 1);
        SetConsoleTextAttribute(stdHandle, 6);
        printf("★");
        SetConsoleTextAttribute(stdHandle, 7);
    }

    int getScore()
    {
        return score;
    }
};

SnakeGame sk;

void RotateSnake()
{
    system("cls");
    system("color 07");
    puts("SCORE: 0");
    sk.printMap();
    location r;
    do
    {
        r.x = makeRandom(2, 39);
        r.y = makeRandom(2, 39);
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
    while (!sk.isGameOvered)
    {
        if (!sk.move()) sk.isGameOvered = true;
        Sleep(200);
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
    printf("%llds(%lldms) Elapsed!\n", finish_t / 1000, finish_t);
    puts("================================= RANKING =================================");
    puts("      RANK                NAME                SCORE");
    ifstream rf;
    //priority_queue<player, vector<player>, 
    int rank = 1;
    rf.open("ranking.txt");
    if (rf.is_open())
    {
        while (!rf.eof())
        {
            char tmp[10];
            rf.getline(tmp, 10);
            char* name = strtok(tmp, ",");
            char* score = strtok(NULL, ",");
            char* end;
            printf("      %d                   %s                   %d\n", rank++, name, strtol(score, &end, 10));
        }
    }
    rf.close();
}

void lobby()
{
    system("mode con cols=84 lines=50");
    sk = SnakeGame();
    for (int i = 1; i <= 9; i++)
    {
        system("cls");
        char temp[10] = "color 0";
        temp[7] = i + '0';
        system(temp);

        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n                                   Snake Game!\n\n\n");
        printf("\n\n                                   Game Start!\n");
        Sleep(100);
    }
    system("pause");


    thread t1(RotateSnake);
    thread t2(MoveSnake);
    t1.join();
    t2.join();
    GameOver();
}



int main(void)
{
    lobby();
}