//#include "SDL.h"
//#include "SDL_image.h"
//#include "SDL_rotozoom.h"
//#include "SDL_mixer.h"
#include "test2.h" 
//#include "UsrFuc.h"
//#include "map.h"
#include "map_table.h" 
//#include "math.h"
#include "page.h" 
//#include "Camera.h"
//#include "Player.h"
//#include "rom.h"
//#include "Spritemap.h"
#include "include_all.h"
#include "string_pic.h"
#include "stringmap.h"
#include "stringpage.h" 
//#include "SPRITE.h"
#define Total    15
#define PIC_AMOUNT 2037
#define CLOUD_NUM 12 
//#define MAP_Y  TOP_HEIGHT-640
extern SDL_Surface *SpriteData[];
extern SDL_Rect rcSprite, rcGrass;
extern SDL_Surface * screen, *screen_buffer[], *sprite[];
extern int colorkey;
extern char type[];
extern Sprite Player_01;
extern Sprite Player_02;
extern Sprite Player_01_Shadow;
extern Sprite Player_02_Shadow;
extern Sprite Pig;
extern Sprite Turtle;
extern Sprite Turtle_Shadow;
extern Sprite Smoke;
extern int PIC;
enum AI{qk, back, run, move};

typedef struct AI_RULE
{
    SDL_Rect hit_position;
    bool hit;
    bool Slip;
    bool stay;
    bool route;
    int local;
    int StartX;
    int EndX;
    int shift;
    int time, endtime, backcount;
    AI ai;
};

typedef struct KEY
{
    bool *a, *b, *space;
};

typedef struct ROLE_RULE
{
    SDL_Rect offset, map; //offset:腳色座標,map:Buffer範圍座標
    fPoint jump_test, jumps; //jump_test:彈跳的高度，jumps:彈跳遞減
    float Power; //拍打速度，遞減 0~-45
    int y; //腳色高度
    int Height_arrow; //map_table 的 Class[] 的成員指標
    int select; //選擇第幾人物 0:阿公，1:阿罵
    int Impel; //被撞飛時X軸移動係數
    int Speed; //自定義frame更新速率 ，ex:Player_table.h 的播放速率 為-1時  播放速率則為此設定值
    int Count;
    int AI_Speed; //no use
    int Stay; //拍打停止時累加
    bool climb; //為true時表示正在攀岩
    bool Jump_To, Jumping, Bound_Jumping; //Jump_To:可否跳到下一層，Jumping:是否正在跳躍中，Bound_Jumping:是否被熊貓彈飛
    bool Left; //true為左，false:為右
    bool HIT; //是否被撞到
    bool KICK; //是否踢到烏龜
    bool Cam_up; //攝影機是否往上移動中
    bool couldRun, couldJump; //couldRun:拍打提示,couldJump:跳躍提示
    bool AutoMode; //玩家模式時為false,NPC模式為true

};

bool Game01();
void User_Interface( ROLE_RULE * PLAY, BUFFER_MANAGER b );
void AutoMode( ROLE_RULE * PLAY, KEY *p );
void Calcu_Rule( ROLE_RULE * PLAY );
void Input();
void motion( ROLE_RULE * PLAY, Player *Draw_Player );
void Running_Cam( ROLE_RULE * PLAY, int speed );
void init_Game01();
void showAnimal();
void showRun();
void showDrawbackground();
void showForeground();
void Game_Logic( ROLE_RULE *PLAY, KEY *Key );
int gameover;
bool upa, downa, space;
FPSmanager fps;



struct
{
    OBJECT Player;
    OBJECT Mala;
    SDL_Rect Cloud[ CLOUD_NUM ];
    SDL_Rect Pigs, Pigs2, Pigs3, Pigs4, Turtle1, Turtle2, Turtle3;
    SDL_Rect player, offset;
    AI_RULE aiPig01, aiPig02, aiPig03, aiPig04, aiTurtle01, aiTurtle02, aiTurtle03;
}

SPRITE;
ROLE_RULE PLAYER01;
ROLE_RULE PLAYER02;
bool key_s, key_a, key_w, key_d, key_z, key_x, key_c, key_v, key_b;
bool cam_up = false;
int y_1 = 7100, y_mod = 7100, y_2 = 7100, x_t = 1440 - 1440 / 5, x_t2 = 1440 - 1440 / 5, frame = 0, height_arrow = 0;

Multi_Map map = Multi_Map();
Player Draw_Player01 = Player();
Player Draw_Player02 = Player();
Player Draw_Player01_Shadow = Player();
Player Draw_Player02_Shadow = Player();
Player Pig01 = Player();
Player Pig02 = Player();
Player Pig03 = Player();
Player Pig04 = Player();
Player Turtle01 = Player();
Player Turtle02 = Player();
Player Turtle03 = Player();
Player Turtles_Shadow = Player();
Player Panda01 = Player();
Player Panda02 = Player();
Camera Screen( x_t2, y_2, 10 );
Camera Screen02( x_t2, y_2, 10 );
SDL_Rect xy1, xy2;
int selected01 = 0;
int selected02 = 0;
Particle abc;
KEY P1, P2;
//SDL_Rect loading[7]={0};
int Load( void *unused )
{
    //SDL_Surface *English[26];
    SDL_Rect loading = {2, 8, 0, 0};
    char * rom;

    long lSize;

    FILE * fp = fopen( "string_pic", "rb" );

    fseek ( fp , 0 , SEEK_END );

    lSize = ftell ( fp );

    rewind ( fp );

    rom = ( char* ) malloc( lSize );

    fread( rom, lSize, sizeof( char ), fp );

    fclose( fp );

    for ( int i = 2037;i < 2063;i++ )
    {
        int index = i - 2037;

        Load_Rom( RES_ENG + PAGE_ENG[ index ], ENG[ index ], i, rom );
    }

    free( rom );

    SDL_setFramerate( &fps , 30 );

    SDL_Rect rects;

    bool exit = 0;

    PIC = 0;

    while ( exit != 1 )
    {
        FONT load[ 7 ] = {L, O, A, D, I, N, G};
        memset( &rects, 0, sizeof( rects ) );
        rects.w = SCREEN_WIDTH;
        rects.h = SCREEN_HEIGHT;
        SDL_FillRect( screen, &rects, 0 );

        for ( int j = -3;j < 4;j++ )
        {
            int k = ( loading.y + ( ( j + 3 ) * 16 ) % 48 );
            DrawFont( load[ j + 3 ], SCREEN_WIDTH / 2 + j * 96 , SCREEN_HEIGHT / 3 + k , 0 );
        }

        ( loading.y += loading.x );
        loading.x = ( loading.y % 48 == 0 ) ? -loading.x : loading.x;
        int color = ( ( ( float ) PIC / PIC_AMOUNT ) * 0xff );
        exit = PIC == PIC_AMOUNT - 1;
        rects.x = 0;
        rects.y = SCREEN_HEIGHT / 2 + 16;
        rects.w = ( ( ( float ) PIC / PIC_AMOUNT ) * SCREEN_WIDTH );
        rects.h = 32;
        Set_Buffer( MAIN_BUFFER );
        SDL_FillRect( screen, &rects, 0xffff00 + color );
        SDL_framerateDelay( &fps );
        Print();
    }

    //for(int i=0;i<7;i++)
    //{
    // memset(&xy[i],0,sizeof(xy[i]));
    //}
    return 0;
}

int Gamemain( void *unused )
{
    Smoke.Catch();
    abc = Particle( &Smoke, 1 );
    int i = 8;
    gameover = 0;
    Sound_Init();
    init_Game01();

    while ( gameover != 1 )
    {
        Game01();
    }

    FreeChunk();

    Mix_CloseAudio();

    return 0;
}

/*   Sound_Chunk 目前使用情形  */
//  Chunk 0 ~ 9  walk.wav 跑步聲
//  Chunk 10 ~ 11 踢到烏龜音效
//  Chunk 12 ~ 13 被豬撞飛音效
//  Chunk 14  彈跳音效
/*           */
bool Game01()
{
    SDL_setFramerate( &fps , 30 );
    int j = 180, k = 2;
    float i = 1;

    SDL_Rect rects = {0};
    SDL_Rect r;

    //SDL_Rect abc,*cba;

    PLAYER01.AutoMode = 0;
    PLAYER02.AutoMode = 0;
    PLAYER02.Count = rand() % 65535;
    Sound_Play( "kick.mp3" );

    while ( !gameover )
    {
        /*  自動模式啟動 */
        AutoMode( &PLAYER01, &P1 );
        AutoMode( &PLAYER02, &P2 );
        /*  IO輸入   */
        Game_Logic( &PLAYER01, &P1 );
        Game_Logic( &PLAYER02, &P2 );

        /*  Y座標移動  */
        Calcu_Rule( &PLAYER01 );
        Calcu_Rule( &PLAYER02 );

        /*  X座標移動  */
        motion( &PLAYER01, &Draw_Player01 );
        motion( &PLAYER02, &Draw_Player02 );

        /*  設定攝影機自動跟隨  */
        Screen.setAuto( true );
        Screen.moveTo( &PLAYER01.map, SPEED4 );
        Screen.move();
        Screen02.setAuto( true );
        Screen02.moveTo( &PLAYER02.map, SPEED4 );
        Screen02.move();

        /*  設定地圖中心座標  */
        map.setMap_Center( 0, &Screen.getCenter() );
        map.setMap_Center( 1, &Screen02.getCenter() );

        /*  繪制位置地圖上所有物品  */
        map.Draw();

        /*  繪制UI介面     */
        User_Interface( &PLAYER01, LEFT_BUFFER );
        User_Interface( &PLAYER02, RIGHT_BUFFER );

        /*  拍打速率測試    */
        memset( &rects, 0, sizeof( rects ) );
        rects.w = ( ( PLAYER01.Power / -45 ) * 256 );
        rects.h = 32;
        Set_Buffer( LEFT_BUFFER );
        SDL_FillRect( screen, &rects, 0xffff00 );
        rects.x = BUFFER_WIDTH;
        rects.w = ( ( PLAYER02.Power / -45 ) * 256 );
        rects.h = 32;
        Set_Buffer( RIGHT_BUFFER );
        SDL_FillRect( screen, &rects, 0xffff00 );

        SDL_framerateDelay( &fps );
        Print_Double_Buffer(); /* update the screen */

        //SDL_Delay( 1 );

    }

    return true;
}

void User_Interface( ROLE_RULE * PLAY, BUFFER_MANAGER b )
{
    //static int

    if ( PLAY->Height_arrow == 24 )
    {
        PLAY->couldRun &= 0;
        PLAY->couldJump &= 0;
    }

    int X , Y;
    int frame = PLAY->couldJump ? 0 : 2 ;

    if ( ( PLAY->couldJump && PLAY->jumps.y == 0 ) || ( PLAY->couldRun && PLAY->Power > -5 ) )
    {
        OBJ draw;
        X = 128;
        Y = 256;
        draw = OBJ( X, Y, ( ( PLAY->Count % 12 ) / 6 ) + frame, 2033, -1 );
        //draw.Alpha = PLAY->Count%255 ;
        draw.showFlag = true;
        draw.setBuffer( b );
        draw.draw();
    }

    ++PLAY->Count;
}

void AutoMode( ROLE_RULE * PLAY, KEY *p )
{

    if ( PLAY->AutoMode && !PLAY->HIT && PLAY->Height_arrow != 24 )
    {
        if ( PLAY->couldRun )
        {
            if ( ( PLAY->Count ) % 3 == 0 )
            {
                if ( PLAY->Count % 2 == 0 )
                {
                    *p->a = 1;
                }
                else
                {
                    *p->b = 1;
                }
            }
        }

        if ( PLAY->couldJump )
        {
            if ( PLAY->Count % 10 == 0 )
            {
                *p->space = 1;
            }
            else
            {
                if ( PLAY->Count % 2 == 0 )
                {
                    *p->a = 1;
                }
                else
                {
                    *p->b = 1;
                }
            }
        }
    }
}

void Calcu_Rule( ROLE_RULE * PLAY )
{
    if ( PLAY->Height_arrow != 23 )
    {

        PLAY->offset.y = ( Class[ PLAY->Height_arrow ].Strat.v /* + ROLE_HEIGHT */ ) - ( /* TOP_HEIGHT - */Players[ PLAY->select ].y ) + PLAY->jump_test.y;

    }
    else if ( !PLAY->climb && PLAY->offset.y <= Class[ PLAY->Height_arrow + 1 ].Strat.v - 64 )
    {
        PLAY->offset.y = Class[ PLAY->Height_arrow + 1 ].Strat.v - 64 + PLAY->jump_test.y;
    }

    int ClassWidth = ( Class[ PLAY->Height_arrow + 1 ].End.x - Class[ PLAY->Height_arrow + 1 ].Strat.x );

    int ClassHeight = Class[ PLAY->Height_arrow + 1 ].Strat.v;

    int ClassX = ( Class[ PLAY->Height_arrow + 1 ].Strat.x ) + ( Class[ PLAY->Height_arrow + 1 ].End.x - Class[ PLAY->Height_arrow + 1 ].Strat.x ) / 2;

    int ClassY = Class[ PLAY->Height_arrow ].Strat.v;

    SDL_Rect point = {ClassX, ClassY, ClassWidth, ClassHeight};

    SDL_Rect point2 = {PLAY->offset.x + Players[ PLAY->select ].x , PLAY->offset.y, 0, 0};

    if ( ( hitTest( &point, &point2, point.w, point.h ) && !PLAY->climb ) || ( ( PLAY->climb && PLAY->offset.y <= Class[ PLAY->Height_arrow + 1 ].Strat.v - 64 ) && PLAY->Height_arrow == 23 ) )
    {
        if ( PLAY->Height_arrow == 12 )
        {
            if ( point2.x >= point.x + 64 )
            {
                PLAY->Jump_To = true;
                PLAY->couldJump = true;
                PLAY->couldRun = false;
            }

        }
        else
        {
            PLAY->Jump_To = true;
            PLAY->couldJump = true;
            PLAY->couldRun = false;
        }
    }
    else if ( PLAY->Height_arrow != 23 )
    {
        PLAY->couldRun = true;
        PLAY->couldJump = false;
        PLAY->Jump_To = false;
    }
    else if ( PLAY->climb )
    {
        PLAY->couldJump = false;
        PLAY->couldRun = true;
    }

    if ( PLAY->Jump_To && PLAY->offset.y >= ( Class[ PLAY->Height_arrow + 1 ].Strat.v /* + ROLE_HEIGHT */ ) - ( /* TOP_HEIGHT - */Players[ PLAY->select ].y ) && PLAY->offset.y <= ( Class[ PLAY->Height_arrow + 1 ].Strat.v /*+ ROLE_HEIGHT */ ) - ( /* TOP_HEIGHT - */Players[ PLAY->select ].y ) + 64 && PLAY->jumps.y > 0 )
    {
        PLAY->Cam_up = true;
        PLAY->Bound_Jumping &= 0;
        PLAY->jump_test.y = 0;
        //PLAY->Jump_To&=0;
        //PLAY->Jumping&=0;
        PLAY->Power = 0;
    }

    if ( PLAY->Cam_up )
    {
        PLAY->couldJump = false;
        PLAY->couldRun = false;
        PLAY->offset.y = ( Class[ PLAY->Height_arrow + 1 ].Strat.v /* + ROLE_HEIGHT */ ) - ( /* TOP_HEIGHT - */Players[ PLAY->select ].y );
    }
    else
    {

        //PLAY->map.y+=PLAY->jumps.y/2;
    }


    PLAY->jump_test.y += PLAY->jumps.y;

    if ( PLAY->jumps.y > 0 && PLAY->jump_test.y > 0 )
    {
        PLAY->jump_test.y = 0;
        PLAY->jumps.y = 0;

    }
    else if ( PLAY->jump_test.y < 0 )
    {
        PLAY->jumps.y += 2;
    }
}

