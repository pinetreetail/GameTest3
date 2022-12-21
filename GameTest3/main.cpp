#include <DxLib.h>

#include "game.h"

#define ENEMY_MAX 10
#define SHOT_MAX 10

struct Player
{
	float posX;
	float posY;
	float sizeOutside;
	float sizeInside;

	float speedMax;
	float Acc;
	float currSpeedX;
	float currSpeedY;

	bool Dead;

	int space;
	int ShotStop;
};

struct Enemy
{
	float posX;
	float posY;
	float sizeOutside;
	float sizeInside;

	float speed;

	int ColorOutside;
	int ColorInside;

	bool Dead;

	bool resize;
};

struct Shot
{
	float posX;
	float posY;
	float size;

	float speed;

	bool Dead;
};

void PlayerInit(Player& player);
void PlayerUpdate(Player& player);
void PlayerDraw(Player& player);

void EnemyInit(Enemy& enemy);
void EnemyUpdate(Enemy& enemy);
void EnemyDraw(Enemy& enemy);

void ShotInit(Shot& shot);
void ShotUpdate(Shot& shot);
void ShotDraw(Shot& shot);


// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// windowモード設定
	ChangeWindowMode(Game::kWindowMode);
	// ウインドウ名設定
	SetMainWindowText(Game::kTitleText);
	// 画面サイズの設定
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorDepth);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);


	Player player;
	PlayerInit(player);
	Enemy enemy;
	EnemyInit(enemy);
	Shot shot;
	ShotInit(shot);


	while (ProcessMessage() == 0)
	{
		LONGLONG time = GetNowHiPerformanceCount();
		// 画面のクリア
		ClearDrawScreen();
		

		PlayerUpdate(player);
		EnemyUpdate(enemy);
		ShotUpdate(shot);

		PlayerDraw(player);
		EnemyDraw(enemy);
		ShotDraw(shot);


		// 裏画面を表画面に入れ替える
		ScreenFlip();

		// escキーを押したら終了する
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// fpsを６０に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}
	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}


void PlayerInit(Player& player)
{
	player.sizeOutside = 70;
	player.sizeInside = 20;

	player.posX = (Game::kScreenWidth / 2) - (player.sizeOutside / 2);
	player.posY = 700 - (player.sizeOutside / 2);

	player.speedMax = 7;
	player.Acc = 4;
	player.currSpeedX = 0;
	player.currSpeedY = 0;

	player.Dead = false;

	player.space = 0;
	player.ShotStop = 30;
}

void PlayerUpdate(Player& player)
{
	if (player.Dead)	return;

	int padState = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	{
		if (padState & PAD_INPUT_LEFT)
		{
			player.currSpeedX -= player.Acc;
			if (player.currSpeedX < -player.speedMax)	player.currSpeedX = -player.speedMax;
		}
		else if (padState & PAD_INPUT_RIGHT)
		{
			player.currSpeedX += player.Acc;
			if (player.currSpeedX > player.speedMax)	player.currSpeedX = player.speedMax;
		}
		if (padState & PAD_INPUT_UP)
		{
			player.currSpeedY -= player.Acc;
			if (player.currSpeedY < -player.speedMax)	player.currSpeedY = -player.speedMax;
		}
		else if (padState & PAD_INPUT_DOWN)
		{
			player.currSpeedY += player.Acc;
			if (player.currSpeedY > player.speedMax)	player.currSpeedY = player.speedMax;
		}
		else
		{
			player.currSpeedX *= 0.9f;
			player.currSpeedY *= 0.9f;
		}

		player.posX += player.currSpeedX;
		player.posY += player.currSpeedY;

		if (player.posX < 0)	player.posX = 0;
		if (player.posX > Game::kScreenWidth - player.sizeOutside)	player.posX = (Game::kScreenWidth - player.sizeOutside);
		if (player.posY < 0)	player.posY = 0;
		if (player.posY > Game::kScreenHeight - player.sizeOutside)	player.posY = (Game::kScreenHeight - player.sizeOutside);
	}

	player.ShotStop--;

	if (padState & PAD_INPUT_START && player.ShotStop == 0)
	{
		if (player.space > 0)
		{
			player.space = -1;
			for (int i = 0; i < 10; i++)
			{





			}

			player.ShotStop = 30;
		}
		else
		{
			player.space = 0;
		}
	}
}

void PlayerDraw(Player& player)
{
	if (player.Dead)	return;

	DrawCircle(player.posX + (player.sizeOutside / 2), player.posY + (player.sizeOutside / 2),
		player.sizeInside, GetColor(0, 255, 0), true);

	DrawBox(player.posX, player.posY, player.posX + player.sizeOutside, player.posY + player.sizeOutside,
		GetColor(0, 255, 255), false);
}


void EnemyInit(Enemy& enemy)
{
	enemy.sizeOutside = 60;
	enemy.sizeInside = 20;

	enemy.posX = GetRand((Game::kScreenWidth - enemy.sizeOutside));
	enemy.posY = 0;

	enemy.speed = 4;

	enemy.ColorOutside = GetColor(255, 0, 0);
	enemy.ColorInside = GetColor(0, 0, 255);

	enemy.Dead = false;

	enemy.resize = true;
}

void EnemyUpdate(Enemy& enemy)
{
	if (enemy.Dead)	return;

	if (enemy.resize)
	{
		enemy.sizeInside--;
		if (enemy.sizeInside == 10)	enemy.resize = false;
	}
	else if (enemy.resize == false)
	{
		enemy.sizeInside++;
		if (enemy.sizeInside == 20)	enemy.resize = true;
	}

	enemy.posY += enemy.speed;

	if (enemy.posY > Game::kScreenHeight - enemy.sizeOutside)	enemy.Dead = true;
}

void EnemyDraw(Enemy& enemy)
{
	if (enemy.Dead)	return;

	DrawBox(enemy.posX, enemy.posY, enemy.posX + enemy.sizeOutside, enemy.posY + enemy.sizeOutside,
		enemy.ColorOutside, false);

	DrawCircle(enemy.posX + (enemy.sizeOutside / 2), enemy.posY + (enemy.sizeOutside / 2),
		enemy.sizeInside, enemy.ColorInside, true);
}


void ShotInit(Shot& shot)
{
	shot.posX = 0;
	shot.posY = 0;
	shot.size = 10;

	shot.speed = 10;

	shot.Dead = true;
}

void ShotUpdate(Shot& shot)
{
	if (shot.Dead)	return;

	shot.posY -= shot.speed;

	if (shot.posY > 0)	shot.Dead = true;

}

void ShotDraw(Shot& shot)
{
	if (shot.Dead)	return;

	DrawBox(shot.posX, shot.posY, shot.posX + shot.size, shot.posY + shot.size,
		GetColor(255, 255, 0), false);
}