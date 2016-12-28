//#include "windows.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "Url.h"
#include "rom.h"
#include "Spritemap.h"
#include "UsrFuc.h"
#include "page.h"
#include "Sprite.h"
#include <stdio.h>    /* for printf and NULL */
#include <stdlib.h>   /* for exit */
#include <math.h>     /* for sqrt, sin, and cos */
#include <assert.h>   /* for assert */
//#if __APPLE__
//#include <GLUT/glut.h>
//#include <OpenGL/glext.h>
//#else
//#include <GL/glut.h>
//#include <GL/glext.h>
//#endif
//#include <Cg/cg.h>    /* Can't include this?  Is Cg Toolkit installed! */
//#include <Cg/cgGL.h>
//#define SCREEN_WIDTH  640
//#define SCREEN_HEIGHT 860


void bitmapInit();
void init();
void freeSprite();
void readRom();
SDL_Rect rcSprite, rcGrass;
SDL_Surface *screen, *screen_buffer[ 2 ], *sprite[ 10 ];
SDL_Surface *SpriteData[ SPRITE_SIZE ];
SDL_Overlay *overlay;
int colorkey, PAGE = 1;
char type[ SPRITE_SIZE ] = {0};

Sprite Player_01 = Sprite();
Sprite Player_02 = Sprite();
Sprite Player_01_Shadow = Sprite();
Sprite Player_02_Shadow = Sprite();
Sprite Pig = Sprite();
Sprite Turtle = Sprite();
Sprite Turtle_Shadow = Sprite();
Sprite Smoke = Sprite();
int update=0;
int main ( int argc, char *argv[] )
{
	//int Random_Base=5;
	//int a[50];
	//int i,m,s,Table_Count=1,Raid_Count=0;
	//bool Adali_Flag=0;
	//while(1)
	//{
	//	Adali_Flag=0;
	//	if(Raid_Count==0||Table_Count%6==Random_Base)
	//	{
	//		//Table_Count=0;
	//		Raid_Count=(Random_Base-1);
	//		for(i=0;i<Random_Base;i++)
 //  				a[i]=i+1;
 //  			for(i=0;i<Random_Base;i++)
	//		{
 //				//srand((TH0<<8)|TL0);
 //  		   		m=rand()%Random_Base;
 //  	   		    s=a[m];
 //   	  	    a[m]=a[i];
 //    		    a[i]=s;
 //    		}
	//	}
	//
	//	if(a[Table_Count%5]==Random_Base)
	//	{
	//		Adali_Flag=1;					//為是中獎旗號設為1
	//		Raid_Count--;
	//		//Table_Count++;
	//	}
	//	else
	//	{
	//		Adali_Flag=0;						//為是中獎旗號設為0
	//	 	Raid_Count--;
	//		//Table_Count++;
	//	}
	//	Table_Count++;
	//}
    //int i = 1;
    init();
    THEME1();
    SDL_Delay( 150 );
    freeSprite();
    SDL_Quit();
    return 0;

}

void init()
{

    //readRom();
    //memset( &Sprite, 0, sizeof( Sprite ) );

    /* initialize SDL */

    SDL_Init(SDL_INIT_VIDEO);

    /* set the title bar */
    SDL_WM_SetCaption( "Mala", "Mala" );

    /* create window */

    /*screen_buffer[ 0 ] = SDL_SetVideoMode( SCREEN_WIDTH / 2, SCREEN_HEIGHT, 0, 0 );
    screen_buffer[ 1 ] = SDL_SetVideoMode( SCREEN_WIDTH / 2, SCREEN_HEIGHT, 0, 0 );*/
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 0,0/*SDL_HWSURFACE |SDL_DOUBLEBUF | SDL_FULLSCREEN*/); //, SDL_FULLSCREEN | SDL_HWSURFACE );
	//overlay = SDL_CreateYUVOverlay(SCREEN_WIDTH, SCREEN_HEIGHT,SDL_YVYU_OVERLAY,screen);
    /* load sprite */

    /* setup sprite colorkey and turn on RLE */
    colorkey = SDL_MapRGB( screen->format, 248, 0, 248 );

    /* set sprite position */
    rcSprite.x = 0;
    rcSprite.y = 0;
	THEME1_Loading();
    bitmapInit();
}