//void Calcu_Rule(ROLE_RULE * PLAY)
//{
//
//    if ( Class[ height_arrow ].Strat.v == Class[ 0 ].Strat.v )
//        SPRITE.offset.y = y_1 + ( SPRITE.player.y + 192 ) + SPRITE.jump_test.y;
//    else
//        SPRITE.offset.y = y_1 + ( SPRITE.player.y /*+ ROLE_HEIGHT*/ ) + SPRITE.jump_test.y;
//
//    int ClassWidth = Class[ height_arrow + 1 ].End.x - Class[ height_arrow + 1 ].Strat.x;
//
//    int ClassHeight = Class[ height_arrow + 1 ].Strat.v;
//
//    int ClassX = (Class[ height_arrow + 1 ].Strat.x) + ( Class[ height_arrow + 1 ].End.x - Class[ height_arrow + 1 ].Strat.x) / 2;
//
//    int ClassY = Class[ height_arrow ].Strat.v;
//
//    SDL_Rect point = {ClassX, ClassY, ClassWidth, ClassHeight};
//
//    SDL_Rect point2 = {SPRITE.offset.x+Players[ selected01 ].x , SPRITE.offset.y, 0, 0};
//
//    if ( hitTest( &point, &point2, point.w, point.h ) )
//    {
//        SPRITE.Jump_To = true;
//    }
//    else
//    {
//        SPRITE.Jump_To &= 0;
//    }
//
//    if ( SPRITE.Jump_To && SPRITE.offset.y >= ( Class[ height_arrow + 1 ].Strat.v /*+ ROLE_HEIGHT*/ ) - ( TOP_HEIGHT - Players[ selected01 ].y ) && SPRITE.offset.y <= ( Class[ height_arrow + 1 ].Strat.v /*+ ROLE_HEIGHT*/ ) - ( TOP_HEIGHT - Players[ selected01 ].y ) + 64 && SPRITE.jumps.y > 0 )
//    {
//        cam_up = true;
//  SPRITE.Bound_Jumping&=0;
//        SPRITE.Mala.gpoint.y = 0;
//    }
//
//    if ( cam_up )
//    {
//        SPRITE.offset.y = ( Class[ height_arrow + 1 ].Strat.v /*+ ROLE_HEIGHT*/ ) - ( TOP_HEIGHT - Players[ selected01 ].y );
//    }
//    else
//    {
//
//        //y_1+=SPRITE.jumps.y/2;
//    }
//
//    //if(Events[  height_arrow  ].Amount==0)
//    //{
//    // SPRITE.Jump_To&=0;
//    //}
//    /*
//    for(int i=0;i<Events[  height_arrow  ].Amount;i++)
//    {
//     SDL_Rect point={Events[  height_arrow  ].POINT[i].x+Events[  height_arrow  ].POINT[i].v/2,Class[  height_arrow  ].Strat.v,Events[  height_arrow  ].POINT[i].v/2,Class[  height_arrow ].Strat.v-Class[  height_arrow - 1 ].Strat.v};
//     SDL_Rect point2={SPRITE.offset.x+250/2,SPRITE.offset.y,0,0};
//     if(hitTest(&point,&point2,point.w,point.h))
//     {
//      SPRITE.Jump_To=true;
//      break;
//     }else
//     {
//      SPRITE.Jump_To&=0;
//     }
//    }
//    */
//
//
//    SPRITE.jump_test.y += SPRITE.jumps.y;
//
//    if ( SPRITE.jumps.y > 0 && SPRITE.jump_test.y > 0 )
//    {
//        SPRITE.jump_test.y = 0;
//        SPRITE.jumps.y = 0;
//
//    }
//    else if ( SPRITE.jump_test.y < 0 )
//    {
//        SPRITE.jumps.y += 2;
//    }
//}
void PigAI( Player *Pig, SDL_Rect *Pigxy, AI_RULE *rule )
{
    //static int rule->backcount;
    SDL_Rect a = {PLAYER01.offset.x + 250 / 2, PLAYER01.offset.y + 250 / 2, 0, 0};
    SDL_Rect c = {PLAYER02.offset.x + 250 / 2, PLAYER02.offset.y + 250 / 2, 0, 0};
    SDL_Rect b = {Pigxy->x + 250 / 2, Pigxy->y + 250 / 2, 0, 0};
    float speed = ( ( float ) Pigxy->h / 255 );
    float Speedleft = ( speed * 30 );

    switch ( rule->ai )
    {
    case run:
        speed = ( ( float ) Pigxy->h / 255 );
        //Pig->Update("Run");


        Speedleft = ( speed * 30 ) + 6;

        if ( rule->hit || rule->time >= 1 )
        {

            if ( ++rule->time > 1 )
            {
                rule->hit_position.w++;
                rule->Slip = true;
                Pigxy->h = Pigxy->h - Pigxy->h / 15;
                rule->time = Pigxy->h / 15 < 2 ? 0 : rule->time;
                Speedleft = ( speed * 10 );

                if ( rule->time == 0 )
                {
                    rule->hit &= 0;
                    rule->ai = qk;
                }

                Pig->Update( "Stop" );

                if ( Pigxy->w )
                    abc.SetCenter( 1, Pig->Offset.X + 74, Pig->Offset.Y + 238 );
                else
                    abc.SetCenter( 1, Pig->Offset.X + 174, Pig->Offset.Y + 238 );

                abc.SetLeft( Pigxy->w );
            }
            else
            {
                Pig->Update( "Run" );
            }
        }
        else
        {
            if ( !PLAYER01.Cam_up && !rule->hit && hitTest( &a, &b, 96, 128 / 2 ) && PLAYER01.jumps.y == 0 && !PLAYER01.HIT )
            {
                Play_Chunk( "bound1.wav", 12 );
                rule->hit_position = PLAYER01.offset;
                PLAYER01.Left = Pigxy->w ;
                rule->time = 60;
                rule->hit = true;
                Pigxy->h = 255;
                rule->hit_position.w = 0;
                PLAYER01.HIT = true;
                PLAYER01.jumps.y = -25;
                Play_Chunk( "bluk.wav", 15 );
            }
            else if ( !PLAYER02.Cam_up && !rule->hit && hitTest( &c, &b, 96, 128 / 2 ) && PLAYER02.jumps.y == 0 && !PLAYER02.HIT )
            {
                Play_Chunk( "bound1.wav", 13 );
                rule->hit_position = PLAYER02.offset;
                rule->hit_position.w = 0;
                PLAYER02.Left = Pigxy->w ;
                rule->time = 90;
                rule->hit = true;
                Pigxy->h = 255;
                PLAYER02.HIT = true;
                PLAYER02.jumps.y = -25;
                Play_Chunk( "bluk.wav", 15 );
            }

            float range = ( float ) abs( rule->StartX - Pigxy->x ) / abs( rule->EndX - rule->StartX ) * 3.0f + 1.0f;
            int shift = rule->shift + range;

            if ( rule->EndX > rule->StartX && Pigxy->x >= rule->EndX )
            {
                Play_Chunk( "bluk.wav", 15 );
                rule->time++;
            }

            if ( rule->EndX < rule->StartX && Pigxy->x <= rule->EndX )
            {
                Play_Chunk( "bluk.wav", 15 );
                rule->time++;
            }

            Pigxy->h = Pigxy->h < 255 ? Pigxy->h + shift : 255;
            Pig->Update( "Run" );
        }

        break;
    case back:
        rule->Slip &= 0;
        Pigxy->w = Pigxy->x > rule->StartX;
        speed = 0.6f;
        Speedleft = 12;

        if ( Pigxy->x < rule->StartX + 10 && Pigxy->x > rule->StartX - 10 )
        {
            Pigxy->w = 0;
            rule->ai = qk;
            Pigxy->w = rand() % 10 > 5 ? 1 : 0;
        }

        Pig->Update( "Run" );
        break;
    case qk:

        rule->Slip &= 0;
        Speedleft = 0;
        Pig->Update( "Stand" );

        if ( !PLAYER01.Cam_up && !PLAYER01.HIT && rule->ai == qk && hitTest( &a, &b, 1540, 6 ) )
        {
            if ( ++rule->backcount > 4 )
            {
                rule->backcount = 0;
                rule->time = rule->endtime;
            }
        }
        else if ( !PLAYER02.Cam_up && !PLAYER02.HIT && rule->ai == qk && hitTest( &c, &b, 1540, 6 ) )
        {
            if ( ++rule->backcount > 4 )
            {
                rule->backcount = 0;
                rule->time = rule->endtime;
            }
        }

        if ( ++rule->time > rule->endtime )
        {
            int ranges = abs( rule->EndX - rule->StartX ) / 3;
            srand( rule->endtime + rand() % 10 * 0xff );
            rule->time = 0;
            rule->ai = ( rule->EndX > rule->StartX && Pigxy->x >= rule->EndX - ranges ) || ( rule->EndX < rule->StartX && Pigxy->x <= rule->EndX + ranges ) /* || (!hitTest( &a, &b, 1920, 6 ) &&!hitTest( &c, &b, 1920, 6 ))*/ ? back : qk;

            if ( rule->ai != back )
            {
                if ( !PLAYER01.Cam_up && !PLAYER01.HIT && rule->ai == qk && hitTest( &a, &b, 1540, 6 ) )
                {
                    Pigxy->w = Pigxy->x - PLAYER01.offset.x > 0;
                    rule->ai = run;
                    //a=b;//PLAYER01.aiPig01=
                }
                else if ( !PLAYER02.Cam_up && !PLAYER02.HIT && rule->ai == qk && hitTest( &c, &b, 1540, 6 ) )
                {
                    Pigxy->w = Pigxy->x - PLAYER02.offset.x > 0;
                    rule->ai = run;
                    //a=b;//PLAYER01.aiPig01=
                }
                else
                {
                    rule->stay = true;
                }
            }

            //rule->ai = rule->ai== run && Pigxy->w!=rule->route ? qk : rule->ai ;
            if ( rule->ai == run && Pigxy->w != rule->route )
                Pigxy->w = rule->route;

            if ( rule->ai == 3 )
                Pigxy->w = rand() % 100 < 50;

            rule->endtime = rand() % 80 + 30;
        }

        break;
    case 3:
        //rule->Slip &= 0;
        //Pig->Update("Run");
        //speed = 0.5f;
        //Speedleft = 5;

        //if ( !PLAYER01.HIT && rule->ai == move && hitTest( &a, &b, 512, 250 / 2 ) )
        //{
        //    rule->time = 0;
        //    Pigxy->w = Pigxy->x - PLAYER01.offset.x > 0;
        //    rule->ai = run;
        //    //a=b;//PLAYER01.aiPig01=
        //}

        //if ( ++rule->time > rule->endtime )
        //{
        //    rule->time = 0;
        //    rule->ai = qk;
        //    rule->endtime = rand() % 50 + 30;
        //}

        break;
    }

    Pig->setRate( speed );

    if ( !rule->stay )
    {
        if ( Pigxy->w == 0 )
        {
            Pigxy->x += Speedleft;

            if ( Pigxy->x > 1920 - 250 )
                Pigxy->w = 1;
        }

        if ( Pigxy->w == 1 )
        {
            Pigxy->x -= Speedleft;

            if ( ( ( rule->StartX < 0 ) && Pigxy->x <= ( rule->StartX ) ) || ( ( rule->StartX > 0 ) && Pigxy->x <= 0 ) )
                Pigxy->w = 0;
        }
    }
    else
    {
        Pigxy->w = rule->route;

        if ( !rule->route && ( ( a.x > rule->StartX + 250 && hitTest( &a, &b, 640, 320 ) ) || ( c.x > rule->StartX + 250 && hitTest( &c, &b, 640, 320 ) ) ) )
        {
            rule->stay = false;
        }
        else if ( rule->route && ( ( a.x < rule->StartX && hitTest( &a, &b, 640, 6 ) ) || ( c.x < rule->StartX && hitTest( &c, &b, 640, 6 ) ) ) )
        {
            rule->stay = false;
        }

        if ( rule->ai == run )
            rule->ai = qk;

    }

    //Pig->draw( buffer );
}

void drawPig( Player *Pig, SDL_Rect *Pigxy, AI_RULE *rule , ROLE_RULE * PLAY, BUFFER_MANAGER buffer )
{
    float speed = ( ( float ) Pigxy->h / 255.0f );
    Pig->setMap( PLAY->map );
    Pig->LEFT( Pigxy->w );
    Pig->setOffset( Pigxy );

    //Pig->Update();
    Pig->draw( buffer );

    if ( Pig->getShowFlag() && rule->Slip )
    {
        speed = 1.0f - speed ;

        if ( Pigxy->w )
            abc.SetCenter( 1, Pig->Offset.X + 74, Pig->Offset.Y + 238 );
        else
            abc.SetCenter( 1, Pig->Offset.X + 174, Pig->Offset.Y + 238 );

        abc.SetLeft( Pigxy->w );

        abc.SetBuffer( buffer );

        abc.Start( 1, speed );
    }

    if ( rule->hit && rule->hit_position.w < 15 )
    {
        int RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( rule->hit_position.x - PLAY->map.x ) );
        int RunY = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( rule->hit_position.y + 32 - PLAY->map.y ) );
        OBJ effect;

        if ( Pigxy->w )
        {
            RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( rule->hit_position.x - 64 - PLAY->map.x ) );
            effect = OBJ( RunX, RunY, rule->hit_position.w, 1136, -1 );
        }
        else
            effect = OBJ( RunX, RunY, rule->hit_position.w, 1151, -1 );

        //rule->hit_position.w++;
        effect.showFlag = abs( effect.x ) < BUFFER_WIDTH && abs( effect.y ) < BUFFER_HEIGHT;

        effect.setBuffer( buffer );

        effect.draw();
    }
}

