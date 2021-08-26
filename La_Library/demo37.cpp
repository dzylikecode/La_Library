
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include "La_Windows.h"
#include "La_WinGDI.h"
#include "La_GameBox.h"
#include "La_Graphic.h"
#include "La_GraphicEx.h"
#include "La_Input.h"
#include "La_Audio.h"
#include "La_Master.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace AUDIO;
using namespace GDI;
using namespace GRAPHIC;
using namespace INPUT_;

KEYBOARD keyboard;

#define NUM_STARS    512  // number of stars in sim
//飞船数目
#define NUM_TIES     32   // number of tie fighters in sim

// 3D engine constants
//近裁剪面
#define NEAR_Z          10   // the near clipping plane
//远裁剪面
#define FAR_Z           2000 // the far clipping plane    
//视距，投影到640窗口上，视野为90度
// 相当于等腰直角三角形，高为320，斜边640，视野90
#define VIEW_DISTANCE   320  // viewing distance from viewpoint 
							 // this gives a field of view of 90 degrees
							 // when projected on a window of 640 wide
// player constants
#define CROSS_VEL       8  // speed that the cross hair moves
#define PLAYER_Z_VEL    8  // virtual z velocity that player is moving
						   // to simulate motion without moving

// tie fighter model constants
#define NUM_TIE_VERTS     10
#define NUM_TIE_EDGES     8

// explosion
#define NUM_EXPLOSIONS    (NUM_TIES) // total number of explosions

// game states
#define GAME_RUNNING      1
#define GAME_OVER         0

// TYPES ///////////////////////////////////////////////////

// this a 3D point
typedef struct POINT3D_TYP
{
	COLOR color;     // color of point 16-bit
	float x, y, z;      // coordinates of point in 3D
} POINT3D, * POINT3D_PTR;

// this is a 3D line, nothing more than two indices into a vertex list
//包含的是索引，所以有一个储存点的内存池
typedef struct LINE3D_TYP
{
	COLOR color;  // color of line 16-bit
	int v1, v2;     // indices to endpoints of line in vertex list

} LINE3D, * LINE3D_PTR;

// a tie fighter
typedef struct TIE_TYP
{
	int state;      // state of the tie, 0=dead, 1=alive
	float x, y, z;  // position of the tie
	float xv, yv, zv; // velocity of the tie
} TIE, * TIE_PTR;

// a basic 3D vector used for velocity
typedef struct VEC3D_TYP
{
	float x, y, z; // coords of vector
} VEC3D, * VEC3D_PTR;

// a wire frame explosion
typedef struct EXPL_TYP
{
	int state;       // state of explosion
	int counter;     // counter for explosion 
	COLOR color;    // color of explosion

	// an explosion is a collection of edges/lines
	// based on the 3D model of the tie that is exploding
	//爆炸是通过随机移动构成飞船的3d模型线段来实现的
	POINT3D p1[NUM_TIE_EDGES];  // start point of edge n
	POINT3D p2[NUM_TIE_EDGES];  // end point of edge n

	//碎片的速度
	VEC3D   vel[NUM_TIE_EDGES]; // velocity of shrapnel

} EXPL, * EXPL_PTR;


// the tie fighter is a collection of vertices connected by
// lines that make up the shape. only one tie fighter see if 
// you can replicate it?
//飞船都是通过一个飞船模型变换得来的

//飞船的顶点
POINT3D tie_vlist[NUM_TIE_VERTS]; // vertex list for the tie fighter model
//飞船的边--采用对顶点的引用
LINE3D  tie_shape[NUM_TIE_EDGES]; // edge list for the tie fighter model
//飞船数目
TIE     ties[NUM_TIES];           // tie fighters

POINT3D stars[NUM_STARS]; // the starfield


COLOR rgb_green,
rgb_white,
rgb_red,
rgb_blue;

#define WINDOW_WIDTH SCREEN_WIDTH
#define WINDOW_HEIGHT SCREEN_HEIGHT
// player vars
float cross_x = 0, // cross hairs
cross_y = 0;