void bitmapInit()
{
    int i = 0, index;
    char * rom;
    long lSize;

    FILE * fp = fopen( "rom", "rb" );

    fseek ( fp , 0 , SEEK_END );

    lSize = ftell ( fp );

    rewind ( fp );

    rom = ( char* ) malloc( lSize );

    fread( rom, lSize, sizeof( char ), fp );

    fclose( fp );

    for ( i = 0;i < 600;i++ )
    {
        index = i ;
        Load_Rom( RES_up + PAGE_up[ index ], up[ index ], i , rom );
    }
	for ( i = 600;i < 628;i++ )
    {
        index = i-600;
        Load_Rom( RES_gpa + PAGE_gpa[ index ], gpa[ index ], i , rom );
    }
	Player_01.load(28,600);
    for ( i = 712;i < 718;i++ )
    {
        index = i-712;
        Load_Rom( RES_pig + PAGE_pig[ index ], pig[ index ], i , rom );
    }
	Pig.load(6,712);
	Load_Rom( RES_effect + PAGE_effect[ 6 ], effect[ 6 ], 736 , rom );
	//Convey2Lowbilt( 736, 737 );
	//Load_Rom( RES_road + PAGE_road[ 0 ], road[ 0 ], 737 , rom );
	for(i=738;i<748;i++){
		SDL_Surface * s;
		s= rotozoomSurface( SpriteData[ 736 ], 0, (i-738+1)*0.05f, 1 );
		SpriteData[ i ]= SDL_DisplayFormatAlpha( s );
		type[i]=type[736];
		SDL_FreeSurface( s );
	}
	Smoke.load_Particle(10,738,0xffffff,0xff);
	for ( i = 778;i < 808;i++ )
    {
        index = i-778;
        Load_Rom( RES_waterfall01 + PAGE_waterfall01[ index ], waterfall01[ index ], i , rom );
    }
	for ( i = 808;i < 838;i++ )
    {
        index = i-808;
        Load_Rom( RES_waterfall02 + PAGE_waterfall02[ index ], waterfall02[ index ], i , rom );
    }
	for ( i = 838;i < 868;i++ )
    {
        index = i-838;
        Load_Rom( RES_waterfall03 + PAGE_waterfall03[ index ], waterfall03[ index ], i , rom );
    }
	Load_Rom( RES_up + PAGE_up[ 0 ], up[ 0 ], 868 , rom );
	for ( i = 869;i < 873;i++ )
    {
        index = i-869;
        Load_Rom( RES_bridge + PAGE_bridge[ index ], bridge[ index ], i , rom );
    }
	for ( i = 873;i < 875;i++ )
    {
        index = i-873;
        Load_Rom( RES_other + PAGE_other[ index ], other[ index ], i , rom );
    }
	for ( i = 875;i < 1103;i++ )
    {
        index = i-875;
        Load_Rom( RES_sky + PAGE_sky[ index ], sky[ index ], i , rom );
    }
	for ( i = 1103;i < 1124;i++ )
    {
        index = i-1103;
        Load_Rom( RES_panda + PAGE_panda[ index ], panda[ index ], i , rom );
    }
	for ( i = 1124;i < 1136;i++ )
    {
        index = i-1124;
        Load_Rom( RES_bridge_log + PAGE_bridge_log[ index ], bridge_log[ index ], i , rom );
    }
	for ( i = 1136;i < 1151;i++ )
    {
        index = i-1136;
        Load_Rom( RES_pig_effect + PAGE_pig_effect[ index ], pig_effect[ index ], i , rom );
    }
	for ( i = 1151;i < 1166;i++ )
    {
        index = i - 1151;
        Convey2Mirror( 1136 + index, i );
    }
	for ( i = 1166;i < 1196;i++ )
    {
        index = i-1166;
        Load_Rom( RES_panda_effect + PAGE_panda_effect[ index ], panda_effect[ index ], i , rom );
    }
	
    for ( i = 1408;i < 1412;i++ )
    {
        index = i-1408;
        Load_Rom( RES_CaveMask + PAGE_CaveMask[ index ], CaveMask[ index ], i , rom );
    }
	for ( i = 1412;i < 1466;i++ )
    {
        index = i-1412;
        Load_Rom( RES_gma + PAGE_gma[ index ], gma[ index ], i , rom );
    }
	Player_02.load(54,1412);
	for ( i = 1628;i < 1656;i++ )
    {
        index = i-1628;
        Load_Rom( RES_gpas + PAGE_gpas[ index ], gpas[ index ], i , rom );
    }
	Player_01_Shadow.load(28,1628);
	for ( i = 1740;i < 1794;i++ )
    {
        index = i-1740;
        Load_Rom( RES_gmas + PAGE_gmas[ index ], gmas[ index ], i , rom );
    }
	Player_02_Shadow.load(54,1740);
	for ( i = 1956;i < 1964;i++ )
    {
        index = i-1956;
        Load_Rom( RES_turtle + PAGE_turtle[ index ], turtle[ index ], i , rom );
    }
	Turtle.load(8,1956);
	for ( i = 1988;i < 1996;i++ )
    {
        index = i-1988;
        Load_Rom( RES_turtles + PAGE_turtles[ index ], turtles[ index ], i , rom );
    }
	Turtle_Shadow.load(8,1988);
	Load_Rom( RES_Placard_Jump + PAGE_Placard_Jump[ 0 ], Placard_Jump[ 0 ], 2020 , rom );
	for ( i = 2021;i < 2025;i++ )
    {
        index = i-2021;
        Load_Rom( RES_cloud + PAGE_cloud[ index ], cloud[ index ], i , rom );
    }
	for ( i = 2025;i < 2029;i++ )
    {
		index = i-2025;
		SDL_Surface * s;
		s= rotozoomSurface( SpriteData[ 2021+index ], 0, 0.8f, 1 );
		SpriteData[ i ]= SDL_DisplayFormatAlpha( s );
		type[i]=type[2021+index];
		SDL_FreeSurface( s );
        
    }
	for ( i = 2029;i < 2033;i++ )
    {
        index = i-2029;
        SDL_Surface * s;
		s= rotozoomSurface( SpriteData[ 2021+index ], 0, 0.6f, 1 );
		SpriteData[ i ]= SDL_DisplayFormatAlpha( s );
		type[i]=type[2021+index];
		SDL_FreeSurface( s );
    }
	for ( i = 2033;i < 2037;i++ )
    {
        index = i-2033;
        Load_Rom( RES_Guide + PAGE_Guide[ index ], Guide[ index ], i , rom );
    }
    /*for(i=2037;i<2063;i++)
    {
		index=i-2037;
		Load_Rom(RES_ENG+PAGE_ENG[index],ENG[index],i);
    }*/
	//Load_Rom( RES_road + PAGE_road[ 0 ], road[ 0 ], 778 , rom );
	//Convey2Mirror(778,779);
//	Smoke.addColor(0,0.5f);
  //  for ( i = 969;i < 973;i++ )
  //  {
  //      index = i - 969;
		////Convey2Lowbilt( 965 + index, i );
  //      Convey2Color( 965 + index, i, 1, 0 );
		////SpriteData[i+4]=SDL_DisplayFormat( SpriteData[ 969 + index ] );
  //      //Convey2Lowbilt( 969 + index, i+4 );
  //      //Convey2Mirror(965+index,i);
  //  }

  //  for ( i = 973;i < 977;i++ )
  //  {
  //      index = i - 973;
  //      Convey2Color( 965 + index, i, 4, 0 );
  //      //Convey2Lowbilt( 973 + index, i );
  //      //Convey2Mirror(965+index,i);
  //  }

  //  for ( i = 977;i < 989;i++ )
  //  {
  //      index = i - 977;
  //      Convey2Mirror( 965 + index, i );
  //  }

	/*pp.addColor(0,0.25);
	pp.addColor_Mirror(0,0.25);
	pp.addColor(4,0);
	pp.addColor_Mirror(4,0);*/

	//i=(965+*pp.Total);
	//for (;i < (965+*pp.Total)+1;i++ )
 //   {
 //       index = i - (965+*pp.Total)+0;
 //       Load_Rom( RES_effect + PAGE_effect[ index ], effect[ index ], i , rom );
 //   }
	//
	//Convey2Lowbilt( 997, 998 );
	//Convey2Color( 998, 999, 0, 0.1f );
	//for(i=1000;i<1010;i++)
	//SpriteData[ i ] = rotozoomSurface( SpriteData[ 999 ], 0, 0.5f/*(float)(i-1000)/10+0.1f*/, 0 );
    free( rom );
    /*for(i=262;i<288;i++)
    {
     index=i-262;
     Load_Rom(RES_ENG+PAGE_ENG[index],ENG[index],i);
    }*/

}

void freeSprite()
{
    for ( int i = 0;i < SPRITE_SIZE;i++ )
        SDL_FreeSurface( SpriteData[ i ] );

    //SDL_UpdateRect( screen, 0 , 0, 0, 0 );
    SDL_FreeSurface( screen );

    SDL_FreeSurface( screen_buffer[ 0 ] );

    SDL_FreeSurface( screen_buffer[ 1 ] );
}