void TurtleAI( Player *Turtle, SDL_Rect *Turtlexy, AI_RULE *rule )
{
    //static int rule->backcount;
    SDL_Rect a = {PLAYER01.offset.x + 250 / 2, PLAYER01.offset.y + 250, 0, 0};
    SDL_Rect c = {PLAYER02.offset.x + 250 / 2, PLAYER02.offset.y + 250, 0, 0};
    SDL_Rect b = {Turtlexy->x + Animals[ 2 ].x, rule->local + Animals[ 2 ].y, 0, 0};
    //float speed = ( ( float ) Turtlexy->h / 255 );
    float Speedleft = 0; // = ( speed * 8 );
    int setY = 0;

    switch ( rule->ai )
    {
    case run:
        Turtle->Update( "Run" );
        Speedleft = 2 ;

        if ( hitTest( &a, &b, 128, 64 ) || hitTest( &c, &b, 128, 64 ) )
        {
            //Play( "bound2.wav" );
            Turtlexy->h = 0;
            rule->time = 0;
            rule->ai = back;
        }
        else if ( ++rule->time > rule->endtime )
        {
            Turtlexy->h = 0;
            rule->time = 0;
            rule->ai = qk;
        }

        break;
    case back:

        if ( ++Turtlexy->h < 5 )
        {
            setY = ( ( float ) Turtlexy->h / 5.0f ) * -32.0f;
            Turtle->Update( "Back1" );
        }
        else if ( Turtlexy->h < 10 )
        {
            setY = ( ( 10.0f - ( float ) Turtlexy->h ) / 5.0f ) * -38.0f;
            Turtle->Update( "Back2" );
        }
        else
        {
            Turtle->Update( "Inside" );
        }

        if ( Turtlexy->h > 100 && !hitTest( &a, &b, 250, 64 ) && !hitTest( &c, &b, 250, 64 ) )
        {
            Turtlexy->h = 0;
            rule->time = 0;
            rule->ai = qk;
            Turtle->Update( "Out" );
        }

        break;
    case qk:
        Turtle->Update( "Stand" );

        if ( hitTest( &a, &b, 128, 64 ) || hitTest( &c, &b, 128, 64 ) )
        {
            Turtlexy->h = 0;
            rule->time = 0;
            rule->ai = back;
        }

        if ( ++rule->time > rule->endtime )
        {
            Turtlexy->h = 0;
            srand( rule->endtime + rand() % 10 * 0xff );
            Turtlexy->w = rand() % 2;
            rule->endtime = 30 + rand() % 20;
            rule->time = 0;
            rule->ai = run;

        }

        break;
    case 3:
        break;
    }

    if ( !PLAYER01.HIT && hitTest( &a, &b, 80, 24 ) && PLAYER01.jumps.y >= 0 )
    {

        Play_Chunk( "bound2.wav", 10 );
        PLAYER01.HIT = true;
        PLAYER01.KICK = true;
        PLAYER01.jumps.y = -15;
        Turtlexy->h = 7;
    }
    else if ( hitTest( &a, &b, 154, 24 ) && PLAYER01.jumps.y < 0 )
    {
        PLAYER01.Power = PLAYER01.Power < -25 ? PLAYER01.Power : -25;
    }

    if ( !PLAYER02.HIT && hitTest( &c, &b, 80, 24 ) && PLAYER02.jumps.y >= 0 )
    {
        Play_Chunk( "bound2.wav", 11 );

        PLAYER02.HIT = true;
        PLAYER02.KICK = true;
        PLAYER02.jumps.y = -15;
        Turtlexy->h = 7;
    }
    else if ( hitTest( &c, &b, 154, 24 ) && PLAYER02.jumps.y < 0 )
    {
        PLAYER02.Power = PLAYER02.Power < -25 ? PLAYER02.Power : -25;
    }

    //Turtle->setRate( speed );
    if ( Turtlexy->w == 0 )
    {
        Turtlexy->x += Speedleft;

        if ( Turtlexy->x > rule->EndX )
            Turtlexy->w = 1;
    }

    if ( Turtlexy->w == 1 )
    {
        Turtlexy->x -= Speedleft;

        if ( Turtlexy->x < rule->StartX )
            Turtlexy->w = 0;
    }

    Turtlexy->y = rule->local + setY;
}

void drawTurtle( Player *Turtle, SDL_Rect *Turtlexy, AI_RULE *rule , ROLE_RULE * PLAY, BUFFER_MANAGER buffer )
{
    SDL_Rect Shadow = {Turtlexy->x, rule->local, 0, 0};
    Turtles_Shadow.setFrame( Turtle->getFrame() );
    Turtles_Shadow.setMap( PLAY->map );
    Turtles_Shadow.LEFT( Turtlexy->w );
    Turtles_Shadow.setOffset( &Shadow );
    Turtles_Shadow.draw( buffer );
    Turtle->setMap( PLAY->map );
    Turtle->LEFT( Turtlexy->w );
    Turtle->setOffset( Turtlexy );
    //Turtle->Update();
    Turtle->draw( buffer );
}

//void PigAI( Player *Pig, SDL_Rect *Pigxy, AI_RULE *rule ,ROLE_RULE * PLAY,BUFFER_MANAGER buffer)
//{
// static int rule->backcount=0;
//    SDL_Rect a = {SPRITE.offset.x + 250 / 2, SPRITE.offset.y + 250 / 2, 0, 0};
//    SDL_Rect b = {Pigxy->x + 250 / 2, Pigxy->y + 250 / 2, 0, 0};
//    float speed = ( ( float ) Pigxy->h / 255 );
//    float Speedleft = ( speed * 30 );
//
//    switch ( rule->ai )
//    {
//    case run:
//        speed = ( ( float ) Pigxy->h / 255 );
//  //Pig->Update("Run");
//        if ( !rule->hit && hitTest( &a, &b, 128 / 2, 128 / 2 ) )
//        {
//            SPRITE.Left = Pigxy->w ;
//            rule->time = 90;
//            rule->hit = true;
//            Pigxy->h = 255;
//            SPRITE.HIT = true;
//            SPRITE.jumps.y = -25;
//        }
//
//        Speedleft = ( speed * 30 );
//
//        if ( rule->hit || Pigxy->h > 254 || rule->time > 0 )
//        {
//            if ( ++rule->time > 90 )
//            {
//                rule->Slip = true;
//                Pigxy->h = Pigxy->h - Pigxy->h / 15;
//                rule->time = Pigxy->h / 15 < 2 ? 0 : rule->time;
//                Speedleft = ( speed * 10 );
//
//                if ( rule->time == 0 )
//                {
//                    rule->hit &= 0;
//                    rule->ai = qk;
//                }
//
//                Pig->Update("Stop");
//
//                if ( Pigxy->w )
//                    abc.SetCenter( 1, Pig->Offset.X + 74, Pig->Offset.Y + 238 );
//                else
//                    abc.SetCenter( 1, Pig->Offset.X + 174, Pig->Offset.Y + 238 );
//
//                abc.SetLeft( Pigxy->w );
//            }
//            else
//            {
//                Pig->Update("Run");
//            }
//        }
//        else
//        {
//            Pigxy->h = Pigxy->h < 255 ? Pigxy->h + rule->shift : 255;
//            Pig->Update("Run");
//        }
//
//        break;
//    case back:
//        rule->Slip &= 0;
//        Pigxy->w = 1;
//        speed = 0.6f;
//        Speedleft = 8;
//
//        if ( Pigxy->x - Speedleft < 0 )
//        {
//            Pigxy->w = 0;
//            rule->ai = qk;
//            Pigxy->w = rand() % 10 > 5 ? 1 : 0;
//        }
//
//        Pig->Update("Run");
//        break;
//    case qk:
//
//        rule->Slip &= 0;
//        Speedleft = 0;
//        Pig->Update("Stand");
//  if ( !cam_up && !SPRITE.HIT && rule->ai == qk && hitTest( &a, &b, 640, 6 ))
//        {
//   if(++rule->backcount>10)
//   {
//    rule->backcount = 0;
//    rule->time = rule->endtime;
//   }
//        }
//        if ( ++rule->time > rule->endtime )
//        {
//            srand( rule->endtime + rand() % 10 * 0xff );
//            rule->time = 0;
//
//            rule->ai = Pigxy->x < 1920 / 3 ? qk : back;
//
//            if ( !cam_up && !SPRITE.HIT && rule->ai == qk && hitTest( &a, &b, 640, 6 ) )
//            {
//                Pigxy->w = Pigxy->x - SPRITE.offset.x > 0;
//                rule->ai = run;
//                //a=b;//SPRITE.aiPig01=
//            }
//
//            if ( rule->ai == move )
//                Pigxy->w = rand() % 100 < 50;
//
//            rule->endtime = rand() % 80 + 30;
//        }
//
//        break;
//    case move:
//        rule->Slip &= 0;
//        Pig->Update("Run");
//        speed = 0.5f;
//        Speedleft = 5;
//
//        if ( !SPRITE.HIT && rule->ai == move && hitTest( &a, &b, 512, 250 / 2 ) )
//        {
//            rule->time = 0;
//            Pigxy->w = Pigxy->x - SPRITE.offset.x > 0;
//            rule->ai = run;
//            //a=b;//SPRITE.aiPig01=
//        }
//
//        if ( ++rule->time > rule->endtime )
//        {
//            rule->time = 0;
//            rule->ai = qk;
//            rule->endtime = rand() % 50 + 30;
//        }
//
//        break;
//    }
//
//    Pig->setRate( speed );
//
//    if ( Pigxy->w == 0 )
//    {
//        Pigxy->x += Speedleft;
//
//        if ( Pigxy->x > 1920 - 250 )
//            Pigxy->w = 1;
//    }
//
//    if ( Pigxy->w == 1 )
//    {
//        Pigxy->x -= Speedleft;
//
//        if ( Pigxy->x < 0 )
//            Pigxy->w = 0;
//    }
//
//    Pig->setMap( PLAYER01.map );
//    Pig->LEFT( Pigxy->w );
//    Pig->setOffset( Pigxy );
//
//    //Pig->Update();
//    Pig->draw( LEFT_BUFFER );
//
//    if ( Pig->getShowFlag() && rule->Slip )
//    {
//        abc.SetBuffer( LEFT_BUFFER );
//        abc.Start( 1, abs( 1 - speed ) );
//    }
// Pig->draw( RIGHT_BUFFER );
// if ( Pig->getShowFlag() && rule->Slip )
//    {
//  abc.SetBuffer( RIGHT_BUFFER );
//        abc.Start( 1, abs( 1 - speed ) );
//    }
//
//}
void drawPanda( bool *bound, int arrow, Player *Panda, SDL_Rect * offset, ROLE_RULE * PLAY, BUFFER_MANAGER buffer )
{
    SDL_Rect a = {PLAYER01.offset.x + 250 / 2, PLAYER01.offset.y + 250 / 2, 0, 0};
    SDL_Rect c = {PLAYER02.offset.x + 250 / 2, PLAYER02.offset.y + 250 / 2, 0, 0};
    SDL_Rect b = {offset->x + 250 / 2, offset->y + 250 / 2, 0, 0};
    int count = 0;

    if ( ( PLAYER01.Bound_Jumping && PLAYER01.Height_arrow == arrow ) || ( PLAYER02.Bound_Jumping && PLAYER02.Height_arrow == arrow ) )
    {
        *bound = Panda->Update( "Bound" );
    }
    else if ( !*bound )
    {
        Panda->Update( "Stand" );
    }

    if ( ( PLAYER01.Jumping || PLAYER01.HIT ) && hitTest( &a, &b, 52, 48 ) )
    {
        Play_Chunk( "bounce03.wav", 14 );
        PLAYER01.Bound_Jumping = true;
        PLAYER01.HIT &= 0;
        *P1.space &= 0;
        PLAYER01.jumps.y = -48;
        count = 0;
        Panda->Update( "Stand" );
    }
    else if ( !*bound )
    {
        if ( hitTest( &a, &b, 132, 32 ) )
            if ( PLAYER01.offset.x < offset->x )
                PLAYER01.offset.x = offset->x - 96;
            else if ( PLAYER01.offset.x > offset->x )
                PLAYER01.offset.x = offset->x + 132;
    }

    if ( ( PLAYER02.Jumping || PLAYER02.HIT ) && hitTest( &c, &b, 52, 48 ) )
    {
        Play_Chunk( "bounce03.wav", 14 );
        PLAYER02.Bound_Jumping = true;
        PLAYER02.HIT &= 0;
        *P2.space &= 0;
        PLAYER02.jumps.y = -48;
        Panda->Update( "Stand" );
    }
    else if ( !*bound )
    {
        if ( hitTest( &c, &b, 132, 32 ) )
            if ( PLAYER02.offset.x < offset->x )
                PLAYER02.offset.x = offset->x - 96;
            else if ( PLAYER02.offset.x > offset->x )
                PLAYER02.offset.x = offset->x + 132;
    }

    Panda->setMap( PLAYER01.map );
    Panda->setOffset( offset );
    Panda->draw( LEFT_BUFFER );
    Panda->setMap( PLAYER02.map );
    //Panda->setOffset(offset);
    Panda->draw( RIGHT_BUFFER );

}

