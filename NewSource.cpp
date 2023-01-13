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

random_device rd;
mt19937 mt(rd());
uniform_int_distribution<int> dis(1, 39);

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
    return dis(mt);
}
void RenderBorder()
{
	gotoxy(0, 0); printf("¶£");
	gotoxy(40*2-1, 0); printf("¶§");
	gotoxy(40*2-1, 40); printf("¶•");
	gotoxy(0, 40); printf("¶¶");
	
	for (int i = 1; i < 40; i++)
	{
		gotoxy(i*2-1, 0); printf("¶°¶°");
		gotoxy(i*2-1, 40); printf("¶°¶°");
		gotoxy(0, i); printf("¶¢");
		gotoxy(40*2-1, i); printf("¶¢");
	}
	puts("");
}
void RenderFirstSnake()
{
    gotoxy(10*2-1, 20); printf("°·");
    for (int i = 6; i <= 9; i++)
    {
    	gotoxy(i*2-1, 20);
    	printf("°‡");
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
		int Facing, score, speed, moveDistance;
		char map[40][40];
		deque<location> block;
		
	public:
		bool isGameOvered;
		Snake()
		{
			isGameOvered = false;
	        score = 0;
	        speed = 300;
	        moveDistance = 0;
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
			printf("°⁄");
			SetColor(White);
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
			
			if (map[there.y][there.x] == 'A')
			{
				block.push_front(there);
				
				map[here.y][here.x] = 'o';
				map[there.y][there.x] = 'O';
				
				gotoxy(here.x*2-1, here.y);
				printf("°‡");
				gotoxy(there.x*2-1, there.y);
				printf("°·");
				here = there;
				
				GenerateApple();
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
				printf("°‡");
				gotoxy(there.x*2-1, there.y);
				printf("°·");
				here = there;
			}
			
			return true;
		}
		int getSpeed()
		{
			return speed;
		}
};
Snake player;

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

void lobby()
{
	FillConsoleOutputCharacter(stdHandle, ' ', 300 * 300, { 0, 0 }, &dw);
    puts("Initalizing...");
    Sleep(1000);
    gotoxy(70, 20);
    printf("> SINGLE PLAYER");
    gotoxy(70, 21);
    printf("MULTI PLAYER(TCP/IP)");

    bool isSinglePlayer = true;
    while (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
    {
        if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000))
        {
            isSinglePlayer = !isSinglePlayer;
            gotoxy(70, 20);
            printf("                         ");
            gotoxy(70, 21);
            printf("                         ");
            if (isSinglePlayer)
            {
                gotoxy(70, 20);
                printf("> SINGLE PLAYER");
                gotoxy(70, 21);
                printf("MULTI PLAYER(TCP/IP)");
            }
            else
            {
                gotoxy(70, 20);
                printf("SINGLE PLAYER");
                gotoxy(70, 21);
                printf("> MULTI PLAYER(TCP/IP)");
            }
            Sleep(300);
        }
    }
    FillConsoleOutputCharacter(stdHandle, ' ', 300 * 300, { 0, 0 }, &dw);
    if (isSinglePlayer)
    {
    	player = Snake();
    	RenderBorder();
    	RenderFirstSnake();
        thread t1(RotateSnake);
        thread t2(MoveSnake);
        
        t1.join();
        t2.join();
        //GameOver();
    }
    
}

int main(void)
{
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
    cursorInfo.dwSize = 1; //ƒøº≠ ±Ω±‚ (1 ~ 100)
    cursorInfo.bVisible = FALSE; //ƒøº≠ Visible TRUE(∫∏¿”) FALSE(º˚±Ë)
    SetConsoleCursorInfo(stdHandle, &cursorInfo);
	system("mode con cols=168 lines=43");
	lobby();
}
