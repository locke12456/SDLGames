/**************************************************************
Author : Locke 

**************************************************************/
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_rotozoom.h"
#include <stdio.h>    /* for printf and NULL */
#include <stdlib.h>   /* for exit */
#include "math.h"
#include "UsrFuc.h"
#include "map.h"
#include "SDL_thread.h"
#include "SDL_framerate.h"
#include "SDL_config.h"
#include "SDL_video.h"
extern SDL_Surface *SpriteData[];
extern SDL_Overlay *overlay;
Multi_Map::Multi_Map()
{
	init();
}

Multi_Map::Multi_Map( unsigned int amount, int Map_Index, int Map_Height, int Map_Width )
{
	init();
    for ( int i = 0;i < BUFFER_AMOUNT;i++ )
    {
		Maps[ i ].showFlag = true;
        Maps[ i ].AMOUNT = amount;
        Maps[ i ].FRAME_INDEX = Map_Index;
        Maps[ i ].FRAME_HEIGHT = SpriteData[ Map_Index ] ->h;
        Maps[ i ].FRAME_WIDTH = SpriteData[ Map_Index ] ->w;
        Maps[ i ].HEIGHT = Map_Height;
        Maps[ i ].WIDTH = Map_Width;
		
    }
	
}
void Multi_Map::Depth_Map(unsigned int amount, int Map_Index, int Map_Height, int Map_Width)
{
    for ( int i = 0;i < BUFFER_AMOUNT;i++ )
    {
		DepthMaps[ i ].showFlag = true;
        DepthMaps[ i ].AMOUNT = amount;
        DepthMaps[ i ].FRAME_INDEX = Map_Index;
        DepthMaps[ i ].FRAME_HEIGHT = SpriteData[ Map_Index ] ->h;
        DepthMaps[ i ].FRAME_WIDTH = SpriteData[ Map_Index ] ->w;
        DepthMaps[ i ].HEIGHT = Map_Height;
        DepthMaps[ i ].WIDTH = Map_Width;
		
    }
}
bool Multi_Map::add_Draw( unsigned int layer, int layer_num, void ( *draw ) ( void ) )
{
    if ( layer >= LAYER_AMOUNT )
        return false;

    this->Layer[ layer ].showFlag = true;

    this->Layer[ layer ].Layer_num = layer_num;

    this->Layer[ layer ].draw = ( *draw );

    return true;
}

bool Multi_Map::setMap_Center( int Which, SDL_Rect *xy )
{
    if ( Which >= BUFFER_AMOUNT )
        return false;

    this->Maps[ Which ].POSITION = ( SDL_Rect * ) xy;
	this->DepthMaps[ Which ].POSITION = ( SDL_Rect * ) xy;
    return true;
}

void Multi_Map::Draw()
{

    for ( int i = 0;i < BUFFER_AMOUNT;i++ )
    {
		if(DepthMaps[i].showFlag){
			draw_Map = OBJ( 0, 0, 0, DepthMaps[i].FRAME_INDEX, -1 );
			if ( i == 0 ) draw_Map.setBuffer( LEFT_BUFFER );
			else draw_Map.setBuffer( RIGHT_BUFFER );
			Draw_Map(&DepthMaps[i]);
		}
	}
	if(this->Layer[ 0 ].showFlag)this->Layer[ 0 ].draw();
    for ( int i = 0;i < BUFFER_AMOUNT;i++ )
    {	
		if(Maps[i].showFlag){
			draw_Map = OBJ( 0, 0, 0, Maps[ i ].FRAME_INDEX, -1 );
			if ( i == 0 ) draw_Map.setBuffer( LEFT_BUFFER );
			else draw_Map.setBuffer( RIGHT_BUFFER );
			Draw_Map(&Maps[i]);
		}
    }
	for ( int i = 1;i < LAYER_AMOUNT;i++ )
		if(this->Layer[ i ].showFlag)this->Layer[ i ].draw();
}