void drawCaveMask( BUFFER_MANAGER b, SDL_Rect *map )
{
    //static int count=0;
    int X , Y;
    OBJ draw;
    /*  Waterfall 01 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 1343 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 3898 - map->y ) );
    draw = OBJ( X, Y, 1, 1408, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && ( ( draw.y < 0 && abs( draw.y ) < SpriteData[ 1409 ] ->h ) || ( draw.y > 0 && draw.y < BUFFER_HEIGHT ) );
    draw.setBuffer( b );
    draw.draw();
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 0 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4768 - map->y ) );
    draw = OBJ( X, Y, 2, 1408, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && ( ( draw.y < 0 && abs( draw.y ) < SpriteData[ 1410 ] ->h ) || ( draw.y > 0 && draw.y < BUFFER_HEIGHT ) );
    draw.setBuffer( b );
    draw.draw();
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 0 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 6254 - map->y ) );
    draw = OBJ( X, Y, 3, 1408, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && ( ( draw.y < 0 && abs( draw.y ) < SpriteData[ 1411 ] ->h ) || ( draw.y > 0 && draw.y < BUFFER_HEIGHT ) );
    draw.setBuffer( b );
    draw.draw();
}

void showAnimal()
{
    static bool onlyPlay01 = 0, onlyPlay02 = 0;
    SDL_Rect Panda1 = {Class[ 12 ].End.x - Animal_Panda.x, Class[ 13 ].End.v + Animal_Panda.y, 0, 0};
    SDL_Rect Panda2 = {Class[ 15 ].Strat.x - Animal_Panda.x, Class[ 15 ].End.v + Animal_Panda.y - 32, 0, 0};

    PigAI( &Pig01, &SPRITE.Pigs, &SPRITE.aiPig01 );
    PigAI( &Pig02, &SPRITE.Pigs2, &SPRITE.aiPig02 );
    PigAI( &Pig03, &SPRITE.Pigs3, &SPRITE.aiPig03 );
    PigAI( &Pig04, &SPRITE.Pigs4, &SPRITE.aiPig04 );

    TurtleAI( &Turtle01, &SPRITE.Turtle1, &SPRITE.aiTurtle01 );
    TurtleAI( &Turtle02, &SPRITE.Turtle2, &SPRITE.aiTurtle02 );
    TurtleAI( &Turtle03, &SPRITE.Turtle3, &SPRITE.aiTurtle03 );


    drawPig( &Pig01, &SPRITE.Pigs, &SPRITE.aiPig01 , &PLAYER01, LEFT_BUFFER );
    drawPig( &Pig01, &SPRITE.Pigs, &SPRITE.aiPig01 , &PLAYER02, RIGHT_BUFFER );
    drawPig( &Pig02, &SPRITE.Pigs2, &SPRITE.aiPig02 , &PLAYER01, LEFT_BUFFER );
    drawPig( &Pig02, &SPRITE.Pigs2, &SPRITE.aiPig02 , &PLAYER02, RIGHT_BUFFER );
    drawPig( &Pig03, &SPRITE.Pigs3, &SPRITE.aiPig03 , &PLAYER01, LEFT_BUFFER );
    drawPig( &Pig03, &SPRITE.Pigs3, &SPRITE.aiPig03 , &PLAYER02, RIGHT_BUFFER );
    drawPig( &Pig04, &SPRITE.Pigs4, &SPRITE.aiPig04 , &PLAYER01, LEFT_BUFFER );
    drawPig( &Pig04, &SPRITE.Pigs4, &SPRITE.aiPig04 , &PLAYER02, RIGHT_BUFFER );

    drawTurtle( &Turtle01, &SPRITE.Turtle1, &SPRITE.aiTurtle01 , &PLAYER01, LEFT_BUFFER );
    drawTurtle( &Turtle01, &SPRITE.Turtle1, &SPRITE.aiTurtle01 , &PLAYER02, RIGHT_BUFFER );
    drawTurtle( &Turtle02, &SPRITE.Turtle2, &SPRITE.aiTurtle02 , &PLAYER01, LEFT_BUFFER );
    drawTurtle( &Turtle02, &SPRITE.Turtle2, &SPRITE.aiTurtle02 , &PLAYER02, RIGHT_BUFFER );
    drawTurtle( &Turtle03, &SPRITE.Turtle3, &SPRITE.aiTurtle03 , &PLAYER01, LEFT_BUFFER );
    drawTurtle( &Turtle03, &SPRITE.Turtle3, &SPRITE.aiTurtle03 , &PLAYER02, RIGHT_BUFFER );

    drawCaveMask( LEFT_BUFFER, &Screen.getCenter() );
    drawCaveMask( RIGHT_BUFFER, &Screen02.getCenter() );

    drawPanda( &onlyPlay01, 12, &Panda01, &Panda1, &PLAYER01, LEFT_BUFFER );
    drawPanda( &onlyPlay02, 14, &Panda02, &Panda2, &PLAYER01, LEFT_BUFFER );
}

void showBound_Effect()
{
    OBJ effect;

    if ( PLAYER01.Bound_Jumping )
    {
        int count = 60 - ( PLAYER01.jumps.y / -48.0f ) * 60.0f;
        count = count < 0 || count > 59 ? 59 : count;
        float height1 = ( float ) count / 60.0f * 250;
        int RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( PLAYER01.offset.x - PLAYER01.map.x ) );
        int RunY = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( PLAYER01.offset.y + abs( PLAYER01.jump_test.y ) - ( 280 + height1 ) - PLAYER01.map.y ) );
        effect = OBJ( RunX, RunY, count / 2, 1166, -1 );
        //rule->hit_position.w++;
        effect.showFlag = abs( effect.x ) < BUFFER_WIDTH && abs( effect.y ) < BUFFER_HEIGHT;
        effect.setBuffer( LEFT_BUFFER );
        effect.draw();
        RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( PLAYER01.offset.x - PLAYER02.map.x ) );
        RunY = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( PLAYER01.offset.y + abs( PLAYER01.jump_test.y ) - ( 280 + height1 ) - PLAYER02.map.y ) );
        effect = OBJ( RunX, RunY, count / 2, 1166, -1 );
        //rule->hit_position.w++;
        effect.showFlag = abs( effect.x ) < BUFFER_WIDTH && abs( effect.y ) < BUFFER_HEIGHT;
        effect.setBuffer( RIGHT_BUFFER );
        effect.draw();
    }

    if ( PLAYER02.Bound_Jumping )
    {

        int count = 60 - ( PLAYER02.jumps.y / -48.0f ) * 60.0f;
        count = count < 0 || count > 59 ? 59 : count;
        float height1 = ( float ) count / 60.0f * 250;
        int RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( PLAYER02.offset.x - PLAYER02.map.x ) );
        int RunY = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( PLAYER02.offset.y + abs( PLAYER02.jump_test.y ) - ( 280 + height1 ) - PLAYER02.map.y ) );
        effect = OBJ( RunX, RunY, count / 2, 1166, -1 );
        //rule->hit_position.w++;
        effect.showFlag = abs( effect.x ) < BUFFER_WIDTH && abs( effect.y ) < BUFFER_HEIGHT;
        effect.setBuffer( RIGHT_BUFFER );
        effect.draw();
        RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( PLAYER02.offset.x - PLAYER01.map.x ) );
        RunY = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( PLAYER02.offset.y + abs( PLAYER02.jump_test.y ) - ( 280 + height1 ) - PLAYER01.map.y ) );
        effect = OBJ( RunX, RunY, count / 2, 1166, -1 );
        //rule->hit_position.w++;
        effect.showFlag = abs( effect.x ) < BUFFER_WIDTH && abs( effect.y ) < BUFFER_HEIGHT;
        effect.setBuffer( LEFT_BUFFER );
        effect.draw();

    }
}

void showPlayer( Player* fPlayer, Player* sPlayer, ROLE_RULE * PLAY, SDL_Rect *map, BUFFER_MANAGER b, int *Count, bool *backCount )
{
    //OBJ effect;
    int RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( PLAY->offset.x - map->x ) );
    int RunY = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( PLAY->offset.y - map->y ) );
    int frame_c = ( ( PLAY->Power / -40 ) * 2 ) + 1;
    float speed_s = ( PLAY->jump_test.y > -21 ) ? ( PLAY->Power / -45 ) : 0.6;
    float speed;
    static int runChunk = 0;
    runChunk %= 10;
    fPlayer->LEFT( PLAY->Left ); //設定人物方向
    sPlayer->LEFT( PLAY->Left );
    fPlayer->setRate( speed_s ); //設定動畫播放速度
    sPlayer->setRate( speed_s );
    SDL_Rect shadow = {PLAY->offset.x, PLAY->offset.y, 0, 0};

    if ( PLAY->Height_arrow != 23 )
    {

        shadow.y = ( Class[ PLAY->Height_arrow ].Strat.v /* + ROLE_HEIGHT */ ) - ( /* TOP_HEIGHT - */Players[ PLAY->select ].y ) ;

    }
    else if ( !PLAY->climb && PLAY->offset.y <= Class[ PLAY->Height_arrow + 1 ].Strat.v - 64 )
    {
        shadow.y = Class[ PLAY->Height_arrow + 1 ].Strat.v - 64;
    }

    if ( ( PLAY->offset.y < Class[ PLAY->Height_arrow + 1 ].Strat.v - Players[ PLAY->select ].y && PLAY->Jump_To ) || PLAY->Cam_up )
    {
        shadow.y = ( Class[ PLAY->Height_arrow + 1 ].Strat.v /* + ROLE_HEIGHT */ ) - ( /* TOP_HEIGHT - */Players[ PLAY->select ].y );
    }

    sPlayer->setOffset( &shadow );

    if ( !PLAY->climb && !PLAY->Bound_Jumping && !PLAY->HIT && PLAY->jump_test.y == 0 && PLAY->offset.y >= Class[ 24 ].Strat.v && PLAY->Height_arrow != 24 )
    {
        if ( ( ( PLAY->Power / -45 ) * 24 ) > 1 )
        {
            if ( PLAY->Count % 15 == 0 )
                Play_Chunk( "walk.wav", ++runChunk % 10 );

            fPlayer->Update( "Run" );

            //sPlayer->Update("Run");
        }
        else
        {
            fPlayer->Update( "Stand" );
            //sPlayer->Update("Stand");
        }
    }

    /*if(PLAY->Cam_up)
     fPlayer->Status=STAND;*/
    if ( PLAY->HIT )
    {
        if ( PLAY->jumps.y < 0 )
        {

            PLAY->Impel = 26;
            fPlayer->Update( "Hit_up" );
            //sPlayer->Update("Hit_up");
        }
        else if ( PLAY->jumps.y > 0 )
        {
            fPlayer->Update( "Hit_down" );
            //sPlayer->Update("Hit_down");
            PLAY->Impel = 16;
        }
        else if ( PLAY->jumps.y == 0 )
        {
            PLAY->Impel = 3;

            *backCount = true;
        }

        if ( *backCount )
        {

            if ( PLAY->Left )
                abc.SetCenter( 0, RunX + 26, RunY + 238 );
            else
                abc.SetCenter( 0, RunX + 216, RunY + 238 );

            abc.SetLeft( PLAY->Left );

            if ( *Count > 20 )
            {
                fPlayer->Update( "Hit" );
                //sPlayer->Update("Hit");
                PLAY->Impel = 5;

            }
            else if ( *Count > 0 && *Count <= 20 )
            {
                PLAY->Impel = 0;
                fPlayer->Update( "Climb" );
                //sPlayer->Update("Climb");
            }
            else if ( *Count == 0 )
            {
                *backCount &= 0;
                PLAY->Jump_To = 0;
                PLAY->Jumping = 0 ;
                PLAY->KICK &= 0;
                PLAY->HIT = 0;
                PLAY->Power = -6;
                PLAY->Left = Class[ PLAY->Height_arrow ].LEFT;
                fPlayer->Update( "Stand" );
                //sPlayer->Update("Stand");
            }
        }
        else
        {
            *Count = 50;
        }

    }
    else if ( PLAY->Bound_Jumping )
    {
        if ( PLAY->jumps.y < -10 )
        {


            fPlayer->Update( "Jumping" );
            //sPlayer->Update("Jumping");
        }
        else
        {
            fPlayer->Update( "Jump_down" );
            //sPlayer->Update("Jump_down");
            PLAY->Left = Class[ PLAY->Height_arrow ].LEFT;
        }
    }
    else if ( PLAY->Height_arrow == 23 )
    {
        if ( PLAY->climb )
        {
            if ( PLAY->jumps.y < 0 )
            {

                fPlayer->Update( "Climbing1" );
                //sPlayer->Update("Climbing1");
            }
            else if ( PLAY->jumps.y > 0 )
            {

                fPlayer->Update( "Climbing3" );
                //sPlayer->Update("Climbing1");
            }
            else if ( PLAY->jumps.y == 0 )
            {
                fPlayer->setRate( speed_s * 2 );

                if ( speed_s > 0.1f )
                {
                    fPlayer->Update( "Climbing2" );
                    //sPlayer->Update("Climbing2");
                }
            }
        }
        else if ( PLAY->offset.y <= Class[ PLAY->Height_arrow + 1 ].Strat.v && !PLAY->Cam_up )
        {
            fPlayer->Update( "Climbed" );
            //sPlayer->Update("Climbed");
        }
    }
    else if ( PLAY->Height_arrow == 24 )
    {
        fPlayer->Update( "Climbed2" );
        //sPlayer->Update("Climbed2");
    }
    else
    {
        if ( PLAY->select == 1 )
        {
            if ( PLAY->jumps.y != 0 )
            {
                fPlayer->Update( "Jump_up" );
                //sPlayer->Update("Jump_up");

                if ( PLAY->jumps.y > 0 )
                    PLAY->Left = Class[ PLAY->Height_arrow ].LEFT;
            }
        }
        else
        {
            if ( PLAY->jumps.y < 0 )
            {
                fPlayer->Update( "Jump_up" );
                //sPlayer->Update("Jump_up");
            }
            else if ( PLAY->jumps.y > 2 && !PLAY->Cam_up )
            {
                fPlayer->Update( "Jump_down" );
                //sPlayer->Update("Jump_down");
                PLAY->Left = Class[ PLAY->Height_arrow ].LEFT;
            }
        }
    }

    if ( PLAY->KICK )
        PLAY->Impel /= 2;

    float alpha = ( float ) ( shadow.y - PLAY->offset.y ) / 200.0f * 155.0f;

    alpha = alpha + 120 > 255 ? 255 : alpha + 120;

    sPlayer->ALPHA( alpha );

    sPlayer->setFrame( fPlayer->getFrame() );

    sPlayer->setMap( PLAYER01.map );

    sPlayer->draw( LEFT_BUFFER );

    sPlayer->setMap( PLAYER02.map );

    sPlayer->draw( RIGHT_BUFFER );

    fPlayer->setOffset( &PLAY->offset );

    fPlayer->setMap( PLAYER01.map );

    fPlayer->draw( LEFT_BUFFER );

    fPlayer->setMap( PLAYER02.map );

    fPlayer->draw( RIGHT_BUFFER );


    if ( PLAY->HIT && *Count < 50 )
    {
        speed = ( 1.5 - ( ( ( float ) ( *Count ) ) / 50.0 ) );
        abc.SetBuffer( b );
        abc.Start( 0, speed );
    }
}

//void showPlayer(Player* fPlayer,ROLE_ROLE * PLAY)
//{
//    static bool backCount = false;
//    static int Count = 0;
//    int RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( SPRITE.offset.x - x_t ) );
//    int RunY = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( SPRITE.offset.y - y_1 ) );
//    int frame_c = ( ( SPRITE.Mala.gpoint.y / -40 ) * 2 ) + 1;
//    float speed;
//
//
//
//    /*if(cam_up)
//     Draw_Player01.Status=STAND;*/
//    if ( SPRITE.HIT )
//    {
//        if ( SPRITE.jumps.y < 0 )
//        {
//            SPRITE.Impel = 26;
//            Draw_Player01.Update("Hit_up");
//        }
//        else if ( SPRITE.jumps.y > 0 )
//        {
//            Draw_Player01.Update("Hit_down");
//            SPRITE.Impel = 16;
//        }
//        else if ( SPRITE.jumps.y == 0 )
//        {
//            SPRITE.Impel = 3;
//
//            backCount = true;
//
//        }
//
//        if ( backCount )
//        {
//
//            if ( SPRITE.Left )
//                abc.SetCenter( 0, RunX + 26, RunY + 238 );
//            else
//                abc.SetCenter( 0, RunX + 216, RunY + 238 );
//
//            abc.SetLeft( SPRITE.Left );
//
//            --Count;
//
//            if ( Count > 20 )
//            {
//                Draw_Player01.Update("Hit");
//                SPRITE.Impel = 5;
//
//            }
//            else if ( Count > 0 && Count <= 20 )
//            {
//                SPRITE.Impel = 0;
//                Draw_Player01.Update("Climb");
//
//            }
//            else if ( Count == 0 )
//            {
//                backCount &= 0;
//                SPRITE.HIT = 0;
//                Draw_Player01.Update("Stand");
//            }
//        }
//        else
//        {
//            Count = 50;
//        }
//
//    }else if(SPRITE.Bound_Jumping)
// {
//  Draw_Player01.Update("Jumping");
// }else
// {
//  if ( SPRITE.jumps.y < 0 )
//   Draw_Player01.Update("Jump_up");
//  else if ( SPRITE.jumps.y > 2 &&!cam_up){
//   Draw_Player01.Update("Jump_down");
//  }
// }
// if(SPRITE.Jumping){
//  if(SPRITE.offset.x+Players[ selected01 ].x>Class[ height_arrow+1 ].End.x)
//   SPRITE.offset.x-=(SPRITE.offset.x+Players[ selected01 ].x-Class[ height_arrow+1 ].End.x)/30;
//  if(SPRITE.offset.x+Players[ selected01 ].x<Class[ height_arrow+1 ].Strat.x)
//   SPRITE.offset.x+=(Class[ height_arrow+1 ].Strat.x-SPRITE.offset.x+Players[ selected01 ].x)/30;
// }
//    //Draw_Player01.Update();
//    Draw_Player01.setOffset( &SPRITE.offset );
//    Draw_Player01.setMap( PLAYER01.map );
//    Draw_Player01.draw( LEFT_BUFFER );
//
//    if ( SPRITE.HIT && Count < 50 )
//    {
//        speed = ( 1.5 - ( ( ( float ) ( Count ) ) / 50.0 ) );
//        abc.SetBuffer( LEFT_BUFFER );
//
//  abc.Start( 0, speed );
//    }
//    Draw_Player01.draw( RIGHT_BUFFER );
// if ( SPRITE.HIT && Count < 50 )
//    {
//        speed = ( 1.5 - ( ( ( float ) ( Count ) ) / 50.0 ) );
//
//        abc.SetBuffer( RIGHT_BUFFER );
//  abc.Start( 0, speed );
//    }
//}
void showRun()
{
    static bool backCount = false, backCount02 = false;
    static int Count = 0, Count02 = 0;

    if ( backCount )
        --Count;

    if ( backCount02 )
        --Count02;

    showPlayer( &Draw_Player01, &Draw_Player01_Shadow, &PLAYER01, &PLAYER01.map, LEFT_BUFFER, &Count, &backCount );

    showPlayer( &Draw_Player02, &Draw_Player02_Shadow, &PLAYER02, &PLAYER02.map, RIGHT_BUFFER, &Count02, &backCount02 );

    showBound_Effect();
}