int cross_x_screen = WINDOW_WIDTH / 2,   // used for cross hair
cross_y_screen = WINDOW_HEIGHT / 2,
target_x_screen = WINDOW_WIDTH / 2,   // used for target
target_y_screen = WINDOW_HEIGHT / 2;

//视点的虚拟速度
int player_z_vel = 4; // virtual speed of viewpoint/ship
//
int cannon_state = 0; // state of laser cannon
//激光炮计数器
int cannon_count = 0; // laster cannon counter

EXPL explosions[NUM_EXPLOSIONS]; // the explosions

int misses = 0; // tracks number of missed ships
int hits = 0; // tracks number of hits
int score = 0; // take a guess :)


int game_state = GAME_RUNNING; // state of game

SOUND laserSound, explosionSound;// sound of laser pulse// sound of explosion
MUSIC mainTrackMusic; // main music track

//会将飞船设置在视点的最远处
void Init_Tie(int index)
{
	// this function starts a tie fighter up at the far end
	// of the universe and sends it our way!

	// position each tie in the viewing volume
	ties[index].x = -SCREEN_WIDTH + rand() % (2 * SCREEN_WIDTH);
	ties[index].y = -SCREEN_HEIGHT + rand() % (2 * SCREEN_HEIGHT);
	ties[index].z = 4 * FAR_Z;

	// initialize velocity of tie fighter
	ties[index].xv = -4 + rand() % 8;
	ties[index].yv = -4 + rand() % 8;
	ties[index].zv = -4 - rand() % 64;

	// turn the tie fighter on
	ties[index].state = 1;
}

void StartUp(void)
{
	//TSTRING path = TEXT(".\\Resource\\demo37\\");
	TSTRING path = TEXT("F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo37\\");


	explosionSound.create(path + TEXT("exp1.wav"));

	laserSound.create(path + TEXT("shocker.wav"));

	mainTrackMusic.create(TEXT("midifile2.mid"), path);

	// create system colors
	rgb_green = RGB_DX(0, 255, 0);
	rgb_white = RGB_DX(255, 255, 255);
	rgb_blue = RGB_DX(0, 0, 255);
	rgb_red = RGB_DX(255, 0, 0);

	// create the star field
	for (int index = 0; index < NUM_STARS; index++)
	{
		// randomly position stars in an elongated cylinder stretching from
		// the viewpoint 0,0,-d to the yon clipping plane 0,0,far_z
		stars[index].x = -SCREEN_WIDTH / 2 + rand() % SCREEN_WIDTH;
		stars[index].y = -SCREEN_HEIGHT / 2 + rand() % SCREEN_HEIGHT;
		stars[index].z = NEAR_Z + rand() % (FAR_Z - NEAR_Z);

		// set color of stars
		stars[index].color = rgb_white;
	}

	// create the tie fighter model 飞船模型

	// the vertex list for the tie fighter
	POINT3D temp_tie_vlist[NUM_TIE_VERTS] =
		// color, x,y,z
	{ {rgb_white,-40,40,0},    // p0
	  {rgb_white,-40,0,0},    // p1
	  {rgb_white,-40,-40,0},   // p2
	  {rgb_white,-10,0,0},    // p3
	  {rgb_white,0,20,0},     // p4
	  {rgb_white,10,0,0},     // p5
	  {rgb_white,0,-20,0},    // p6
	  {rgb_white,40,40,0},     // p7
	  {rgb_white,40,0,0},     // p8
	  {rgb_white,40,-40,0} };   // p9

	// copy the model into the real global arrays
	for (int index = 0; index < NUM_TIE_VERTS; index++)
		tie_vlist[index] = temp_tie_vlist[index];

	// the edge list for the tie fighter
	LINE3D temp_tie_shape[NUM_TIE_EDGES] =
		// color, vertex 1, vertex 2
	{ {rgb_green,0,2      },    // l0
	  {rgb_green,1,3      },    // l1
	  {rgb_green,3,4      },    // l2
	  {rgb_green,4,5      },    // l3
	  {rgb_green,5,6      },    // l4
	  {rgb_green,6,3      },    // l5
	  {rgb_green,5,8      },    // l6
	  {rgb_green,7,9      } };  // l7

	// copy the model into the real global arrays
	for (int index = 0; index < NUM_TIE_EDGES; index++)
		tie_shape[index] = temp_tie_shape[index];

	//初始化飞船
	// initialize the position of each tie fighter and it's velocity
	for (int index = 0; index < NUM_TIES; index++)
	{
		// initialize this tie fighter
		Init_Tie(index);

	}

	keyboard.create();

	fpsSet.set(60);
}

