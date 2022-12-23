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
};

struct Enemy
{
	float posX;
	float posY;
	float sizeOutside;
	float sizeInside;

	float speed;

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
	player.sizeOutside = 70;
	player.sizeInside = 20;  

	player.posX = (Game::kScreenWidth / 2) - (player.sizeOutside / 2);
	player.posY = 700 - (player.sizeOutside / 2);

	player.speedMax = 7;
	player.Acc = 4;
	player.currSpeedX = 0;
	player.currSpeedY = 0;

	player.Dead = false;

	int space = 0;
	int ShotStop = 30;

	int EnemyStop = 120;

	//PlayerInit(player);
	Enemy enemy[ENEMY_MAX];
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		enemy[i].sizeOutside = 60;
		enemy[i].sizeInside = 20;

		enemy[i].speed = 6;

		enemy[i].Dead = true;

		enemy[i].resize = true;
	}

//	EnemyInit(enemy);
	Shot shot[SHOT_MAX];
	for (int i = 0; i < SHOT_MAX; i++)
	{
		shot[i].size = 10;

		shot[i].speed = 10;

		shot[i].Dead = true;

//		ShotInit(shot[i]);
	}
	

	while (ProcessMessage() == 0)
	{
		LONGLONG time = GetNowHiPerformanceCount();
		// 画面のクリア
		ClearDrawScreen();


		if (player.Dead == false)
		{
			int padState = GetJoypadInputState(DX_INPUT_KEY_PAD1);

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


			ShotStop--;

			if (padState & PAD_INPUT_A && ShotStop < 0)
			{
				if (space > 0)
				{
					space = -1;
					for (int i = 0; i < SHOT_MAX; i++)
					{
						if (shot[i].Dead == true)
						{
							shot[i].posX = ((player.posX + player.sizeOutside) / 2)
											- (shot[i].size / 2);
							shot[i].posY = player.posY + shot[i].size;
						
							shot[i].Dead = false;

							break;
						}
					}

					ShotStop = 30;
				}
				else
				{
					space = 0;
				}
			}

			DrawCircle(player.posX + (player.sizeOutside / 2), player.posY + (player.sizeOutside / 2),
				player.sizeInside, GetColor(0, 255, 0), true);

			DrawBox(player.posX, player.posY, player.posX + player.sizeOutside, player.posY + player.sizeOutside,
				GetColor(0, 255, 255), false);
		}
		
		EnemyStop--;

		if (EnemyStop < 0)
		{
			for (int i = 0; i < ENEMY_MAX; i++)
			{
				if (enemy[i].Dead == true)
				{
					enemy[i].posX = GetRand((Game::kScreenWidth - enemy[i].sizeOutside));
					enemy[i].posY = 0;

					enemy[i].Dead == false;

					EnemyStop = 120;

					break;
				}
			}
		}

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].Dead == false)
			{
				if (enemy[i].resize)
				{
					enemy[i].sizeInside--;
					if (enemy[i].sizeInside == 10)	enemy[i].resize = false;
				}
				else if (enemy[i].resize == false)
				{
					enemy[i].sizeInside++;
					if (enemy[i].sizeInside == 20)	enemy[i].resize = true;
				}

				enemy[i].posY += enemy[i].speed;

				if (enemy[i].posY > Game::kScreenHeight - enemy[i].sizeOutside)	enemy[i].Dead = true;

				DrawBox(enemy[i].posX, enemy[i].posY, enemy[i].posX + enemy[i].sizeOutside, enemy[i].posY + enemy[i].sizeOutside,
					GetColor(255, 0, 0), false);

				DrawCircle(enemy[i].posX + (enemy[i].sizeOutside / 2), enemy[i].posY + (enemy[i].sizeOutside / 2),
					enemy[i].sizeInside, GetColor(0, 0, 255), true);
			}
		}

		for (int i = 0; i < SHOT_MAX; i++)
		{
			if (shot[i].Dead == false)
			{
				shot[i].posY -= shot[i].speed;

				if (shot[i].posY > 0)	shot[i].Dead = true;

				DrawBox(shot[i].posX, shot[i].posY, shot[i].posX + shot[i].size, shot[i].posY + shot[i].size,
					GetColor(255, 255, 0), false);
			}
		}


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