void Drawbackground( ROLE_RULE *PLAY, BUFFER_MANAGER b, int count , SDL_Rect *map )
{
    //static int count=0;
    int X , Y;
    OBJ draw;
    /*  Waterfall 01 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 874 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 5703 - map->y ) );
    draw = OBJ( X, Y, count, 838, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && ( ( draw.y < 0 && abs( draw.y ) < SpriteData[ 838 ] ->h ) || ( draw.y > 0 && draw.y < BUFFER_HEIGHT ) );
    draw.setBuffer( b );
    draw.draw();
    /*  Waterfall 02 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 884 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4951 - map->y ) );
    draw = OBJ( X, Y, count, 808, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
    draw.setBuffer( b );
    draw.draw();
    /*  Waterfall 03 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 984 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4595 - map->y ) );
    draw = OBJ( X, Y, count, 778, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
    draw.setBuffer( b );
    draw.draw();
    /*  Rainbow 01 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 648 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 5905 - map->y ) );
    draw = OBJ( X, Y, 1, 873, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
    draw.setBuffer( b );
    draw.draw();
    /*  Placard Jump 01 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 32 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 3996 - map->y ) );
    draw = OBJ( X, Y, 0, 2020, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
    draw.setBuffer( b );
    draw.draw();
    /*  Placard Jump 02 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 1756 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4766 - map->y ) );
    draw = OBJ( X, Y, 0, 2020, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
    draw.setBuffer( b );
    draw.draw();
}

void DrawForeground( ROLE_RULE *PLAY, BUFFER_MANAGER b, int count , SDL_Rect *map )
{
    static int inc = 1, fade = 0, countn = 6;
    static bool fade_on = 0;
    SDL_Rect b1 = {PLAY->offset.x + Players[ PLAY->select ].x, PLAY->offset.y + Players[ PLAY->select ].y, 0, 0};
    SDL_Rect b2 = {SPRITE.Pigs2.x + Players[ PLAY->select ].x, SPRITE.Pigs2.y + Players[ PLAY->select ].y, 0, 0};
    SDL_Rect b3 = {SPRITE.Pigs.x + Players[ PLAY->select ].x, SPRITE.Pigs.y + Players[ PLAY->select ].y, 0, 0};
    SDL_Rect b4 = {SPRITE.Turtle3.x + Animals[ 2 ].x, SPRITE.Turtle3.y + Animals[ 2 ].y, 0, 0};
    inc = ( fade >= 100 && inc > 0 ) || ( fade <= 1 && inc < 0 ) ? -inc : inc;

    if ( fade_on )
    {
        fade += ( inc * countn );

        if ( ( fade >= 100 && inc > 0 ) || ( fade <= 1 && inc < 0 ) )
            countn -= 2;

        if ( count < 1 )
        {
            fade_on = 0;
            countn = 6;
        }
    }

    //static int count=0;
    int X , Y;

    OBJ draw;

    /*  Bridge 01 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 1040 - map->x ) );

    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4508 - map->y ) );

    draw = OBJ( X, Y, 0, 869, -1 );

    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;

    draw.setBuffer( b );

    draw.draw();

    /*  Bridge 02 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 958 - map->x ) );

    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4858 - map->y ) );

    draw = OBJ( X, Y, 1, 869, -1 );

    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;

    draw.setBuffer( b );

    draw.draw();

    for ( int i = 0;i < 16;i++ )
    {
        int height_fade = i < 8 ? i % 8 : 8 - i % 8;

        SDL_Rect a1 = {992 + i * 30 + 30 / 2, 4951, 0, 0};

        if ( hitTest( &a1, &b1, 48, 32 ) )
        {
            fade_on = true;
            height_fade += 3;
        }

        X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 992 + i * 30 - map->x ) );
        Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4951 + ( ( float ) height_fade * ( 1.0f + ( ( float ) ( fade ) / 100.0f ) ) ) - map->y ) );
        draw = OBJ( X, Y, 0, 1124 + ( i % 4 ), -1 );
        draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
        draw.setBuffer( b );
        draw.draw();
    }

    /*  Bridge 03 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 858 - map->x ) );

    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 5603 - map->y ) );

    draw = OBJ( X, Y, 2, 869, -1 );

    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;

    draw.setBuffer( b );

    draw.draw();

    for ( int i = 0;i < 16;i++ )
    {
        int height_fade = i < 8 ? i % 8 : 8 - i % 8;

        SDL_Rect a1 = {896 + i * 33 + 33 / 2, 5706, 0, 0};

        if ( hitTest( &a1, &b1, 48, 32 ) || hitTest( &a1, &b3, 48, 32 ) || hitTest( &a1, &b4, 48, 32 ) )
        {
            fade_on = true;
            height_fade += 3;
        }

        X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 896 + i * 33 - map->x ) );
        Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 5706 + ( ( float ) height_fade * ( 1.0f + ( ( float ) ( fade ) / 100.0f ) ) ) - map->y ) );
        draw = OBJ( X, Y, 0, 1128 + ( i % 4 ), -1 );
        draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
        draw.setBuffer( b );
        draw.draw();
    }

    /*  Bridge 04 */
    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 715 - map->x ) );

    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 6315 - map->y ) );

    draw = OBJ( X, Y, 3, 869, -1 );

    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;

    draw.setBuffer( b );

    draw.draw();



    for ( int i = 0;i < 20;i++ )
    {
        int height_fade = i < 10 ? i % 10 : 10 - i % 10;

        SDL_Rect a1 = {747 + i * 37 + 37 / 2, 6420, 0, 0};

        if ( hitTest( &a1, &b1, 48, 32 ) || hitTest( &a1, &b2, 48, 32 ) )
        {
            fade_on = true;
            height_fade += 3;
        }

        X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 747 + i * 37 - map->x ) );
        Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 6420 + ( ( float ) height_fade * ( 1.0f + ( ( float ) ( fade ) / 100.0f ) ) ) - map->y ) );
        draw = OBJ( X, Y, 0, 1132 + ( i % 4 ), -1 );
        draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
        draw.setBuffer( b );
        draw.draw();
    }

    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 304 - map->x ) );
    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 3027 - map->y ) );
    draw = OBJ( X, Y, 0, 1408, -1 );
    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && ( ( draw.y < 0 && abs( draw.y ) < SpriteData[ 1408 ] ->h ) || ( draw.y > 0 && draw.y < BUFFER_HEIGHT ) );
    draw.setBuffer( b );
    draw.draw();

}

//void Drawbackground(ROLE_RULE PLAY, BUFFER_MANAGER b, int count )
//{
//    //static int count=0;
//    int X , Y;
//    OBJ draw;
//    /*  Waterfall 01 */
//    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 874 - x_t ) );
//    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 5704 - PLAYER01.map.y ) );
//    draw = OBJ( X, Y, count, 838, -1 );
//    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && ( ( draw.y < 0 && abs( draw.y ) < SpriteData[ 838 ] ->h ) || ( draw.y > 0 && draw.y < BUFFER_HEIGHT ) );
//    draw.setBuffer( b );
//    draw.draw();
//    /*  Waterfall 02 */
//    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 884 - x_t ) );
//    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4951 - PLAYER01.map.y ) );
//    draw = OBJ( X, Y, count, 808, -1 );
//    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//    draw.setBuffer( b );
//    draw.draw();
//    /*  Waterfall 03 */
//    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 984 - x_t ) );
//    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4595 - PLAYER01.map.y ) );
//    draw = OBJ( X, Y, count, 778, -1 );
//    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//    draw.setBuffer( b );
//    draw.draw();
//    /*  Rainbow 01 */
//    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 648 - x_t ) );
//    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 5905 - PLAYER01.map.y ) );
//    draw = OBJ( X, Y, 1, 873, -1 );
//    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//    draw.setBuffer( b );
//    draw.draw();
//}
//void DrawForeground(ROLE_RULE PLAY, BUFFER_MANAGER b, int count )
//{
// static int inc=1,fade=0,countn=6;
// static bool fade_on=0;
// SDL_Rect b1={SPRITE.offset.x+Players[selected01].x,SPRITE.offset.y+Players[selected01].y-150,0,0};
// SDL_Rect b2={SPRITE.Pigs2.x+Players[selected01].x,SPRITE.Pigs2.y+Players[selected01].y-150,0,0};
// SDL_Rect b3={SPRITE.Pigs.x+Players[selected01].x,SPRITE.Pigs.y+Players[selected01].y-150,0,0};
// inc = ( fade >= 100 && inc > 0 ) || ( fade <= 1 && inc < 0 ) ? -inc : inc;
// if(fade_on){
//  fade+=(inc*countn);
//  if(( fade >= 100 && inc > 0 ) || ( fade <= 1 && inc < 0 ))
//   countn-=2;
//  if(count<1){
//   fade_on=0;
//   countn=6;
//  }
// }
//    //static int count=0;
//    int X , Y;
//    OBJ draw;
//    /*  Bridge 01 */
//    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 1040 - x_t ) );
//    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4508 - PLAYER01.map.y ) );
//    draw = OBJ( X, Y, 0, 869, -1 );
//    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//    draw.setBuffer( b );
//    draw.draw();
//    /*  Bridge 02 */
//    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 958 - x_t ) );
//    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4858 - PLAYER01.map.y ) );
//    draw = OBJ( X, Y, 1, 869, -1 );
//    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//    draw.setBuffer( b );
//    draw.draw();
// for(int i=0;i<16;i++)
// {
//  int height_fade=i<8?i%8:8-i%8;
//
//  SDL_Rect a1={992+i*30+30/2,4951,0,0};
//
//  if(hitTest(&a1,&b1,48,32))
//  {
//   fade_on=true;
//   height_fade+=3;
//  }
//
//  X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 992+i*30 - x_t ) );
//  Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 4951+((float)height_fade*(1.0f+((float)(fade)/100.0f))) - PLAYER01.map.y ) );
//  draw = OBJ( X, Y, 0, 1124+(i%4), -1 );
//  draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//  draw.setBuffer( b );
//  draw.draw();
// }
//    /*  Bridge 03 */
//    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 858 - x_t ) );
//    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 5603 - PLAYER01.map.y ) );
//    draw = OBJ( X, Y, 2, 869, -1 );
//    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//    draw.setBuffer( b );
//    draw.draw();
// for(int i=0;i<16;i++)
// {
//  int height_fade=i<8?i%8:8-i%8;
//
//  SDL_Rect a1={896+i*33+33/2,5706,0,0};
//
//  if(hitTest(&a1,&b1,48,32)||hitTest(&a1,&b3,48,32))
//  {
//   fade_on=true;
//   height_fade+=3;
//  }
//
//  X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 896+i*33 - x_t ) );
//  Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 5706+((float)height_fade*(1.0f+((float)(fade)/100.0f))) - PLAYER01.map.y ) );
//  draw = OBJ( X, Y, 0, 1128+(i%4), -1 );
//  draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//  draw.setBuffer( b );
//  draw.draw();
// }
//    /*  Bridge 04 */
//    X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 715 - x_t ) );
//    Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 6315 - PLAYER01.map.y ) );
//    draw = OBJ( X, Y, 3, 869, -1 );
//    draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//    draw.setBuffer( b );
//    draw.draw();
//
//
//
// for(int i=0;i<20;i++)
// {
//  int height_fade=i<10?i%10:10-i%10;
//
//  SDL_Rect a1={747+i*37+37/2,6420,0,0};
//
//  if(hitTest(&a1,&b1,48,32)||hitTest(&a1,&b2,48,32))
//  {
//   fade_on=true;
//   height_fade+=3;
//  }
//
//  X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( 747+i*37 - x_t ) );
//  Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( 6420+((float)height_fade*(1.0f+((float)(fade)/100.0f))) - PLAYER01.map.y ) );
//  draw = OBJ( X, Y, 0, 1132+(i%4), -1 );
//  draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
//  draw.setBuffer( b );
//  draw.draw();
// }
//
//}
void showDrawbackground()
{
    static int count = 0;
    Drawbackground( &PLAYER01, LEFT_BUFFER, count , &Screen.getCenter() );
    Drawbackground( &PLAYER02, RIGHT_BUFFER, count , &Screen02.getCenter() );
    ( ++count ) %= 30;
}

void showForeground()
{
    static int count = 0;
    DrawForeground( &PLAYER01, LEFT_BUFFER, count , &Screen.getCenter() );
    DrawForeground( &PLAYER02, RIGHT_BUFFER, count , &Screen02.getCenter() );
    ( ++count ) %= 30;
}