void Start_Explosion(int tie)
{
	// this starts an explosion based on the sent tie fighter

	// first hunt and see if an explosion is free
	//找到内存池里面的可用的
	for (int index = 0; index < NUM_EXPLOSIONS; index++)
	{
		if (explosions[index].state == 0)
		{
			// start this explosion up using the properties
			// if the tie fighter index sent
			//表示为在用
			explosions[index].state = 1; // enable state of explosion
			explosions[index].counter = 0; // reset counter for explosion 

			// set color of explosion
			explosions[index].color = rgb_green;

			// make copy of of edge list, so we can blow it up
			for (int edge = 0; edge < NUM_TIE_EDGES; edge++)
			{
				// start point of edge         形心           飞船模型  点（边起点索引） = 边的起点
				explosions[index].p1[edge].x = ties[tie].x + tie_vlist[tie_shape[edge].v1].x;
				explosions[index].p1[edge].y = ties[tie].y + tie_vlist[tie_shape[edge].v1].y;
				explosions[index].p1[edge].z = ties[tie].z + tie_vlist[tie_shape[edge].v1].z;

				// end point of edge
				explosions[index].p2[edge].x = ties[tie].x + tie_vlist[tie_shape[edge].v2].x;
				explosions[index].p2[edge].y = ties[tie].y + tie_vlist[tie_shape[edge].v2].y;
				explosions[index].p2[edge].z = ties[tie].z + tie_vlist[tie_shape[edge].v2].z;

				// compute trajectory vector for edges
				//为边设置爆炸的速度
				explosions[index].vel[edge].x = ties[tie].xv - 8 + rand() % 16;  //原有速度上面叠加
				explosions[index].vel[edge].y = ties[tie].yv - 8 + rand() % 16;
				explosions[index].vel[edge].z = -3 + rand() % 4;

			}

			// done, so return
			return;
		}

	}

}

void Process_Explosions(void)
{
	// this processes all the explosions

	// loop thro all the explosions and render them
	for (int index = 0; index < NUM_EXPLOSIONS; index++)
	{
		// test if this explosion is active?
		if (explosions[index].state == 0)
			continue;
		//对内存池里（对象）统一处理
		// 更新位置
		for (int edge = 0; edge < NUM_TIE_EDGES; edge++)
		{
			// must be exploding, update edges (shrapel)

			explosions[index].p1[edge].x += explosions[index].vel[edge].x;
			explosions[index].p1[edge].y += explosions[index].vel[edge].y;
			explosions[index].p1[edge].z += explosions[index].vel[edge].z;

			explosions[index].p2[edge].x += explosions[index].vel[edge].x;
			explosions[index].p2[edge].y += explosions[index].vel[edge].y;
			explosions[index].p2[edge].z += explosions[index].vel[edge].z;
		}

		// test for terminatation of explosion?
			//结束爆炸
		if (++explosions[index].counter > 100)
			explosions[index].state = explosions[index].counter = 0;
	}
}

