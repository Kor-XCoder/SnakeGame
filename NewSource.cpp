#include <cstdio>
#include <windows.h>
#include <random>
#include <deque>
#include <thread>

#define stdHandle GetStdHandle(STD_OUTPUT_HANDLE)
using namespace std;

struct location
{
    int x;
    int y;
};
const int Right = 0;
const int Left = 1;
const int Up = 2;
const int Down = 3;

const int Black = 0;
const int Blue = 1;
const int Green = 2;
const int BlueGreen = 3;
const int Red = 4;
const int Purple = 5;
const int Yellow = 6;
const int White = 7;
const int Gray = 8;
const int LightBlue = 9;
const int LightGreen = 10;
const int LightBlueGreen = 11;
const int LightRed = 12;
const int LightPurple = 13;
const int LightYellow = 14;
const int LightWhite = 15;

const location dir[4] = { {1, 0}, {-1, 0}, {0, -1}, {0, 1} };
unsigned long dw;

//random_device rd;
//mt19937 mt(rd());
//uniform_int_distribution<int> dis(1, 39);
mt19937 engine((unsigned int)time(NULL));
uniform_int_distribution<int> distribution(1, 39);
auto generator = bind(distribution, engine);

bool isOutOfRange(location here)
{
    return (here.x <= 0 || here.y <= 0 || here.x > 39 || here.y > 39);
}
void gotoxy(SHORT x, SHORT y)
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
int makeRandom()
{
    return generator();
}
void RenderBorder()
{
    gotoxy(0, 0); printf("┌");
    gotoxy(40*2-1, 0); printf("┐");
    gotoxy(40*2-1, 40); printf("┘");
    gotoxy(0, 40); printf("└");
    
    for (int i = 1; i < 40; i++)
    {
        gotoxy(i*2-1, 0); printf("──");
        gotoxy(i*2-1, 40); printf("──");
        gotoxy(0, i); printf("│");
        gotoxy(40*2-1, i); printf("│");
    }
    puts("");
}
void RenderFirstSnake()
{
    gotoxy(10*2-1, 20); printf("■");
    for (int i = 6; i <= 9; i++)
    {
        gotoxy(i*2-1, 20);
        printf("□");
    }
}
void SetColor(int col)
{
    SetConsoleTextAttribute(stdHandle, col);
}

class Snake
{
    private:
        location here;
        int Facing, score, speed;
        char map[40][40];
        deque<location> block;
        
        void addScore(int sc)
        {
            score += sc;
            gotoxy(120, 4);
            printf("현재 스코어: %d점", score);
        }
        void addSpeed(int sp)
        {
            if (speed - sp < 30) return;
            speed -= sp;
            gotoxy(120, 5);
            printf("현재 속도: %dms/block", speed);
        }
        void GenerateKillTriangle()
        {
            location r;
            do
            {
                r.x = makeRandom();
                r.y = makeRandom();
            }
            while (map[r.y][r.x] != '.');
            map[r.y][r.x] = 'C';
            gotoxy(r.x * 2 - 1, r.y);
            SetColor(LightRed);
            printf("▲");
            SetColor(White);
        }
        
    public:
        bool isGameOvered;
        Snake()
        {
            isGameOvered = false;
            score = 0;
            speed = 250;
            while (!block.empty()) // deque init
            {
                block.pop_back();
            }
            for (int i = 1; i < 40; i++) // map init
            {
                for (int j = 1; j < 40; j++)
                {
                    map[i][j] = '.';
                }
            }
            
            here = { 10, 20 };
            block.push_back(here);
            block.push_back({ 9, 20 });
            block.push_back({ 8, 20 });
            block.push_back({ 7, 20 });
            block.push_back({ 6, 20 });
            Facing = Right;
            
            map[20][10] = 'O';
            map[20][9] = 'o';
            map[20][8] = 'o';
            map[20][7] = 'o';
            map[20][6] = 'o';
        }
        
        void setFacing(int going)
        {
            if (ReverseDirection(Facing) == going) return;
            Facing = going;
        }
        bool Move()
        {
            location there = DIRtoLOC(here, Facing);
            if (isOutOfRange(there)) return false;
            if (map[there.y][there.x] != '.' && map[there.y][there.x] != 'A') return false;
            addScore(1);
            if (map[there.y][there.x] == 'A')
            {
                block.push_front(there);
                
                map[here.y][here.x] = 'o';
                map[there.y][there.x] = 'O';
                
                gotoxy(here.x*2-1, here.y);
                printf("□");
                gotoxy(there.x*2-1, there.y);
                printf("■");
                here = there;
                
                GenerateApple();
                GenerateKillTriangle();
                addScore(100);
                addSpeed(10);
            }
            else
            {
                location removing = block.back();
                block.pop_back();
                block.push_front(there);
                
                map[removing.y][removing.x] = '.';
                map[here.y][here.x] = 'o';
                map[there.y][there.x] = 'O';
                
                gotoxy(removing.x*2-1, removing.y);
                printf("  ");
                gotoxy(here.x*2-1, here.y);
                printf("□");
                gotoxy(there.x*2-1, there.y);
                printf("■");
                here = there;
            }
            
            return true;
        }
        int getSpeed()
        {
            return speed;
        }
        int getScore()
        {
            return score;
        }
        void GenerateApple(void)
        {
            location r;
            do
            {
                r.x = makeRandom();
                r.y = makeRandom();
            }
            while (map[r.y][r.x] != '.');
            map[r.y][r.x] = 'A';
            gotoxy(r.x*2-1, r.y);
            SetColor(Yellow);
            printf("★");
            SetColor(White);
        }
};
Snake player;
void lobby();