void Running_Cam( ROLE_RULE * PLAY, int speed )
{
    int height = MAP_HEIGHT - TOP_HEIGHT;
    int RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( PLAY->offset.x - PLAY->map.x ) );
    int map_mov = speed; //(PLAY->Left&&RunX>=640-250)||(!PLAY->Left&&RunX<=0)?192:0;

    if ( PLAY->Jumping && PLAY->Jump_To && ( PLAY->Height_arrow != 14 ) )
    {

        if ( PLAY->offset.x + Players[ PLAY->select ].x > Class[ PLAY->Height_arrow + 1 ].End.x + 32 )
        {
            PLAY->Left = 1;
            speed = ( PLAY->offset.x + Players[ PLAY->select ].x - ( Class[ PLAY->Height_arrow + 1 ].End.x + 32 ) ) / 30;
            //PLAY->offset.x-=(PLAY->offset.x+Players[ PLAY->select ].x-Class[ PLAY->Height_arrow+1 ].End.x)/30;
        }

        if ( PLAY->offset.x + Players[ PLAY->select ].x < Class[ PLAY->Height_arrow + 1 ].Strat.x - 32 )
        {
            PLAY->Left = 0;
            speed = ( Class[ PLAY->Height_arrow + 1 ].Strat.x - ( PLAY->offset.x + Players[ PLAY->select ].x - 32 ) ) / 30;
            //PLAY->offset.x+=(Class[ PLAY->Height_arrow+1 ].Strat.x-PLAY->offset.x+Players[ PLAY->select ].x)/30;
        }
    }

    /*if(!PLAY->Jump_To){

     else
    }*/

    if ( PLAY->Height_arrow != 23 )
    {
        if ( PLAY->Left == 1 )
        {
            /*if(PLAY->offset.x<Class[PLAY->Height_arrow].End.x-Players[PLAY->select].x)
             PLAY->offset.x=Class[PLAY->Height_arrow].End.x-Players[PLAY->select].x;*/ 
            //PLAY->Speed=speed>0.16f?(int)((float)PLAY->Speed-(speed*3)*16)%96:PLAY->Speed;

            if ( RunX > 256 )
            {
                //PLAY->player.x = PLAY->player.x - speed;

                if ( PLAY->HIT || !PLAY->Jumping )
                    PLAY->offset.x = PLAY->offset.x - speed > Class[ PLAY->Height_arrow ].Strat.x - Players[ PLAY->select ].x ? PLAY->offset.x - speed : Class[ PLAY->Height_arrow ].Strat.x - Players[ PLAY->select ].x;
                else
                    PLAY->offset.x = PLAY->offset.x - speed > Class[ PLAY->Height_arrow + 1 ].Strat.x - Players[ PLAY->select ].x ? PLAY->offset.x - speed : Class[ PLAY->Height_arrow + 1 ].Strat.x - Players[ PLAY->select ].x;
            }
            else
            {
                PLAY->map.x = PLAY->map.x > 0 ? PLAY->map.x - map_mov : 0;

                if ( PLAY->HIT || !PLAY->Jumping )
                    PLAY->offset.x = PLAY->offset.x - speed > Class[ PLAY->Height_arrow ].Strat.x - Players[ PLAY->select ].x ? PLAY->offset.x - speed : Class[ PLAY->Height_arrow ].Strat.x - Players[ PLAY->select ].x;
                else
                    PLAY->offset.x = PLAY->offset.x - speed > Class[ PLAY->Height_arrow + 1 ].Strat.x - Players[ PLAY->select ].x ? PLAY->offset.x - speed : Class[ PLAY->Height_arrow + 1 ].Strat.x - Players[ PLAY->select ].x;

                if ( PLAY->map.x == 0 )
                {

                    if ( RunX == 0 )
                    {
                        PLAY->Left = 1;
                        PLAY->offset.x = Class[ PLAY->Height_arrow ].Strat.x - Players[ PLAY->select ].x;
                    }
                }

            }

            if ( !PLAY->HIT )
                if ( Class[ PLAY->Height_arrow ].BACK && ( !PLAY->Jumping ) && PLAY->offset.x - speed < Class[ PLAY->Height_arrow ].Strat.x - Players[ PLAY->select ].x )
                    PLAY->Left ^= 1;
        }

        if ( PLAY->Left == 0 )
        {
            /*if(PLAY->offset.x>Class[PLAY->Height_arrow].Strat.x-Players[PLAY->select].x)
             PLAY->offset.x=Class[PLAY->Height_arrow].Strat.x-Players[PLAY->select].x;*/

            //PLAY->Speed=speed>0.16f?(int)((float)PLAY->Speed+(speed*3)*16)%96:PLAY->Speed;

            if ( RunX < 256 )
            {
                //PLAY->player.x = PLAY->player.x + speed;

                if ( PLAY->HIT || !PLAY->Jumping )
                    PLAY->offset.x = PLAY->offset.x + speed < Class[ PLAY->Height_arrow ].End.x - Players[ PLAY->select ].x ? PLAY->offset.x + speed : Class[ PLAY->Height_arrow ].End.x - Players[ PLAY->select ].x;
                else
                    PLAY->offset.x = PLAY->offset.x + speed < Class[ PLAY->Height_arrow + 1 ].End.x - Players[ PLAY->select ].x ? PLAY->offset.x + speed : Class[ PLAY->Height_arrow + 1 ].End.x - Players[ PLAY->select ].x;
            }
            else
            {
                PLAY->map.x = PLAY->map.x < MAP_WIDTH - SCREEN_WIDTH / 2 ? PLAY->map.x + map_mov : MAP_WIDTH - SCREEN_WIDTH / 2;

                if ( PLAY->HIT || !PLAY->Jumping )
                    PLAY->offset.x = PLAY->offset.x + speed < Class[ PLAY->Height_arrow ].End.x - Players[ PLAY->select ].x ? PLAY->offset.x + speed : Class[ PLAY->Height_arrow ].End.x - Players[ PLAY->select ].x;
                else
                    PLAY->offset.x = PLAY->offset.x + speed < Class[ PLAY->Height_arrow + 1 ].End.x - Players[ PLAY->select ].x ? PLAY->offset.x + speed : Class[ PLAY->Height_arrow + 1 ].End.x - Players[ PLAY->select ].x;

                if ( PLAY->map.x == MAP_WIDTH - SCREEN_WIDTH / 2 )
                {

                    if ( RunX == BUFFER_WIDTH - 256 )
                    {
                        PLAY->Left = 0;
                        PLAY->offset.x = Class[ PLAY->Height_arrow ].End.x - Players[ PLAY->select ].x;
                    }
                }
            }

            if ( !PLAY->HIT )
                if ( Class[ PLAY->Height_arrow ].BACK && ( !PLAY->Jumping ) && PLAY->offset.x + speed > Class[ PLAY->Height_arrow ].End.x - Players[ PLAY->select ].x )
                    PLAY->Left ^= 1;
        }
    }
    else
    {
        if ( PLAY->climb )
        {
            int move;
            int climb = speed < 12 ? 0 : PLAY->offset.y % 26;
            speed /= 4;

            if ( PLAY->offset.y - speed > Class[ PLAY->Height_arrow + 1 ].Strat.v - 64 )
            {
                if ( PLAY->jumps.y < 18 && PLAY->jumps.y != 0 )
                {
                    PLAY->offset.y += PLAY->jumps.y / 2;

                    if ( PLAY->select == selected01 )
                    {
                        move = abs( PLAY->offset.x - Class[ PLAY->Height_arrow ].Strat.x - 128 ) / 10;
                        PLAY->offset.x = PLAY->offset.x > Class[ PLAY->Height_arrow ].Strat.x - 128 ? PLAY->offset.x - move : Class[ PLAY->Height_arrow ].Strat.x - 128; //(PLAY->offset.x - Class[ PLAY->Height_arrow ].Strat.x)>0 ? PLAY->offset.x-abs(PLAY->offset.x-Class[ PLAY->Height_arrow ].Strat.x)/3: PLAY->offset.x;
                        PLAY->map.x = PLAY->map.x - move > 420 ? PLAY->map.x - move : 420;
                    }

                    if ( PLAY->select == selected02 )
                    {
                        move = abs( PLAY->offset.x - ( Class[ PLAY->Height_arrow ].End.x - 128 ) ) / 10;
                        PLAY->offset.x = PLAY->offset.x < Class[ PLAY->Height_arrow ].End.x - 128 ? PLAY->offset.x + move : Class[ PLAY->Height_arrow ].End.x - 128; //(PLAY->offset.x - Class[ PLAY->Height_arrow ].Strat.x)>0 ? PLAY->offset.x-abs(PLAY->offset.x-Class[ PLAY->Height_arrow ].Strat.x)/3: PLAY->offset.x;
                        PLAY->map.x = PLAY->map.x + move < 740 ? PLAY->map.x + move : 740;
                    }
                }
                else
                {
                    if ( PLAY->select == selected01 )
                        PLAY->offset.x = Class[ PLAY->Height_arrow ].Strat.x - 128 + climb;

                    if ( PLAY->select == selected02 )
                        PLAY->offset.x = Class[ PLAY->Height_arrow ].End.x - 128 + climb;
                }

                PLAY->map.y -= speed;
                PLAY->offset.y -= speed;
            }
            else
                PLAY->offset.y = Class[ PLAY->Height_arrow + 1 ].Strat.v - 64;
        }
    }
}

//void Running_Cam(ROLE_RULE * PLAY, int speed)
//{
//    int height = MAP_HEIGHT - TOP_HEIGHT;
//    int RunX = BUFFER_WIDTH - ( BUFFER_WIDTH - ( SPRITE.offset.x - x_t ) );
//    int map_mov = speed; //(SPRITE.Left&&RunX>=640-250)||(!SPRITE.Left&&RunX<=0)?192:0;
//    /*if(!SPRITE.Jump_To){
//
//     else
//    }*/
//
//
//    if ( SPRITE.Left == 1 )
//    {
//        /*if(SPRITE.offset.x<Class[height_arrow].End.x-Players[selected01].x)
//         SPRITE.offset.x=Class[height_arrow].End.x-Players[selected01].x;*/
//        //SPRITE.Speed=speed>0.16f?(int)((float)SPRITE.Speed-(speed*3)*16)%96:SPRITE.Speed;
//
//        if ( RunX > 256 )
//        {
//            //SPRITE.player.x = SPRITE.player.x - speed;
//
//            if ( !SPRITE.Jumping )
//                SPRITE.offset.x = SPRITE.offset.x - speed > Class[ height_arrow ].Strat.x - Players[ selected01 ].x ? SPRITE.offset.x - speed : Class[ height_arrow ].Strat.x - Players[ selected01 ].x;
//            else
//                SPRITE.offset.x = SPRITE.offset.x - speed > Class[ height_arrow + 1 ].Strat.x - Players[ selected01 ].x ? SPRITE.offset.x - speed : Class[ height_arrow + 1 ].Strat.x - Players[ selected01 ].x;
//        }
//        else
//        {
//            x_t = x_t > 0 ? x_t - map_mov : 0;
//
//            if ( !SPRITE.Jumping )
//                SPRITE.offset.x = SPRITE.offset.x - speed > Class[ height_arrow ].Strat.x - Players[ selected01 ].x ? SPRITE.offset.x - speed : Class[ height_arrow ].Strat.x - Players[ selected01 ].x;
//            else
//                SPRITE.offset.x = SPRITE.offset.x - speed > Class[ height_arrow + 1 ].Strat.x - Players[ selected01 ].x ? SPRITE.offset.x - speed : Class[ height_arrow + 1 ].Strat.x - Players[ selected01 ].x;
//
//            if ( x_t == 0 )
//            {
//                SPRITE.player.x = SPRITE.player.x > 0 ? SPRITE.player.x - speed : 0;
//
//                if ( RunX == 0 )
//                {
//                    SPRITE.Left = 1;
//                    SPRITE.offset.x = Class[ height_arrow ].Strat.x - Players[ selected01 ].x;
//                }
//            }
//
//        }
//  if ( !SPRITE.HIT )
//   if ( Class[ height_arrow ].BACK && ( !SPRITE.Jumping ) && SPRITE.offset.x - speed < Class[ height_arrow ].Strat.x - Players[ selected01 ].x )
//    SPRITE.Left ^= 1;
//    }
//
//    if ( SPRITE.Left == 0 )
//    {
//        /*if(SPRITE.offset.x>Class[height_arrow].Strat.x-Players[selected01].x)
//         SPRITE.offset.x=Class[height_arrow].Strat.x-Players[selected01].x;*/
//
//        //SPRITE.Speed=speed>0.16f?(int)((float)SPRITE.Speed+(speed*3)*16)%96:SPRITE.Speed;
//
//        if ( RunX < 256 )
//        {
//            //SPRITE.player.x = SPRITE.player.x + speed;
//
//            if ( !SPRITE.Jumping )
//                SPRITE.offset.x = SPRITE.offset.x + speed < Class[ height_arrow ].End.x - Players[ selected01 ].x ? SPRITE.offset.x + speed : Class[ height_arrow ].End.x - Players[ selected01 ].x;
//            else
//                SPRITE.offset.x = SPRITE.offset.x + speed < Class[ height_arrow + 1 ].End.x - Players[ selected01 ].x ? SPRITE.offset.x + speed : Class[ height_arrow + 1 ].End.x - Players[ selected01 ].x;
//        }
//        else
//        {
//            x_t = x_t < MAP_WIDTH - SCREEN_WIDTH / 2 ? x_t + map_mov : MAP_WIDTH - SCREEN_WIDTH / 2;
//
//            if ( !SPRITE.Jumping )
//                SPRITE.offset.x = SPRITE.offset.x + speed < Class[ height_arrow ].End.x - Players[ selected01 ].x ? SPRITE.offset.x + speed : Class[ height_arrow ].End.x - Players[ selected01 ].x;
//            else
//                SPRITE.offset.x = SPRITE.offset.x + speed < Class[ height_arrow + 1 ].End.x - Players[ selected01 ].x ? SPRITE.offset.x + speed : Class[ height_arrow + 1 ].End.x - Players[ selected01 ].x;
//
//            if ( x_t == MAP_WIDTH - SCREEN_WIDTH / 2 )
//            {
//                SPRITE.player.x = SPRITE.player.x < SCREEN_WIDTH / 2 - 256 ? SPRITE.player.x + speed : SCREEN_WIDTH / 2 - 256;
//
//                if ( RunX == BUFFER_WIDTH - 256 )
//                {
//                    SPRITE.Left = 0;
//                    SPRITE.offset.x = Class[ height_arrow ].End.x - Players[ selected01 ].x;
//                }
//            }
//        }
//  if ( !SPRITE.HIT )
//   if ( Class[ height_arrow ].BACK && ( !SPRITE.Jumping ) && SPRITE.offset.x + speed > Class[ height_arrow ].End.x - Players[ selected01 ].x )
//    SPRITE.Left ^= 1;
//    }
//
//
//}
void motion( ROLE_RULE * PLAY, Player *Draw_Player )
{

    //int CAM_HEIGHT = Class[PLAY->Height_arrow].Strat.v - Class[PLAY->Height_arrow+1].Strat.v
    int frame_c = ( ( PLAY->Power / -45 ) * 15 );

    int speed = frame_c; // ( PLAY->jump_test.y > -21 ) ? frame_c : 12;
    float speed_s = ( PLAY->jump_test.y > -21 ) ? ( PLAY->Power / -45 ) : 0.4;

    int height = MAP_HEIGHT - TOP_HEIGHT;

    //   Draw_Player->setRate( speed_s );

    //if(!PLAY->climb&&!PLAY->Bound_Jumping&&!PLAY->HIT&&PLAY->jump_test.y==0 && PLAY->offset.y>=Class[ 24 ].Strat.v&&PLAY->Height_arrow !=24){
    // if ( speed > 1 )
    //  Draw_Player->Update("Run");
    // else
    //  Draw_Player->Update("Stand");
    //}else
    //{

    //}

    if ( !PLAY->HIT )
    {
        speed = speed > 0 ? speed + 4 : speed;
        Running_Cam( PLAY, speed );
    }
    else
    {
        Running_Cam( PLAY, PLAY->Impel );
    }

    if ( !PLAY->Cam_up && PLAY->Height_arrow != 23 )
    {
        //if(PLAY->Height_arrow==0)
        // PLAY->map.y=7200;
        //else
        PLAY->map.y = Class[ PLAY->Height_arrow ].Strat.v - ( TOP_HEIGHT - SUB_HEIGHT ); //Events[PLAY->Height_arrow].Height - TOP_HEIGHT;
    }
    else if ( PLAY->Cam_up )
    {
        if ( PLAY->Height_arrow == 23 )
            Draw_Player->Update( "Climbed2" );

        float up = ( float ) ( ( PLAY->map.y - ( Class[ PLAY->Height_arrow + 1 ].Strat.v - ( TOP_HEIGHT - SUB_HEIGHT ) ) ) / 5 );

        if ( up > 0.3f )
        {
            PLAY->map.y = PLAY->map.y - up;
            PLAY->Left = Class[ PLAY->Height_arrow + 1 ].LEFT;
        }
        else
        {
            PLAY->Height_arrow += 1;
            PLAY->Cam_up = false;
            PLAY->Jumping = false;

        }
    }
}