void Draw_Explosions(void)
{
	// this draws all the explosions

	// loop thro all the explosions and render them
	for (int index = 0; index < NUM_EXPLOSIONS; index++)
	{
		// test if this explosion is active?
		if (explosions[index].state == 0)
			continue;

		// render this explosion
		// each explosion is made of a number of edges
		for (int edge = 0; edge < NUM_TIE_EDGES; edge++)
		{
			//用来透视
			POINT3D p1_per, p2_per; // used to hold perspective endpoints

			// test if edge if beyond near clipping plane
			//是否超出视野深度
			if (explosions[index].p1[edge].z < NEAR_Z &&
				explosions[index].p2[edge].z < NEAR_Z)
				continue;

			// step 1: perspective transform each end point
			//首先透视变换 即为 k = 视距 / z ； 投影后的x = k * x
			p1_per.x = VIEW_DISTANCE * explosions[index].p1[edge].x / explosions[index].p1[edge].z;
			p1_per.y = VIEW_DISTANCE * explosions[index].p1[edge].y / explosions[index].p1[edge].z;
			p2_per.x = VIEW_DISTANCE * explosions[index].p2[edge].x / explosions[index].p2[edge].z;
			p2_per.y = VIEW_DISTANCE * explosions[index].p2[edge].y / explosions[index].p2[edge].z;

			// step 2: compute screen coords
			//设置为屏幕中心为视距的原点
			int p1_screen_x = WINDOW_WIDTH / 2 + p1_per.x;
			int p1_screen_y = WINDOW_HEIGHT / 2 - p1_per.y;
			int p2_screen_x = WINDOW_WIDTH / 2 + p2_per.x;
			int p2_screen_y = WINDOW_HEIGHT / 2 - p2_per.y;

			// step 3: draw the edge
			GRAPHIC::DrawLine(p1_screen_x, p1_screen_y, p2_screen_x, p2_screen_y,
				explosions[index].color);

		}
	}
}

void Move_Starfield(void)
{
	// move the stars

	// the stars are technically stationary,but we are going
	// to move them to simulate motion of the viewpoint
	// 通过移动背景 -- 星星 -- 来表达 视点的移动感
	for (int index = 0; index < NUM_STARS; index++)
	{
		// move the next star
		stars[index].z -= player_z_vel;

		// test for past near clipping plane
		if (stars[index].z <= NEAR_Z)
			stars[index].z = FAR_Z;
	}
}


void Draw_Starfield(void)
{
	// draw the stars in 3D using perspective transform
	COLOR* back_buffer = graphicOut.getMemory();
	const int back_lpitch = graphicOut.getLpitch();
	for (int index = 0; index < NUM_STARS; index++)
	{
		// draw the next star
		// step 1: perspective transform
		float x_per = VIEW_DISTANCE * stars[index].x / stars[index].z;
		float y_per = VIEW_DISTANCE * stars[index].y / stars[index].z;

		// step 2: compute screen coords
		int x_screen = WINDOW_WIDTH / 2 + x_per;
		int y_screen = WINDOW_HEIGHT / 2 - y_per;

		// clip to screen coords
		if (x_screen >= WINDOW_WIDTH || x_screen < 0 ||
			y_screen >= WINDOW_HEIGHT || y_screen < 0)
		{
			// continue to next star
			continue;
		}
		else
		{
			// else render to buffer
			(back_buffer)[x_screen + y_screen * back_lpitch] = stars[index].color;
		}
	}
}

void Process_Ties(void)
{
	// process the tie fighters and do AI (what there is of it!)

	// move each tie fighter toward the viewpoint
	for (int index = 0; index < NUM_TIES; index++)
	{
		// is this one dead?
		if (ties[index].state == 0)
			continue;

		// move the next star
		//可以看出这是绝对速度
		ties[index].z += ties[index].zv;
		ties[index].x += ties[index].xv;
		ties[index].y += ties[index].yv;

		// test for past near clipping plane
		if (ties[index].z <= NEAR_Z)
		{
			// reset this tie
			Init_Tie(index);

			// another got away
			misses++;

		}
	}
}

