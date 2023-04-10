#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <rpcdce.h>

#define col GetStdHandle(STD_OUTPUT_HANDLE)

#define RED SetConsoleTextAttribute(col, 0x000c);

#define BLUE SetConsoleTextAttribute(col, 0x0001 | 0x0008);

#define HIGH SetConsoleTextAttribute(col, 0x00a);

#define WHITE SetConsoleTextAttribute(col, 0x000f);

#define SKY SetConsoleTextAttribute(col, 0x000b);

#define YEL SetConsoleTextAttribute(col, 0x000e);

#define FLUOR SetConsoleTextAttribute(col, 0x000d);

#define VIO SetConsoleTextAttribute(col, 0x0001 | 0x0008 |0x000c); 

#define BCK SetConsoleTextAttribute(col, 0x0000);

#define GRAY SetConsoleTextAttribute(col, 0x0008);

#define WHT SetConsoleTextAttribute(col, 0x000F);

#define HIGHGRAY SetConsoleTextAttribute(col, 0x00a | 0x0000);

#define ScreenX 70 

#define ScreenY 34

#define LEFT 75

#define RIGHT 77

#define UP 72

#define DOWN 80

#define ENTER 13

#define ESC 27

#define SPACE 32 

#define SHIFT 15

#define GUID_SIZE 36

#define LICENSE_SIZE 28

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;

int Field[40][40];

int ShownField[40][40];

int Flag[40][40];

int Search[40][40];

int Wrong[40][40];

int FieldX;

int FieldY;

int Difficulty = 1;

int FlagCount;

int MineCount;

int Space_Left;

int RealP = 1;

int GameOver;

int WrongGuess;

void SetCursorType(CURSOR_TYPE c);

void GoToXY(int x, int y);

void PrintArray(const char a[], int x, int y, int delay);

void EmptyArray();

void SetMines();

void DrawScreenEdge();

void ClearScreen();

void SelectDifficulty();

void NumberField();

void DrawMineEdge();

void PrintField();

void Move();

void RerollBombs(int y, int x);

void RevealValue(int y, int x);

void Expand0(int y, int x);

void PlayAgain();

_declspec(noinline) int LicenseCheck();

_declspec(noinline) unsigned CollapseGuidToWord(char* guid);

int _declspec(noinline) unsigned CountSetBits(int n);

_declspec(noinline) void DebuggerCheck();

_declspec(noinline) int CheckLicenseWord(unsigned machineGuidWord, unsigned licenseWord);

_declspec(noinline) char* GetMachineGUID(char* guid, unsigned size);

int main()
{
	DebuggerCheck();

	LicenseCheck();

	SetCursorType(NOCURSOR);

	DrawScreenEdge();

	while (RealP)
	{
		DebuggerCheck();

		ClearScreen();

		SelectDifficulty();

		ClearScreen();

		EmptyArray();

		SetMines();

		NumberField();

		DrawMineEdge();

		Move();

		PlayAgain();

	}
	GoToXY(0, ScreenY);
}

void DebuggerCheck()
{
	if (IsDebuggerPresent())
	{
		printf("Do not debug me!");
		exit(-1);
	}
}

int CheckLicenseWord(unsigned machineGuidWord, unsigned licenseWord)
{
	unsigned setBits = CountSetBits(licenseWord);
	if (setBits < 16)
		return 1;

	int isEvenNumber = licenseWord & 1;
	if (machineGuidWord % 37 == 0)
	{
		return !isEvenNumber;
	}

	return isEvenNumber;
}