void CreatCloud()
{
    static int setY = 0;

    for ( int i = 0;i < CLOUD_NUM;i++ )
    {
        if ( SPRITE.Cloud[ i ].h == 0 )
        {
            //srand(SPRITE.Cloud[i].w*100);
            SPRITE.Cloud[ i ].h = 1;
            SPRITE.Cloud[ i ].x = SCREEN_WIDTH + 400;
            SPRITE.Cloud[ i ].y = Could[ ++setY % 19 ];
            SPRITE.Cloud[ i ].w = 4 + rand() % 8;
            break;
        }
    }
}

void drawCloud( BUFFER_MANAGER b, SDL_Rect *map )
{
    int X , Y;
    OBJ draw;

    for ( int i = 0;i < CLOUD_NUM;i++ )
    {
        if ( SPRITE.Cloud[ i ].h == 1 )
        {
            X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( SPRITE.Cloud[ i ].x - map->x ) );
            Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( SPRITE.Cloud[ i ].y - map->y ) );
            draw = OBJ( X, Y, SPRITE.Cloud[ i ].y % 12, 2021, -1 );
            draw.showFlag = abs( draw.x ) < BUFFER_WIDTH && abs( draw.y ) < BUFFER_HEIGHT;
            draw.setBuffer( b );
            draw.draw();
            SPRITE.Cloud[ i ].x -= SPRITE.Cloud[ i ].w / 2;

            if ( SPRITE.Cloud[ i ].x < -480 )
            {
                SPRITE.Cloud[ i ].h = 0;
            }
        }
        else
        {
            CreatCloud();
        }
    }
}

void showDepth()
{
    drawCloud( LEFT_BUFFER, &Screen.getCenter() );
    drawCloud( RIGHT_BUFFER, &Screen02.getCenter() );
}

void Game_Logic( ROLE_RULE *PLAY, KEY *Key )
{

    int speed = -( ( ( 45.0f - ( float ) ( -PLAY->Power ) ) / -45.0f ) * 6 );

    if ( speed <= 2 )
        speed = 2;

    if ( *Key->a || *Key->b )
    {
    }

    if ( *Key->a )
    {
        if ( PLAY->jump_test.y == 0 )
        {
            PLAY->Power = PLAY->Power > -45 ? PLAY->Power - 1.5f : -45;
            PLAY->y = Players[ PLAY->select ].y + ( ( PLAY->Power / -45 ) * 4 );
            PLAY->Stay = 0;
        }

        *Key->a &= 0;
    }

    if ( *Key->b )
    {
        if ( PLAY->jump_test.y == 0 )
        {
            PLAY->Power = PLAY->Power > -45 ? PLAY->Power - 1.5f : -45;
            PLAY->y = Players[ PLAY->select ].y + ( ( PLAY->Power / -45 ) * 4 );
            PLAY->Stay = 0;
        }


        *Key->b &= 0;
    }

    if ( ++PLAY->Stay > 10 && PLAY->jump_test.y == 0 )
        PLAY->Power = ( PLAY->Power < -1 ) ? PLAY->Power + ( 1.0f ) : PLAY->Power;

    PLAY->Power = ( PLAY->Power < -1 ) ? PLAY->Power + ( 0.05f ) : PLAY->Power;

    if ( key_w )
    {
        PLAY->Height_arrow = PLAY->Height_arrow < 24 ? PLAY->Height_arrow + 1 : 24;

        key_w &= 0;
    }

    if ( key_s )
    {
        PLAY->Height_arrow = PLAY->Height_arrow > 0 ? PLAY->Height_arrow - 1 : 0;

        key_s &= 0;
    }

    if ( key_a )
    {
        PLAY->Left = 1;

        key_a &= 0;
    }

    if ( key_d )
    {
        PLAY->Left = 0;

        key_d &= 0;
    }



    if ( !PLAY->Cam_up )
    {
        if ( *Key->space )
        {
            *Key->space = false;

            if ( PLAY->offset.x < 1024 && PLAY->Height_arrow == 16 )
                return ;

            if ( !PLAY->HIT )
            {
                if ( PLAY->jump_test.y == 0 )
                {
                    PLAY->jumps.y = PLAY->Power > -6 && PLAY->Power < -25 ? -35 : PLAY->Power;

                    if ( PLAY->Height_arrow == 23 )
                    {
                        if ( !PLAY->climb )
                        {
                            PLAY->jumps.y = -30;
                            PLAY->climb = true;
                            //PLAY->offset.y-=64;
                        }
                        else
                            PLAY->jumps.y = 0;

                        if ( PLAY->climb && PLAY->offset.y <= Class[ PLAY->Height_arrow + 1 ].Strat.v - 64 )
                        {
                            PLAY->Jump_To = true;
                            PLAY->Jumping = true;
                            PLAY->jumps.y = -40;
                            PLAY->climb = false;
                        }
                        else
                        {
                        }


                    }
                    else if ( /* PLAY->Jump_To && */PLAY->Height_arrow == 12 )
                    {
                        PLAY->Jumping = true;
                        PLAY->jumps.y = -25;
                        PLAY->Power = PLAY->Power > -20 ? -20 : PLAY->Power;
                    }
                    else if ( /*PLAY->Jump_To &&*/PLAY->Height_arrow == 14 )
                    {
                        PLAY->Jumping = true;
                        PLAY->jumps.y = -25;
                        PLAY->Power = PLAY->Power > -20 ? -20 : PLAY->Power;
                    }
                    else if ( PLAY->Jump_To && PLAY->Height_arrow != 0 )
                    {
                        PLAY->Jumping = true;
                        PLAY->jumps.y = PLAY->jumps.y < -35 ? PLAY->jumps.y : -35 ;
                    }
                    else if ( PLAY->Jump_To && PLAY->Height_arrow == 0 )
                    {
                        PLAY->Jumping = true;
                        PLAY->jumps.y = PLAY->jumps.y < -40 ? PLAY->jumps.y : -40 ;
                    }
                    else
                    {
                        PLAY->Jumping &= 0;
                        PLAY->jumps.y = PLAY->jumps.y > -20 ? -20 : PLAY->jumps.y;
                    }


                    PLAY->y = Players[ PLAY->select ].y;

                    Play_Chunk( "bounce01.wav", 14 );
                    //Play( "bounce01.wav" );
                }
            }
        }
    }
    else
        *Key->space = false;
}

//void Game_Logic()
//{
// static int speed_count = 0;
//    if ( key_z )
//    {
//        if ( PLAYER01.jump_test.y == 0 )
//        {
//            PLAYER01.Power = PLAYER01.Power > -45 ? PLAYER01.Power - 2.0f : -45;
//            PLAYER01.y = Players[ PLAYER01.select ].y + ( ( PLAYER01.Power / -45 ) * 4 );
//            speed_count = 0;
//        }
//
//
//        key_z &= 0;
//    }
//
//    if ( key_x )
//    {
//        if ( PLAYER01.jump_test.y == 0 )
//        {
//            PLAYER01.Power = PLAYER01.Power > -45 ? PLAYER01.Power - 2.0f : -45;
//            PLAYER01.y = Players[ PLAYER01.select ].y + ( ( PLAYER01.Power / -45 ) * 4 );
//            speed_count = 0;
//        }
//
//
//        key_x &= 0;
//    }
//
//    if ( ++speed_count > 30 && PLAYER01.jump_test.y == 0 )
//        PLAYER01.Power = ( PLAYER01.Power < -1 ) ? PLAYER01.Power + ( 1.0f ) : PLAYER01.Power;
//
//    PLAYER01.Power = ( PLAYER01.Power < -1 ) ? PLAYER01.Power + ( 0.05f ) : PLAYER01.Power;
//
//    if ( key_w )
//    {
//        PLAYER01.Height_arrow = PLAYER01.Height_arrow < 24 ? PLAYER01.Height_arrow + 1 : 24;
//
//        key_w &= 0;
//    }
//
//    if ( key_s )
//    {
//        PLAYER01.Height_arrow = PLAYER01.Height_arrow > 0 ? PLAYER01.Height_arrow - 1 : 0;
//
//        key_s &= 0;
//    }
//
//    if ( key_a )
//    {
//        PLAYER01.Left = 1;
//
//        key_a &= 0;
//    }
//
//    if ( key_d )
//    {
//        PLAYER01.Left = 0;
//
//        key_d &= 0;
//    }
//
//
//
//    if ( !PLAYER01.Cam_up )
//    {
//        if ( space )
//        {
//            space = false;
//
//            if ( !PLAYER01.HIT )
//            {
//                if ( PLAYER01.jump_test.y == 0 )
//                {
//                    PLAYER01.jumps.y = PLAYER01.Power > -6 && PLAYER01.Power < -25 ? -35 : PLAYER01.Power;
//
//     if ( PLAYER01.Jump_To && PLAYER01.Height_arrow == 12 )
//     {
//      PLAYER01.Jumping = true;
//                        PLAYER01.jumps.y = -25;
//     }else if (PLAYER01.Height_arrow == 14 )
//     {
//      PLAYER01.Jumping = true;
//                        PLAYER01.jumps.y = -25;
//     }
//                    else if ( PLAYER01.Jump_To && PLAYER01.Height_arrow != 0 )
//                    {
//      PLAYER01.Jumping = true;
//                        PLAYER01.jumps.y = PLAYER01.jumps.y < -35 ? PLAYER01.jumps.y : -35 ;
//                    }
//                    else if ( PLAYER01.Jump_To && PLAYER01.Height_arrow == 0 )
//                    {
//      PLAYER01.Jumping = true;
//                        PLAYER01.jumps.y = PLAYER01.jumps.y < -40 ? PLAYER01.jumps.y : -40 ;
//                    }
//     else{
//      PLAYER01.Jumping &= 0;
//      PLAYER01.jumps.y = PLAYER01.jumps.y > -20 ? -20 : PLAYER01.jumps.y;
//     }
//
//
//                    PLAYER01.y = Players[ PLAYER01.select ].y;
//
//                    Play( "bounce01.wav" );
//                }
//            }
//        }
//    }
//    else
//        space = false;
//}
//void motion(ROLE_RULE * PLAY)
//{
//    static int speed_count = 0;
//    int frame_c = ( ( SPRITE.Mala.gpoint.y / -45 ) * 24 );
//
//    int speed = frame_c; // ( SPRITE.jump_test.y > -21 ) ? frame_c : 12;
//    float speed_s = ( SPRITE.jump_test.y > -21 ) ? ( SPRITE.Mala.gpoint.y / -45 ) : 0.4;
//    int height = MAP_HEIGHT - TOP_HEIGHT;
//    Draw_Player01.setRate( speed_s );
//
// if(!SPRITE.Bound_Jumping&&!SPRITE.HIT){
//  if ( speed > 1 )
//   Draw_Player01.Update("Run");
//  else
//   Draw_Player01.Update("Stand");
// }
//    if ( !SPRITE.HIT )
//    {
//        Running_Cam(PLAY, speed );
//    }
//    else
//    {
//        Running_Cam(PLAY, SPRITE.Impel );
//    }
//
//    if ( !cam_up )
//    {
//        y_1 = Class[ height_arrow ].Strat.v - TOP_HEIGHT;
//    }
//    else
//    {
//
//        float up = ( float ) ( ( y_1 - ( Class[ height_arrow + 1 ].Strat.v - TOP_HEIGHT ) ) / 5 );
//
//        if ( up > 0.3f )
//        {
//            y_1 = y_1 - up;
//            SPRITE.Left = Class[ height_arrow + 1 ].LEFT;
//        }
//        else
//        {
//            height_arrow += 1;
//            cam_up = false;
//            SPRITE.Jumping = false;
//
//        }
//    }
//
//    if ( key_z )
//    {
//        if ( SPRITE.jump_test.y == 0 )
//        {
//            SPRITE.Mala.gpoint.y = SPRITE.Mala.gpoint.y > -45 ? SPRITE.Mala.gpoint.y - 2.0f : -45;
//            SPRITE.player.y = Players[ selected01 ].y + ( ( SPRITE.Mala.gpoint.y / -45 ) * 4 );
//            speed_count = 0;
//        }
//
//
//        key_z &= 0;
//    }
//
//    if ( key_x )
//    {
//        if ( SPRITE.jump_test.y == 0 )
//        {
//            SPRITE.Mala.gpoint.y = SPRITE.Mala.gpoint.y > -45 ? SPRITE.Mala.gpoint.y - 2.0f : -45;
//            SPRITE.player.y = Players[ selected01 ].y + ( ( SPRITE.Mala.gpoint.y / -45 ) * 4 );
//            speed_count = 0;
//        }
//
//
//        key_x &= 0;
//    }
//
//    if ( ++speed_count > 30 && SPRITE.jump_test.y == 0 )
//        SPRITE.Mala.gpoint.y = ( SPRITE.Mala.gpoint.y < -1 ) ? SPRITE.Mala.gpoint.y + ( 1.0f ) : SPRITE.Mala.gpoint.y;
//
//    SPRITE.Mala.gpoint.y = ( SPRITE.Mala.gpoint.y < -1 ) ? SPRITE.Mala.gpoint.y + ( 0.05f ) : SPRITE.Mala.gpoint.y;
//
//    if ( key_w )
//    {
//        height_arrow = height_arrow < 24 ? height_arrow + 1 : 24;
//
//        key_w &= 0;
//    }
//
//    if ( key_s )
//    {
//        height_arrow = height_arrow > 0 ? height_arrow - 1 : 0;
//
//        key_s &= 0;
//    }
//
//    if ( key_a )
//    {
//        SPRITE.Left = 1;
//
//        key_a &= 0;
//    }
//
//    if ( key_d )
//    {
//        SPRITE.Left = 0;
//
//        key_d &= 0;
//    }
//
//
//
//    if ( !cam_up )
//    {
//        if ( space )
//        {
//            space = false;
//
//            if ( !SPRITE.HIT )
//            {
//                if ( SPRITE.jump_test.y == 0 )
//                {
//                    SPRITE.jumps.y = SPRITE.Mala.gpoint.y > -6 && SPRITE.Mala.gpoint.y < -25 ? -35 : SPRITE.Mala.gpoint.y;
//
//     if ( SPRITE.Jump_To && height_arrow == 12 )
//     {
//      SPRITE.Jumping = true;
//                        SPRITE.jumps.y = -25;
//     }else if (height_arrow == 14 )
//     {
//      SPRITE.Jumping = true;
//                        SPRITE.jumps.y = -25;
//     }
//                    else if ( SPRITE.Jump_To && height_arrow != 0 )
//                    {
//      SPRITE.Jumping = true;
//                        SPRITE.jumps.y = SPRITE.jumps.y < -35 ? SPRITE.jumps.y : -35 ;
//                    }
//                    else if ( SPRITE.Jump_To && height_arrow == 0 )
//                    {
//      SPRITE.Jumping = true;
//                        SPRITE.jumps.y = SPRITE.jumps.y < -40 ? SPRITE.jumps.y : -40 ;
//                    }
//     else{
//      SPRITE.Jumping &= 0;
//      SPRITE.jumps.y = SPRITE.jumps.y > -20 ? -20 : SPRITE.jumps.y;
//     }
//
//
//                    SPRITE.player.y = Players[ selected01 ].y;
//
//                    Play( "bounce01.wav" );
//                }
//            }
//        }
//    }
//    else
//        space = false;
//}