void Draw_Ties(void)
{
	// draw the tie fighters in 3D wireframe with perspective

	// used to compute the bounding box of tie fighter
	// for collision detection
	// 飞船的边框用来测试碰撞和射击
	int bmin_x, bmin_y, bmax_x, bmax_y;

	// draw each tie fighter
	for (int index = 0; index < NUM_TIES; index++)
	{
		// draw the next tie fighter

		// is this one dead?
		if (ties[index].state == 0)
			continue;

		// reset the bounding box to impossible values
		bmin_x = 100000;
		bmax_x = -100000;
		bmin_y = 100000;
		bmax_y = -100000;

		// based on z-distance shade tie fighter
		// normalize the distance from 0 to max_z then
		// scale it to 255, so the closer the brighter
		//飞机由远及近逐渐变亮
		COLOR rgb_tie_color = RGB_DX(0, (255 - 255 * (ties[index].z / (4 * FAR_Z))), 0);

		// each tie fighter is made of a number of edges
		for (int edge = 0; edge < NUM_TIE_EDGES; edge++)
		{
			POINT3D p1_per, p2_per; // used to hold perspective endpoints

			// step 1: perspective transform each end point
			// note the translation of each point to the position of the tie fighter
			// that is the model is relative to the position of each tie fighter -- IMPORTANT
			// 由物体坐标转化到世界坐标，然后再来透视
			p1_per.x =
				VIEW_DISTANCE * (ties[index].x + tie_vlist[tie_shape[edge].v1].x) /
				(tie_vlist[tie_shape[edge].v1].z + ties[index].z);

			p1_per.y = VIEW_DISTANCE * (ties[index].y + tie_vlist[tie_shape[edge].v1].y) /
				(tie_vlist[tie_shape[edge].v1].z + ties[index].z);

			p2_per.x = VIEW_DISTANCE * (ties[index].x + tie_vlist[tie_shape[edge].v2].x) /
				(tie_vlist[tie_shape[edge].v2].z + ties[index].z);

			p2_per.y = VIEW_DISTANCE * (ties[index].y + tie_vlist[tie_shape[edge].v2].y) /
				(tie_vlist[tie_shape[edge].v2].z + ties[index].z);

			// step 2: compute screen coords
			int p1_screen_x = WINDOW_WIDTH / 2 + p1_per.x;
			int p1_screen_y = WINDOW_HEIGHT / 2 - p1_per.y;
			int p2_screen_x = WINDOW_WIDTH / 2 + p2_per.x;
			int p2_screen_y = WINDOW_HEIGHT / 2 - p2_per.y;

			// step 3: draw the edge
			GRAPHIC::DrawLine(p1_screen_x, p1_screen_y, p2_screen_x, p2_screen_y,
				rgb_tie_color);

			// update bounding box with next edge
			int min_x = min(p1_screen_x, p2_screen_x);
			int max_x = max(p1_screen_x, p2_screen_x);

			int min_y = min(p1_screen_y, p2_screen_y);
			int max_y = max(p1_screen_y, p2_screen_y);

			bmin_x = min(bmin_x, min_x);
			bmin_y = min(bmin_y, min_y);

			bmax_x = max(bmax_x, max_x);
			bmax_y = max(bmax_y, max_y);

		}
		// test if this guy has been hit by lasers???
		if (cannon_state == 1)
		{
			//射中就是看是否在边界框内
			// simple test of screen coords of bounding box contain laser target
			if (target_x_screen > bmin_x && target_x_screen < bmax_x &&
				target_y_screen > bmin_y && target_y_screen < bmax_y)
			{
				// this tie is dead meat!
				Start_Explosion(index);

				// start sound
				explosionSound.play();

				// increase score
				score += ties[index].z;

				// add one more hit
				hits++;

				// finally reset this tie fighter
				Init_Tie(index);

			}
		}
	}
}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	keyboard.read();

	if (game_state == GAME_RUNNING)
	{
		// move players crosshair
		if (keyboard[DIK_RIGHT])
		{
			// move cross hair to right
			cross_x += CROSS_VEL;

			// test for wraparound
			if (cross_x > WINDOW_WIDTH / 2)
				cross_x = -WINDOW_WIDTH / 2;

		}
		if (keyboard[DIK_LEFT])
		{
			// move cross hair to left
			cross_x -= CROSS_VEL;

			// test for wraparound
			if (cross_x < -WINDOW_WIDTH / 2)
				cross_x = WINDOW_WIDTH / 2;
		}
		if (keyboard[DIK_DOWN])
		{
			// move cross hair up
			cross_y -= CROSS_VEL;

			// test for wraparound
			if (cross_y < -WINDOW_HEIGHT / 2)
				cross_y = WINDOW_HEIGHT / 2;
		}
		if (keyboard[DIK_UP])
		{
			// move cross hair up
			cross_y += CROSS_VEL;

			// test for wraparound
			if (cross_y > WINDOW_HEIGHT / 2)
				cross_y = -WINDOW_HEIGHT / 2;
		}

		// speed of ship controls  控制飞船速度，也就是控制背景速度
		if (keyboard[DIK_A])
			player_z_vel++;
		else if (keyboard[DIK_S])
			player_z_vel--;

		// test if player is firing laser cannon
		if (keyboard[DIK_SPACE] && cannon_state == 0)
		{
			// fire the cannon
			cannon_state = 1;
			cannon_count = 0;

			// save last position of target
			//开火时的瞄准位置才是真正的位置
			target_x_screen = cross_x_screen;
			target_y_screen = cross_y_screen;

			// make sound
			laserSound.play();

		}
	}
	// process cannon, simple FSM ready->firing->cool  FSM 有限状态机 在准备 开火 冷却中切换

	// firing phase
	if (cannon_state == 1)
		if (++cannon_count > 15)
			cannon_state = 2;

	// cool down phase  冷却时间
	if (cannon_state == 2)
		if (++cannon_count > 20)
			cannon_state = 0;

	// move the star field
	Move_Starfield();

	// move and perform ai for ties
	Process_Ties();

	// Process the explosions
	Process_Explosions();

	// lock the back buffer and obtain pointer and width
	BeginDrawOn();

	// draw the starfield
	Draw_Starfield();

	// draw the tie fighters
	Draw_Ties();

	// draw the explosions
	Draw_Explosions();

	// draw the cross hairs 

	// first compute screen coords of crosshair
	// note inversion of y-axis   y轴倒过来了
	cross_x_screen = WINDOW_WIDTH / 2 + cross_x;
	cross_y_screen = WINDOW_HEIGHT / 2 - cross_y;

	// draw the crosshair in screen coords
	GRAPHIC::DrawLine(cross_x_screen - 16, cross_y_screen,
		cross_x_screen + 16, cross_y_screen,
		rgb_red);

	GRAPHIC::DrawLine(cross_x_screen, cross_y_screen - 16,
		cross_x_screen, cross_y_screen + 16,
		rgb_red);

	GRAPHIC::DrawLine(cross_x_screen - 16, cross_y_screen - 4,
		cross_x_screen - 16, cross_y_screen + 4,
		rgb_red);

	GRAPHIC::DrawLine(cross_x_screen + 16, cross_y_screen - 4,
		cross_x_screen + 16, cross_y_screen + 4,
		rgb_red);

	// draw the laser beams  开火时绘制激光
	if (cannon_state == 1)
	{
		if ((rand() % 2 == 1))
		{
			// right beam
			GRAPHIC::DrawLine(WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1,
				-4 + rand() % 8 + target_x_screen, -4 + rand() % 8 + target_y_screen,
				RGB_DX(0, 128, 128 + rand() % 128));
		}
		else
		{
			// left beam
			GRAPHIC::DrawLine(0, WINDOW_HEIGHT - 1,
				-4 + rand() % 8 + target_x_screen, -4 + rand() % 8 + target_y_screen,
				RGB_DX(0, 128, 128 + rand() % 128));
		}
	}
	// done rendering, unlock back buffer surface
	EndDrawOn();

	gPrintf(0, 0, RGB(0, 255, 0), TEXT("Score %d     Kills %d      Escaped %d"), score, hits, misses);
	if (game_state == GAME_OVER)
		gPrintf(WINDOW_WIDTH / 2 - 8 * 10, WINDOW_HEIGHT / 2, RGB(255, 255, 255), TEXT("G A M E  O V E R"));

	//循环播放
	// check if the music has finished, if so restart
	if (!mainTrackMusic.isPlaying())
		mainTrackMusic.play();

	fpsSet.adjust();
	gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
	Flush();

	// check for game state switch
	if (misses > 4 * NUM_TIES)
		game_state = GAME_OVER;
}

void GameClose()
{

}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