int LicenseCheck()
{
	printf("License check (xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx): ");

	char license[GUID_SIZE + 1] = { 0 };
	fgets(license, sizeof(license), stdin);

	size_t inputLength = strlen(license);
	license[inputLength] = 0;

	if (inputLength != GUID_SIZE)
	{
		printf("Failed license check, will exit...");
		exit(-1);
	}

	for (int i = 0; license[i]; i++)
	{
		license[i] = tolower(license[i]);
	}
	
	char* guid = (char*)malloc(GUID_SIZE + 1);
	int size = GUID_SIZE + 1;
	unsigned machineGuidDword = CollapseGuidToWord(GetMachineGUID(guid, size));
	free(guid);

	unsigned licenseWord = CollapseGuidToWord(license);
	if (CheckLicenseWord(machineGuidDword, licenseWord))
	{
		printf("Failed license check, will exit...");
		exit(-1);
	}

	return 0;
}

unsigned CountSetBits(int n)
{
	unsigned int count = 0;
	while (n) 
	{
		n &= (n - 1);
		count++;
	}
	return count;
}

char* GetMachineGUID(char* guid, unsigned size)
{
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return NULL;
	}

	if (RegQueryValueExA(hKey, "MachineGuid", NULL, NULL, (LPBYTE)guid, (LPDWORD)&size) != ERROR_SUCCESS)
	{
		DWORD dw = GetLastError();
		RegCloseKey(hKey);
		return NULL;
	}
	RegCloseKey(hKey);

	return guid;
}

unsigned CollapseGuidToWord(char* shortGuid)
{
	unsigned xorWord = 0;
	for (int i = 0; i < strlen(shortGuid); i += sizeof(unsigned))
	{
		xorWord ^= *((unsigned*)(&shortGuid[i]));
	}

	return xorWord;
}