void init_Game01()
{
    memset( &SPRITE, 0, sizeof( SPRITE ) );
    memset( &PLAYER01, 0, sizeof( ROLE_RULE ) );
    memset( &PLAYER02, 0, sizeof( ROLE_RULE ) );
    Player_01.Catch();   //讀取阿公圖片位置
    Player_02.Catch();   //讀取阿罵圖片位置
    Player_01_Shadow.Catch(); //讀取阿公影子圖片位置
    Player_02_Shadow.Catch(); //讀取阿罵影子圖片位置
    Smoke.Catch();    //讀取煙霧圖片位置

    //設定按鍵
    P1.a = &key_z;
    P1.b = &key_x;
    P1.space = &space;
    P2.a = &key_c;
    P2.b = &key_v;
    P2.space = &key_b;

    //選擇人物
    selected01 = 0;
    selected02 = 1;

    upa = false;
    downa = false;
    space = false;

    //設定framerate
    SDL_initFramerate( &fps );
    SDL_setFramerate( &fps , 30 );

    map = Multi_Map( 600, 0, 8000, 1920 );  //註冊大地圖
    map.Depth_Map( 228, 875, 3040, 1920 );  //註冊大地圖背景
    map.add_Draw( 0, 0, showDepth );    //註冊雲為第0圖層
    map.add_Draw( 1, 1, showDrawbackground ); //註冊背景為第1圖層
    map.add_Draw( 2, 2, showAnimal );   //註冊動物為第2圖層
    map.add_Draw( 3, 3, showRun );    //註冊玩家為第3圖層
    map.add_Draw( 4, 4, showForeground );  //註冊前景為第4圖層

    //此處深度排序為
    /*    圖層					排序 */
    /*   大地圖背景				   0 */
    /*  map.add_Draw( 0, 0,..)     1 */
    /*    大地圖				   2 */
    /*  map.add_Draw( 1, 1,..)     3 */
    /*  map.add_Draw( 2, 2,..)     4 */
    /*     ↓      */
    /*     ↓      */
    /*  map.add_Draw( n, n,..)     n */

    //設定PLAYER01腳色狀態
    PLAYER01.Left = 1; //左右
    PLAYER01.y = Players[ selected01 ].y; //腳色高度
    PLAYER01.map.x = x_t; //地圖座標X
    PLAYER01.map.y = 7200; //地圖座標Y
    //腳色座標
    PLAYER01.offset.x = 1920 - 256;
    PLAYER01.offset.y = Class[ 0 ].Strat.v + ( PLAYER01.y ) + PLAYER01.jump_test.y;
    PLAYER01.select = selected01;

    PLAYER02 = PLAYER01;
    PLAYER02.select = selected02;

    //載入第一個腳色
    Draw_Player01 = Player( &Players[ PLAYER01.select ], PLAYER01.offset.x, PLAYER01.offset.y, ( int ) * Player_01.Frame_Index, 4 );
    Draw_Player01.setSpeed( 1 );
    Draw_Player01.setMirror( ( int ) * Player_01.Mirror_Index );
    //載入第一個腳色的影子
    Draw_Player01_Shadow = Player( &Players_Shadow[ PLAYER01.select ], PLAYER01.offset.x, PLAYER01.offset.y, ( int ) * Player_01_Shadow.Frame_Index, 4 );
    Draw_Player01_Shadow.setSpeed( 1 );
    Draw_Player01_Shadow.setMirror( ( int ) * Player_01_Shadow.Mirror_Index );
    Draw_Player01_Shadow.setAlpha( ( int ) * Player_01_Shadow.Alpha_Index, ( int ) * Player_01_Shadow.Mirror_Alpha_Index );

    //載入第二個腳色
    Draw_Player02 = Player( &Players[ PLAYER02.select ], PLAYER02.offset.x, PLAYER02.offset.y, ( int ) * Player_02.Frame_Index, 4 );
    Draw_Player02.setSpeed( 1 );
    Draw_Player02.setMirror( ( int ) * Player_02.Mirror_Index );
    //載入第二個腳色的影子
    Draw_Player02_Shadow = Player( &Players_Shadow[ PLAYER02.select ], PLAYER02.offset.x, PLAYER02.offset.y, ( int ) * Player_02_Shadow.Frame_Index, 4 );
    Draw_Player02_Shadow.setSpeed( 1 );
    Draw_Player02_Shadow.setMirror( ( int ) * Player_02_Shadow.Mirror_Index );
    Draw_Player02_Shadow.setAlpha( ( int ) * Player_02_Shadow.Alpha_Index, ( int ) * Player_02_Shadow.Mirror_Alpha_Index );

    //設定山豬座標
    SPRITE.Pigs.w = 0; //方向，0 :右 1 :左
    SPRITE.Pigs.x = -128;
    SPRITE.Pigs.y = Class[ 12 ].Strat.v - Animal_Pig.y;
    SPRITE.Pigs2.w = 0;
    SPRITE.Pigs2.x = -64;
    SPRITE.Pigs2.y = Class[ 6 ].Strat.v - Animal_Pig.y;
    SPRITE.Pigs3.w = 1;
    SPRITE.Pigs3.x = 1400;
    SPRITE.Pigs3.y = Class[ 14 ].Strat.v - Animal_Pig.y;
    SPRITE.Pigs4.w = 0;
    SPRITE.Pigs4.x = 448;
    SPRITE.Pigs4.y = Class[ 16 ].Strat.v - Animal_Pig.y;

    //設定烏龜座標
    SPRITE.Turtle1.w = 0; //方向，0 :右 1 :左
    SPRITE.Turtle1.x = Class[ 0 ].Strat.x + ( Class[ 0 ].End.x - Class[ 0 ].Strat.x ) / 2;
    SPRITE.Turtle1.y = Class[ 0 ].Strat.v - Animal_Turtle.y;
    SPRITE.Turtle2.w = 0;
    SPRITE.Turtle2.x = Class[ 4 ].Strat.x + ( Class[ 4 ].End.x - Class[ 4 ].Strat.x ) / 2;
    SPRITE.Turtle2.y = Class[ 4 ].Strat.v - Animal_Turtle.y;
    SPRITE.Turtle3.w = 1;
    SPRITE.Turtle3.x = Class[ 9 ].Strat.x + ( Class[ 9 ].End.x - Class[ 9 ].Strat.x ) / 2;
    SPRITE.Turtle3.y = Class[ 9 ].Strat.v - Animal_Turtle.y;

    //讀取山豬圖片位置
    Pig.Catch();

    //載入山豬01~04
    Pig01 = Player( &Animals[ 0 ], SPRITE.Pigs.x, SPRITE.Pigs.y, ( int ) * Pig.Frame_Index, 0 );
    Pig01.setSpeed( 1 );
    Pig01.setMirror( ( int ) * Pig.Mirror_Index );
    Pig02 = Player( &Animals[ 0 ], SPRITE.Pigs2.x, SPRITE.Pigs2.y, ( int ) * Pig.Frame_Index, 0 );
    Pig02.setSpeed( 1 );
    Pig02.setMirror( ( int ) * Pig.Mirror_Index );
    Pig03 = Player( &Animals[ 0 ], SPRITE.Pigs3.x, SPRITE.Pigs3.y, ( int ) * Pig.Frame_Index, 0 );
    Pig03.setSpeed( 1 );
    Pig03.setMirror( ( int ) * Pig.Mirror_Index );
    Pig04 = Player( &Animals[ 0 ], SPRITE.Pigs4.x, SPRITE.Pigs4.y, ( int ) * Pig.Frame_Index, 0 );
    Pig04.setSpeed( 1 );
    Pig04.setMirror( ( int ) * Pig.Mirror_Index );

    //讀取烏龜圖片位置
    Turtle.Catch();

    //載入烏龜01~03
    Turtle01 = Player( &Animals[ 2 ], SPRITE.Turtle1.x, SPRITE.Turtle1.y, ( int ) * Turtle.Frame_Index, 0 );
    Turtle01.setSpeed( 1 );
    Turtle01.setMirror( ( int ) * Turtle.Mirror_Index );
    Turtle02 = Player( &Animals[ 2 ], SPRITE.Turtle2.x, SPRITE.Turtle2.y, ( int ) * Turtle.Frame_Index, 0 );
    Turtle02.setSpeed( 1 );
    Turtle02.setMirror( ( int ) * Turtle.Mirror_Index );
    Turtle03 = Player( &Animals[ 2 ], SPRITE.Turtle3.x, SPRITE.Turtle3.y, ( int ) * Turtle.Frame_Index, 0 );
    Turtle03.setSpeed( 1 );
    Turtle03.setMirror( ( int ) * Turtle.Mirror_Index );

    //載入烏龜影子
    Turtle_Shadow.Catch();
    Turtles_Shadow = Player( &Animals[ 2 ], SPRITE.Turtle1.x, SPRITE.Turtle1.y, ( int ) * Turtle_Shadow.Frame_Index, 0 );
    Turtles_Shadow.setMirror( ( int ) * Turtle_Shadow.Mirror_Index );

    //載入熊貓01~02
    Panda01 = Player( &Animals[ 1 ], Class[ 13 ].End.x, ROLE_HEIGHT - TOP_HEIGHT + Animal_Panda.y, 1103, 0 );
    Panda01.setSpeed( 2 );
    Panda02 = Player( &Animals[ 1 ], Class[ 15 ].End.x, ROLE_HEIGHT - TOP_HEIGHT + Animal_Panda.y, 1103, 0 );
    Panda02.setSpeed( 2 );

    height_arrow = 0;

    //設定山豬AI
    SPRITE.aiPig01.ai = qk;
    SPRITE.aiPig01.endtime = 30;
    SPRITE.aiPig04 = SPRITE.aiPig03 = SPRITE.aiPig02 = SPRITE.aiPig01;
    SPRITE.aiPig03.route = true; //route: true 為行走路徑為左 ，false為行走路徑為右
    SPRITE.aiPig04.stay = true; //stay為是否等待敵人超前
    //設定山豬加速速度
    SPRITE.aiPig01.shift = 3;
    SPRITE.aiPig02.shift = 3;
    SPRITE.aiPig03.shift = 3;
    SPRITE.aiPig04.shift = 4;
    //設定山豬起始位置與路徑終點
    SPRITE.aiPig01.StartX = -64;
    SPRITE.aiPig02.StartX = -64;
    SPRITE.aiPig03.StartX = 1400;
    SPRITE.aiPig04.StartX = 448;
    SPRITE.aiPig01.EndX = Class[ 12 ].End.x - Animal_Pig.x * 1.5;
    SPRITE.aiPig02.EndX = Class[ 6 ].End.x - Animal_Pig.x * 1.5;
    SPRITE.aiPig03.EndX = Class[ 14 ].Strat.x + Animal_Pig.x / 2;
    SPRITE.aiPig04.EndX = Class[ 16 ].End.x - Animal_Pig.x / 2;

    //設定烏龜AI
    SPRITE.aiTurtle01.ai = qk;
    SPRITE.aiTurtle01.endtime = 30;

    SPRITE.aiTurtle03 = SPRITE.aiTurtle02 = SPRITE.aiTurtle01;

    SPRITE.aiTurtle01.local = SPRITE.Turtle1.y;
    SPRITE.aiTurtle02.local = SPRITE.Turtle2.y;
    SPRITE.aiTurtle03.local = SPRITE.Turtle3.y;

    SPRITE.aiTurtle01.StartX = Class[ 0 ].Strat.x;
    SPRITE.aiTurtle02.StartX = Class[ 4 ].Strat.x;
    SPRITE.aiTurtle03.StartX = Class[ 9 ].Strat.x;

    SPRITE.aiTurtle01.shift = 2;
    SPRITE.aiTurtle02.shift = 2;
    SPRITE.aiTurtle03.shift = 2;

    SPRITE.aiTurtle01.EndX = Class[ 0 ].End.x;
    SPRITE.aiTurtle02.EndX = Class[ 4 ].End.x;
    SPRITE.aiTurtle03.EndX = Class[ 9 ].End.x;


    //Draw_Player02=Draw_Player01;
}

int THEME1_Loading()
{

    SDL_Thread * thread;
    thread = SDL_CreateThread( Load, NULL );
    return 0;
}

int THEME1()
{

    SDL_Thread * thread;
    thread = SDL_CreateThread( Gamemain, NULL );
    Input();

    return 0;
}

void Input()
{
    SDL_Event event;
    Uint8 *keystate;
    FPSmanager t_fps;
    SDL_initFramerate( &t_fps );
    SDL_setFramerate( &t_fps , 30 );
    static float a = 0;

    while ( gameover != 1 )
    {

        if ( SDL_PollEvent( &event ) )
        {
            /* an event was found */

            switch ( event.type )
            {
                /* close button clicked */
            case SDL_QUIT:
                gameover = 1;
                break;

                /* handle the keyboard */
            case SDL_KEYDOWN:

                switch ( event.key.keysym.sym )
                {

                case SDLK_TAB:
                    break;
                case SDLK_DOWN:
                    SPRITE.Mala.gpoint.y = 0;
                    break;
                case SDLK_ESCAPE:
                case SDLK_q:
                    gameover = 1;
                    break;
                case SDLK_z:
                    key_z = true;
                    break;
                case SDLK_x:
                    key_x = true;
                    break;
                case SDLK_w:
                    key_w = true;
                    break;
                case SDLK_s:
                    key_s = true;
                    break;
                case SDLK_KP1:
                    key_c = true;
                    break;
                case SDLK_KP2:
                    key_v = true;
                    break;

                }

                break;
            case SDL_KEYUP:

                switch ( event.key.keysym.sym )
                {

                case SDLK_RIGHT:
                    a = x_t + SCREEN_WIDTH / 8 < SCREEN_WIDTH ? x_t + SCREEN_WIDTH / 8 : SCREEN_WIDTH;
                    x_t = a;
                    break;

                case SDLK_SPACE:
                    space = true;

                    break;
                case SDLK_KP0:
                    key_b = true;
                    break;
                case SDLK_LEFT:
                    a = x_t - SCREEN_WIDTH / 8 > 0 ? x_t - SCREEN_WIDTH / 8 : 0;
                    x_t = a;
                    break;


                default:
                    //int i;
                    //i=event.key.keysym.sym;
                    //i=0;
                    break;
                }

                break;
            }
        }

        keystate = SDL_GetKeyState( NULL );

        if ( keystate[ SDLK_LEFT ] )
        {}

        if ( keystate[ SDLK_RIGHT ] )
        {}

        if ( keystate[ SDLK_DOWN ] )
        {}

        if ( keystate[ SDLK_UP ] )
        {}

        if ( keystate[ SDLK_a ] )
        {
            key_a = true;
        }

        if ( keystate[ SDLK_d ] )
        {
            key_d = true;
        }

        if ( keystate[ SDLK_SPACE ] )
        {}

        SDL_framerateDelay( &t_fps );

    }
}