void MoveSnake()
{
    player.GenerateApple();
    while (!player.isGameOvered)
    {
        player.isGameOvered = !player.Move();
        Sleep(player.getSpeed());
    }
}

void RotateSnake()
{
    while (!player.isGameOvered)
    {
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        {
            player.setFacing(Right);
        }
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            player.setFacing(Left);
        }
        else if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            player.setFacing(Up);
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            player.setFacing(Down);
        }
    }
}

void GameOver()
{
    FillConsoleOutputCharacter(stdHandle, ' ', 300 * 300, { 0, 0 }, &dw);
    gotoxy(0, 0);
    SetColor(Red);
    puts(" _______  _______  __   __  _______    _______  __   __  _______  ______   ");
    puts("|       ||   _   ||  |_|  ||       |  |       ||  | |  ||       ||    _ |  ");
    puts("|    ___||  |_|  ||       ||    ___|  |   _   ||  |_|  ||    ___||   | ||  ");
    puts("|   | __ |       ||       ||   |___   |  | |  ||       ||   |___ |   |_||_ ");
    puts("|   ||  ||       ||       ||    ___|  |  |_|  ||       ||    ___||    __  |");
    puts("|   |_| ||   _   || ||_|| ||   |___   |       | |     | |   |___ |   |  | |");
    puts("|_______||__| |__||_|   |_||_______|  |_______|  |___|  |_______||___|  |_|");
    
    SetColor(White);
    gotoxy(60, 15);
    printf("최종 스코어: ");
    int sc = player.getScore();
    int WaitingTime = 500;
    for (int i = 0; i <= sc; i++)
    {
        gotoxy(73, 15);
        printf("        ");
        gotoxy(73, 15);
        printf("%d", i);
        WaitingTime -= WaitingTime * 0.1;
        Sleep(WaitingTime);
    }
    
    gotoxy(0, 9);
    if (sc < 300)
    {
        SetColor(Red);
        puts("         _______ ");
        puts("        |       |");
        puts("        |    ___|");
        puts("        |   |___ ");
        puts("        |    ___|");
        puts("        |   |    ");
        puts("        |___|    ");
    }
    else if (sc < 700)
    {
        SetColor(Blue);
        puts("         _______ ");
        puts("        |       |");
        puts("        |    ___|");
        puts("        |   |    ");
        puts("        |   |___ ");
        puts("        |       |");
        puts("        |_______|");
    }
    else if (sc < 2000)
    {
        SetColor(BlueGreen);
        puts("         _______ ");
        puts("        |  _    |");
        puts("        | |_|   |");
        puts("        |      _|");
        puts("        |  _  |_ ");
        puts("        | |_|   |");
        puts("        |_______|");
    }
    else if (sc < 3000)
    {
        SetColor(Green);
        puts("         _______ ");
        puts("        |   _   |");
        puts("        |  | |  |");
        puts("        |  |_|  |");
        puts("        |       |");
        puts("        |   _   |");
        puts("        |__| |__|");
    }
    else if (sc < 5000)
    {
        SetColor(LightGreen);
        puts("         _______    _    ");
        puts("        |   _   | _| |_  ");
        puts("        |  | |  ||_   _| ");
        puts("        |  |_|  |  |_|   ");
        puts("        |       |");
        puts("        |   _   |");
        puts("        |__| |__|");
    }
    else if (sc < 10000)
    {
        SetColor(Yellow);
        puts("         _______ ");
        puts("        |       |");
        puts("        |  _____|");
        puts("        | |_____ ");
        puts("        |_____  |");
        puts("         _____| |");
        puts("        |_______|");
    }
    else
    {
        SetColor(LightYellow);
        puts("         _______    _    ");
        puts("        |       | _| |_  ");
        puts("        |  _____||_   _| ");
        puts("        | |_____   |_|    ");
        puts("        |_____  |");
        puts("         _____| |");
        puts("        |_______|");
    }
    SetColor(White);
    
    
    gotoxy(0, 17);
    system("pause");
    lobby();
}

void lobby()
{
    FillConsoleOutputCharacter(stdHandle, ' ', 300 * 300, { 0, 0 }, &dw);
    gotoxy(50, 3);
    SetColor(Green);
    puts("   _____             __           ______                   ");
    gotoxy(50, 4);puts("  / ___/____  ____ _/ /_____     / ____/___ _____ ___  ___ ");
    gotoxy(50, 5);puts("  \\__ \\/ __ \\/ __ `/ //_/ _ \\   / / __/ __ `/ __ `__ \\/ _ \\");
    gotoxy(50, 6);puts(" ___/ / / / / /_/ / ,< /  __/  / /_/ / /_/ / / / / / /  __/");
    gotoxy(50, 7);puts("/____/_/ /_/\\__,_/_/|_|\\___/   \\____/\\__,_/_/ /_/ /_/\\___/ ");
    
	SetColor(White);
    gotoxy(70, 20);
    printf("> SINGLE PLAYER");

    while (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
    {
        
    }
    FillConsoleOutputCharacter(stdHandle, ' ', 300 * 300, { 0, 0 }, &dw);
    player = Snake();
    RenderBorder();
    RenderFirstSnake();
    gotoxy(120, 5);
    printf("현재 속도: 250ms/block");
    thread t1(RotateSnake);
    thread t2(MoveSnake);

    t1.join();
    t2.join();
    GameOver();
    
}

int main(void)
{
    CONSOLE_CURSOR_INFO cursorInfo = { 0, };
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = 0;
    SetConsoleCursorInfo(stdHandle, &cursorInfo);
    system("mode con cols=168 lines=43");
    lobby();
}