void Multi_Map::Draw_Map(MAP *Maps)
{
    
    int Count = 0, trueCount = 0, printCount = 0, k = 0;
    int StartY = ( int ) ( Maps->POSITION->y / Maps->FRAME_HEIGHT ) * Maps->FRAME_HEIGHT;
    int StartX = ( int ) ( Maps->POSITION->x / Maps->FRAME_WIDTH ) * Maps->FRAME_WIDTH;
	if(StartY>Maps->HEIGHT)
		return;
    for ( int j = StartX;j < StartX + BUFFER_WIDTH + Maps->FRAME_WIDTH;j += Maps->FRAME_WIDTH )
    {
        if ( abs( j - Maps->POSITION->x ) <= BUFFER_WIDTH )
        {
            for ( int i = StartY;i < StartY + BUFFER_HEIGHT + Maps->FRAME_HEIGHT;i += Maps->FRAME_HEIGHT )
            {
                if ( abs( i - Maps->POSITION->y ) <= BUFFER_HEIGHT )
                {
                    k = ( ( i / Maps->FRAME_HEIGHT ) * ( Maps->WIDTH / Maps->FRAME_HEIGHT ) );

                    Count = k + ( j / Maps->FRAME_WIDTH );
                    
					draw_Map.x =j;

					draw_Map.y =i;
					
					draw_Map.frame=Count;
                    
					draw_Map.x -= Maps->POSITION->x;
                    
					draw_Map.y -= Maps->POSITION->y;

					draw_Map.showFlag = Count<Maps->AMOUNT&&( abs( draw_Map.y ) <= BUFFER_HEIGHT && abs( draw_Map.x ) <= BUFFER_WIDTH );

                    trueCount = draw_Map.showFlag == true ? trueCount + 1 : trueCount;
					//if (SDL_LockYUVOverlay(overlay) < 0) return;
                    draw_Map.draw();
					//SDL_UnlockYUVOverlay(overlay);
					//SDL_DisplayYUVOverlay(overlay, &SpriteData[ Maps->FRAME_INDEX ]->clip_rect);	
                    printCount++;
                }
            }
        }

        /*if(trueCount>=(BUFFER_HEIGHT/Maps[Map].FRAME_HEIGHT+1)*(BUFFER_WIDTH/Maps[Map].FRAME_WIDTH+1))
        {
         break;
        }*/
    }

}
void Multi_Map::init()
{
	memset(this,0,sizeof(Multi_Map));
}
//void Multi_Map::Draw_Map(int Map)
//{
// OBJ draw_Map;
// int Count=0,trueCount=0,printCount=0,k=0;
// for(int i=0;i<Maps[Map].HEIGHT;i+=Maps[Map].FRAME_HEIGHT)
// {
//  if(abs(i-Maps[Map].POSITION->y)<=BUFFER_HEIGHT){
//   for(int j=0;j<Maps[Map].WIDTH;j+=Maps[Map].FRAME_WIDTH)
//   {
//    if(abs(j-Maps[Map].POSITION->x)<=BUFFER_WIDTH){
//     k=((i/Maps[Map].FRAME_HEIGHT)*(Maps[Map].WIDTH/Maps[Map].FRAME_HEIGHT));
//     Count=k+(j/Maps[Map].FRAME_WIDTH);
//     draw_Map=OBJ(j,i,Count,Maps[Map].FRAME_INDEX,-1);
//     draw_Map.x-=Maps[Map].POSITION->x;
//     draw_Map.y-=Maps[Map].POSITION->y;
//     if(Map==0)draw_Map.setBuffer(LEFT_BUFFER);
//     else draw_Map.setBuffer(RIGHT_BUFFER);
//     draw_Map.showFlag=(abs(draw_Map.y)<=BUFFER_HEIGHT&&abs(draw_Map.x)<=BUFFER_WIDTH);
//     trueCount=draw_Map.showFlag==true?trueCount+1:trueCount;
//     draw_Map.draw();
//     printCount++;
//    }
//   }
//  }
//  /*if(trueCount>=(BUFFER_HEIGHT/Maps[Map].FRAME_HEIGHT+1)*(BUFFER_WIDTH/Maps[Map].FRAME_WIDTH+1))
//  {
//   break;
//  }*/
// }
//
//}