void SetCursorType(CURSOR_TYPE c)
{
	CONSOLE_CURSOR_INFO CurInfo;
	switch (c)
	{
	case NOCURSOR:

		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;

	case SOLIDCURSOR:

		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;

	case NORMALCURSOR:

		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void GoToXY(int x, int y)
{
	COORD Cur;
	Cur.Y = y;
	Cur.X = x;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

void PrintArray(const char a[], int x, int y, int delay)
{
	size_t length = strlen(a);
	for (size_t i = x; i < x + length; i++)
	{
		GoToXY((int)i, y);
		printf("%c", a[i - x]);
		Sleep(delay);
	}
}

void EmptyArray()
{
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			Flag[i][j] = 0;
			ShownField[i][j] = 0;
			Field[i][j] = 0;
			Search[i][j] = 0;
			Wrong[i][j] = 0;
		}
	}
}

void SetMines()
{
	int i = 0;
	srand((unsigned)time(NULL));

	while (MineCount != i)
	{
	reshuffle:

		int randX = rand() % FieldX + 1;
		int randY = rand() % FieldY + 1;

		if (Field[randY][randX])
		{
			goto reshuffle;
		}
		else
		{
			Field[randY][randX] = '*';
			i++;
		}
	}
}

void ColorNumber(int x)
{
	SetConsoleTextAttribute(col, x - 48);
	printf("%d ", x - 48);
}

void DrawScreenEdge()
{
	int i;

	for (i = 0; i < ScreenX; i++)
	{
		GoToXY(i, 0);
		_putch('+');
	}

	for (i = 1; i < ScreenX; i++)
	{
		GoToXY(i, 5);
		_putch('-');
	}

	for (i = 0; i < ScreenY; i++)
	{
		GoToXY(ScreenX, i);
		_putch('+');
	}

	for (i = ScreenX; i > 0; i--)
	{
		GoToXY(i, ScreenY);
		_putch('+');
	}

	for (i = ScreenY; i > 0; i--)
	{
		GoToXY(0, i);
		_putch('+');
	}
	WHT;

	GoToXY(8, ScreenY - 3);
	printf("Move :");

	SKY;

	printf(" Arrow Keys ");
	WHT;
	printf(" Reveal :");

	YEL;

	printf(" Enter Key ");
	WHT;
	printf(" Flag :");

	VIO;
	printf(" Space Key");
	WHT;
	GoToXY(0, ScreenY);
}

void SelectDifficulty()
{
	int inputChar = 0;
	PrintArray("Select Difficulty", 23, 8, 60);

	YEL;

	GoToXY(30, 16);
	printf("Tiny");

	GoToXY(28, 16);
	printf("-");

	WHT;

	GoToXY(30, 18);
	printf("Small");

	GoToXY(30, 20);
	printf("Large");

	Difficulty = 1;

	while (inputChar != ENTER)
	{
		GoToXY(30, 25);
		inputChar = _getch();

		if (inputChar == 0xE0 || inputChar == 0)
		{
			inputChar = _getch();

			if (Difficulty == 1 && inputChar == DOWN)
			{
				Difficulty++;

				YEL;
				GoToXY(28, 16);
				_putch(' ');
				GoToXY(30, 18);
				printf("Small");
				GoToXY(28, 18);
				_putch('-');

				WHT;
				GoToXY(30, 16);
				printf("Tiny");
			}
			else if (Difficulty == 2 && inputChar == UP)
			{
				Difficulty--;

				YEL;
				GoToXY(28, 18);
				_putch(' ');
				GoToXY(30, 16);
				printf("Tiny");
				GoToXY(28, 16);
				_putch('-');

				WHT;
				GoToXY(30, 18);
				printf("Small");
			}
			else if (Difficulty == 2 && inputChar == DOWN)
			{
				Difficulty++;

				YEL;
				GoToXY(28, 18);
				_putch(' ');
				GoToXY(30, 20);
				printf("Large");
				GoToXY(28, 20);
				_putch('-');

				WHT;
				GoToXY(30, 18);
				printf("Small");
			}
			else if (Difficulty == 3 && inputChar == UP)
			{
				Difficulty--;

				YEL;
				GoToXY(28, 20);
				_putch(' ');
				GoToXY(30, 18);
				printf("Small");
				GoToXY(28, 18);
				_putch('-');

				WHT;
				GoToXY(30, 20);
				printf("Large");
			}
		}
		else if (inputChar == ESC) { GoToXY(0, ScreenY); exit(1); }
	}

	VIO;

	if (Difficulty == 1)
	{
		FieldX = 9;
		FieldY = 9;
		MineCount = 10;
		PrintArray("Tiny", 30, 25, 40);
	}
	else if (Difficulty == 2)
	{
		FieldX = 16;
		FieldY = 16;
		MineCount = 40;
		PrintArray("Small", 30, 25, 40);
	}
	else 
	{
		FieldX = 30;
		FieldY = 16;
		MineCount = 99;
		PrintArray("Large", 30, 25, 40);
	}

	Space_Left = FieldX * FieldY;
	WHT;

	Sleep(350);

	GoToXY(0, ScreenY);
}

void ClearScreen()
{
	int i, j;

	for (i = 1; i < ScreenX; i++)
	{
		GoToXY(i, 4);
		_putch(' ');
		for (j = 6; j < ScreenY - 3; j++)
		{
			GoToXY(i, j);
			_putch(' ');
		}
	}
	GoToXY(0, ScreenY);
}

void DrawMineEdge()
{
	int i;
	GRAY;
	for (i = 35 - FieldX - 2; i < 35 + FieldX + 1; i++)
	{
		GoToXY(i, 8);
		_putch('+');
	}

	for (i = 8; i < 12 + FieldY; i++)
	{
		GoToXY(35 + FieldX + 1, i);
		_putch('+');
	}
	for (i = 35 + FieldX + 1; i > 35 - FieldX - 2; i--)
	{
		GoToXY(i, 12 + FieldY);
		_putch('+');
	}

	for (i = 12 + FieldY; i > 8; i--)
	{
		GoToXY(35 - FieldX - 2, i);
		_putch('+');
	}
	WHT;
	GoToXY(0, ScreenY);
}

void PrintField() //For Dubugging
{
	for (int i = 1; i <= FieldY; i++)
	{
		GoToXY(35 - FieldX, i + 10);
		for (int j = 1; j <= FieldX; j++)
		{
			if (Field[i][j] == '*')
			{
				if (Flag[i][j])
				{
					VIO;
					printf("> ");
				}
				else
				{
					GRAY; printf("%c ", Field[i][j]); WHT;
				}
			}
			else if (Flag[i][j]) { RED; printf("><"); WHT; }
			else if ((Field[i][j] != 48) && Field[i][j])ColorNumber(Field[i][j]);
			else printf("  ");
		}
	}
}

void NumberField()
{
	for (int i = 1; i <= FieldY; i++)
	{
		for (int j = 1; j <= FieldX; j++)
		{
			int Near_Mine = 0;
			if (Field[i][j] != '*')
			{
				if (Field[i - 1][j - 1] == '*') Near_Mine++;
				if (Field[i - 1][j] == '*') Near_Mine++;
				if (Field[i - 1][j + 1] == '*') Near_Mine++;
				if (Field[i][j + 1] == '*') Near_Mine++;
				if (Field[i + 1][j + 1] == '*') Near_Mine++;
				if (Field[i + 1][j] == '*') Near_Mine++;
				if (Field[i + 1][j - 1] == '*') Near_Mine++;
				if (Field[i][j - 1] == '*') Near_Mine++;
				Field[i][j] = Near_Mine + 48;
			}
		}
	}
}

void Expand0(int y, int x)
{
	if ((x > FieldX) || (x < 1) || (y > FieldY) || (y < 1)) { BCK; return; }
	if (ShownField[y][x] == 1) { BCK; return; }
	else
	{
		if (Flag[y][x])
		{
			Wrong[y][x] = 1;
			WrongGuess++;
			BCK;
			return;
		}
		if (Field[y][x] != '0')
		{
			GoToXY(33 - FieldX + 2 * x, y + 10);
			ColorNumber(Field[y][x]);
			Space_Left--;
			ShownField[y][x] = 1;
			WHT;
		}
		else
		{
			BCK;
			GoToXY(33 - FieldX + 2 * x, y + 10);
			printf("[]");
			Space_Left--;
			ShownField[y][x] = 1;
			Expand0(y - 1, x - 1);
			Expand0(y - 1, x);
			Expand0(y - 1, x + 1);
			Expand0(y, x + 1);
			Expand0(y + 1, x + 1);
			Expand0(y + 1, x);
			Expand0(y + 1, x - 1);
			Expand0(y, x - 1);
		}
	}
}

void PlayAgain()
{
	int tempPlay = 1;
	char applyLimit[50] = { 'P','l','a','y',' ','a','g','a','i','n','?' };
	FLUOR;
	PrintArray(applyLimit, 28, 30, 50);

	FLUOR;
	GoToXY(33, 32);
	printf("YES");

	GoToXY(31, 32);
	printf("\\-");

	WHT;

	GoToXY(33, 34);
	printf("NO");

	int ch = 0;
	while (ch != ENTER)
	{
		ch = _getch();

		if (ch == 0xE0 || ch == 0)

		{
			ch = _getch();

			if (tempPlay == 1 && ch == DOWN)
			{
				tempPlay = 0;

				FLUOR;
				GoToXY(31, 34);
				printf("\\-");
				GoToXY(33, 34);
				printf("NO");
				WHT;
				GoToXY(33, 32);
				printf("YES");
				GoToXY(31, 32);
				_putch(' ');

			}
			else if (tempPlay == 0 && ch == UP)
			{
				tempPlay = 1;

				FLUOR;
				GoToXY(31, 32);
				printf("\\-");
				GoToXY(33, 32);
				printf("YES");
				WHT;
				GoToXY(33, 34);
				printf("NO");
				GoToXY(31, 34);
				_putch(' ');

			}
		}
		else if (ch == ESC) { GoToXY(0, ScreenY); exit(1); }
	}
	RealP = tempPlay;

	char game_end[25] = { 'C','l','o','s','i','n','g',' ','P','r','o','g','r','a','m','.','.','.','.','.','.' };

	FLUOR;

	if (RealP == 0)

		PrintArray(game_end, 25, 36, 50);

	WHT;

	Sleep(350);

}

void RevealValue(int y, int x)
{
	if ((x > FieldX) || (x < 1) || (y > FieldY) || (y < 1) || (Flag[y][x])) return;

	if (ShownField[y][x] == 0)
	{
		if (Field[y][x] == '*')
		{
			WHT;
			if (GameOver == 0) PrintField();
			HIGH;
			if (Flag[y][x] == 1)SKY;
			GoToXY(33 - FieldX + 2 * x, y + 10);
			printf("\\* ");
			GoToXY(30, 4);
			RED;
			printf("GAME OVER");
			WHT;
			GameOver++;
		}
		else if (Field[y][x] != '0')
		{
			HIGH;
			GoToXY(33 - FieldX + 2 * x, y + 10);
			ColorNumber(Field[y][x]);
			ShownField[y][x] = 1;
			Space_Left--;
			WHT;
			if (Flag[y][x])
			{
				Flag[y][x] = 0;
				FlagCount--;
			}
		}
		else
		{
			Expand0(y, x);
			BCK;
			GoToXY(33 - FieldX + 2 * x, y + 10);
			printf("[]");
			WHT;

		}
	}
	else return;
}

void RerollBombs(int y, int x)
{
	if (Field[y][x] == '*')
	{
		Field[y][x] = 48;

		if (Field[y - 1][x - 1] != '*') Field[y - 1][x - 1]--; else Field[y][x]++;
		if (Field[y - 1][x] != '*') Field[y - 1][x]--;	else Field[y][x]++;
		if (Field[y - 1][x + 1] != '*') Field[y - 1][x + 1]--;	else Field[y][x]++;
		if (Field[y][x + 1] != '*') Field[y][x + 1]--;	else Field[y][x]++;
		if (Field[y + 1][x + 1] != '*') Field[y + 1][x + 1]--;	else Field[y][x]++;
		if (Field[y + 1][x] != '*') Field[y + 1][x]--;	else Field[y][x]++;
		if (Field[y + 1][x - 1] != '*') Field[y + 1][x - 1]--;	else Field[y][x]++;
		if (Field[y][x - 1] != '*') Field[y][x - 1]--;	else Field[y][x]++;

	again:

		int randy = rand() % FieldY + 1;
		int randx = rand() % FieldX + 1;
		if (x == randx && y == randy)
		{
			Field[randy][randx] = 48;
			goto again;
		}

		if (Field[randy][randx] == '*')
		{
			Field[randy][randx] = 48;
			goto again;
		}
		else
		{
			Field[randy][randx] = '*';
			randy--; randx--;
			if (Field[randy][randx] != '*') Field[randy][randx]++;
			randx++;
			if (Field[randy][randx] != '*') Field[randy][randx]++;
			randx++;
			if (Field[randy][randx] != '*') Field[randy][randx]++;
			randy++;
			if (Field[randy][randx] != '*') Field[randy][randx]++;
			randy++;
			if (Field[randy][randx] != '*') Field[randy][randx]++;
			randx--;
			if (Field[randy][randx] != '*') Field[randy][randx]++;
			randx--;
			if (Field[randy][randx] != '*') Field[randy][randx]++;
			randy--;
			if (Field[randy][randx] != '*') Field[randy][randx]++;
			randx++;
		}
	}
}

void Move()
{
	int x = 1;
	int y = 1;
	int pre_x = 0;
	int pre_y = 0;
	int pre_pre_x = -1;
	int pre_pre_y = -1;
	int try_ = 0;
	FlagCount = 0;
	GameOver = 0;
	WrongGuess = 0;
	{
		for (int i = 1; i <= FieldY; i++)
		{
			GoToXY(35 - FieldX, i + 10);
			for (int j = 1; j <= FieldX; j++)
			{
				printf("[]");
			}
		}
	}
	GoToXY(33 - FieldX + 2 * x, y + 10);
	HIGH;
	printf("[]");
	WHT;
	while (1)
	{
		fflush(stdin);
		WHT;
		GoToXY(2, 4);
		printf("Flags Left: %3d", MineCount - FlagCount);
		GoToXY(53, 4);
		printf("Spaces Left: ");
		GoToXY(66, 4);
		printf("%3d", Space_Left - MineCount);
		GoToXY(33, 9);
		YEL;
		WHT;
		
		int inputChar = 0;
		while (inputChar != ENTER)
		{
			inputChar = _getch();

			if (inputChar == 0xE0 || inputChar == 0)
			{
				inputChar = _getch();

				if (inputChar == LEFT)
				{
					if (x > 1)
					{
						WHT;
						GoToXY(33 - FieldX + 2 * x, y + 10);
						if (ShownField[y][x])
						{
							if (Field[y][x] != '0')
							{
								ColorNumber(Field[y][x]);
							}
							else
							{
								BCK;
								printf("[]");
								WHT;
							}

						}
						else if (Flag[y][x])
						{
							FLUOR;
							printf("> ");
							WHT;
						}
						else
						{
							printf("[]");
						}

						x--;
						HIGH;

						GoToXY(33 - FieldX + 2 * x, y + 10);
						if (ShownField[y][x])
						{
							if (Field[y][x] != '0')
							{
								printf("%c ", Field[y][x]);
							}
							else
							{
								HIGHGRAY;
								printf("[]");
								WHT;
							}

						}
						else if (Flag[y][x])
						{
							printf("> ");
							WHT;
						}
						else
						{
							printf("[]");
						}

					}
				}
				else if (inputChar == RIGHT)
				{
					if (x < FieldX)
					{
						WHT;
						GoToXY(33 - FieldX + 2 * x, y + 10);
						if (ShownField[y][x])
						{
							if (Field[y][x] != '0')
							{
								ColorNumber(Field[y][x]);
							}
							else
							{
								BCK;
								printf("[]");
								WHT;
							}

						}
						else if (Flag[y][x])
						{
							FLUOR;
							printf("> ");
							WHT;
						}
						else
						{
							printf("[]");
						}

						x++;
						HIGH;

						GoToXY(33 - FieldX + 2 * x, y + 10);
						if (ShownField[y][x])
						{
							if (Field[y][x] != '0')
							{
								printf("%c ", Field[y][x]);
							}
							else
							{
								HIGHGRAY;
								printf("[]");
								WHT;
							}

						}
						else if (Flag[y][x])
						{
							printf("> ");
							WHT;
						}
						else
						{
							printf("[]");
						}
					}
				}
				else if (inputChar == UP)
				{
					if (y > 1)
					{
						WHT;
						GoToXY(33 - FieldX + 2 * x, y + 10);
						if (ShownField[y][x])
						{
							if (Field[y][x] != '0')
							{
								ColorNumber(Field[y][x]);
							}
							else
							{
								BCK;
								printf("[]");
								WHT;
							}

						}
						else if (Flag[y][x])
						{
							FLUOR;
							printf("> ");
							WHT;
						}
						else
						{
							printf("[]");
						}

						y--;
						HIGH;

						GoToXY(33 - FieldX + 2 * x, y + 10);
						if (ShownField[y][x])
						{
							if (Field[y][x] != '0')
							{
								printf("%c ", Field[y][x]);
							}
							else
							{
								HIGHGRAY;
								printf("[]");
								WHT;
							}

						}
						else if (Flag[y][x])
						{
							printf("> ");
							WHT;
						}
						else
						{
							printf("[]");
						}
					}
				}
				else if (inputChar == DOWN)
				{
					if (y < FieldY)
					{
						WHT;
						GoToXY(33 - FieldX + 2 * x, y + 10);
						if (ShownField[y][x])
						{
							if (Field[y][x] != '0')
							{
								ColorNumber(Field[y][x]);
							}
							else
							{
								BCK;
								printf("[]");
								WHT;
							}

						}
						else if (Flag[y][x])
						{
							FLUOR;
							printf("> ");
							WHT;
						}
						else
						{
							printf("[]");
						}

						y++;
						HIGH;

						GoToXY(33 - FieldX + 2 * x, y + 10);
						if (ShownField[y][x])
						{
							if (Field[y][x] != '0')
							{
								printf("%c ", Field[y][x]);
							}
							else
							{
								HIGHGRAY;
								printf("[]");
								WHT;
							}

						}
						else if (Flag[y][x])
						{
							printf("> ");
							WHT;
						}
						else
						{
							printf("[]");
						}
					}
				}
			}
			else if ((inputChar == SPACE) && ShownField[y][x] == 0)
			{
				GoToXY(33 - FieldX + 2 * x, y + 10);
				if (Flag[y][x] == 0)
				{
					HIGH;
					printf("> ");
					WHT;
					Flag[y][x] = 1;
					FlagCount++;
					if (Field[y][x] == '*') Wrong[y][x] = 1;
				}
				else
				{
					HIGH;
					printf("[]");
					Flag[y][x] = 0;
					WHT;
					FlagCount--;
					if (Field[y][x] == '*') Wrong[y][x] = 0;
				}
				WHT;
				GoToXY(2, 4);
				printf("Flag Left : %3d", MineCount - FlagCount);
			}
		}

		try_++;

		if (try_ == 1)
		{
			RerollBombs(y, x);
		}

		if (Flag[y][x] == 0)RevealValue(y, x);

		if (ShownField[y][x] && Field[y][x] != '0') // 
		{
			int whether_open;
			whether_open = Flag[y - 1][x - 1] + Flag[y - 1][x] + Flag[y - 1][x + 1] + Flag[y][x + 1] + Flag[y + 1][x + 1] + Flag[y + 1][x] + Flag[y + 1][x - 1] + Flag[y][x - 1];
			if (whether_open == Field[y][x] - 48)
			{
				if (!Flag[y - 1][x - 1])RevealValue(y - 1, x - 1);
				if (!Flag[y - 1][x])RevealValue(y - 1, x);
				if (!Flag[y - 1][x + 1])RevealValue(y - 1, x + 1);
				if (!Flag[y][x + 1])RevealValue(y, x + 1);
				if (!Flag[y + 1][x + 1])RevealValue(y + 1, x + 1);
				if (!Flag[y + 1][x])RevealValue(y + 1, x);
				if (!Flag[y + 1][x - 1])RevealValue(y + 1, x - 1);
				if (!Flag[y][x - 1])RevealValue(y, x - 1);
			}
		}

		if (WrongGuess >= 1)
		{
			PrintField();
			GoToXY(30, 4);
			RED;
			printf("GAME OVER");
			GameOver = 1;
		}
		if (GameOver >= 1)
		{
			GoToXY(33, 9);
			YEL;
			printf("x\\_x");
			goto end;
		}
		else if (Flag[y][x])
		{
			;
		}
		else if (Field[y][x] == '*')
		{
			;
		}
		else if (Field[y][x] != 48)
		{
			GoToXY(33 - FieldX + 2 * x, y + 10);
			HIGH;
			printf("%c ", Field[y][x]);
		}
		else
		{
			GoToXY(33 - FieldX + 2 * x, y + 10);
			HIGH;
			printf("[]");
		}

		if (Space_Left == MineCount)
		{
			WHT;
			GoToXY(2, 4);
			printf("Flag Left :   0");
			GoToXY(53, 4);
			printf("Space Left :   0");
			GoToXY(33, 9);
			YEL;
			HIGH;
			GoToXY(25, 4);
			printf("Congratulations!");
			break;
		}
		inputChar = 0;

	}

end:
	;
}