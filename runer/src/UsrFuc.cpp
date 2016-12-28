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
//#include "map.h"
#include "SDL_thread.h"
#include "SDL_framerate.h"
#include "SDL_config.h"
#include "SDL_video.h"
#include <string>
//using namespace std;

#define LAYER    10
#define LIMIT    10


extern SDL_Surface *SpriteData[];
extern SDL_Surface * screen, *screen_buffer[], *sprite[];
extern SDL_RWops *Sound[];
extern int colorkey;
extern int PAGE;
extern char type[];
extern SDL_Overlay *overlay;
extern int update;
SDL_Surface * spriteIndex , *tempSurface;
Mix_Chunk *Chunk[ SOUND_CHUNK ];
Mix_Music * mix_music ;
unsigned int ChunkIndex = 0;
void Check_effect( SP_TYPE * , int );
void PNG_ALPHA( SP_TYPE * );
void MIRROR( SP_TYPE * , int );
void Page_Count();
void BlitSurface( SDL_Rect * );
int sqrt_line( Point *, Point * );
Uint32 GetPixel ( SDL_Surface* , int , int ) ;
SDL_Color GetRGB( SDL_Surface* , int , int ) ;
void putpixel( SDL_Surface *, int , int , Uint32 );
bool over = true;

int time( void * );
SDL_Thread * thread;
int PIC=0;
//thread = SDL_CreateThread( time, NULL );
/**************************************************************
 Use struct SP_TYPE to configure effect
   
 **************************************************************/
void drawSpriteBmp( SP_TYPE *rc, int p )
{
    if ( !rc->RotataionFlag && !rc->ZoomFlag )
    {
        spriteIndex = SpriteData[ p ];
    }
    else
    {

        Check_effect( rc , p );
    }

    if ( rc->MirrorFlag )
    {
        MIRROR( rc , p );
    }
    else if ( rc->AlphaFlag && type[ PAGE / LIMIT ] == 'B' )
    {
        SDL_SetAlpha( spriteIndex, SDL_SRCALPHA | SDL_RLEACCEL, 255 - rc->Alpha );
        SDL_SetColorKey( spriteIndex, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey );
        BlitSurface( &rc->rcSprite );
        //SDL_BlitSurface( spriteIndex, NULL, screen, &rc->rcSprite );

        if ( rc->RotataionFlag || rc->ZoomFlag )
            SDL_FreeSurface( spriteIndex );
    }
    else if ( rc->RotataionFlag || rc->ZoomFlag )
    {
        SDL_SetColorKey( spriteIndex, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey );
        BlitSurface( &rc->rcSprite );
        //SDL_BlitSurface( spriteIndex, NULL, screen, &rc->rcSprite );
        SDL_FreeSurface( spriteIndex );
    }
    else
    {
        SDL_SetColorKey( spriteIndex, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey );
        BlitSurface( &rc->rcSprite );
        //SDL_BlitSurface( spriteIndex, NULL, screen, &rc->rcSprite );
    }

    Page_Count();
}

void drawSpritePng( SP_TYPE *rc, int p ,bool pngalpha)
{
    if ( !rc->RotataionFlag && !rc->ZoomFlag && !rc->AlphaFlag )
    {
        spriteIndex = SpriteData[ p ] ;
    }
    else
    {
        Check_effect( rc , p );
    }

    if ( rc->MirrorFlag )
        MIRROR( rc , p );
    else if ( rc->AlphaFlag )
    {
        spriteIndex = SpriteData[ p ] ;
        if(!pngalpha)
			PNG_ALPHA( rc );
		else
			PNG_ALPHA_Lowspeed( rc );

        if ( rc->RotataionFlag || rc->ZoomFlag )
            SDL_FreeSurface( spriteIndex );
    }
    else if ( rc->RotataionFlag || rc->ZoomFlag )
    {
        BlitSurface( &rc->rcSprite );
        //SDL_BlitSurface( spriteIndex, NULL, screen, &rc->rcSprite );
        SDL_FreeSurface( spriteIndex );
    }
    else
    {
        BlitSurface( &rc->rcSprite );
        //SDL_BlitSurface( spriteIndex, NULL, screen, &rc->rcSprite );
    }


    Page_Count();
}

void drawSprite( SDL_Rect *rc, int p )
{
    spriteIndex = SpriteData[ p ] ;

    SDL_SetColorKey( spriteIndex, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey );

    BlitSurface( rc );
}

void drawSprite_fullcolor( SDL_Rect *rc, int p )
{
    spriteIndex = SpriteData[ p ] ;

    BlitSurface( rc );
}

void Check_effect( SP_TYPE *rc , int p )
{
    double zoom = 1, rot = 0;
    SDL_Surface * s;

    if ( rc->RotataionFlag )
        rot = rc->Rotataion;

    if ( rc->ZoomFlag )
        zoom = rc->Zoom;

    if ( rc->RotataionFlag || rc->ZoomFlag )
    {
        int a = ( type[ p ] == 'B' || ( type[ p ] == 'P' && rc->AlphaFlag ) ) ? 0 : 1;

        s = rotozoomSurface( SpriteData[ p ], rot, zoom, a );

        if ( type[ p ] == 'B' && !rc->MirrorFlag || type[ p ] == 'P' && rc->AlphaFlag )
        {
            spriteIndex = SDL_DisplayFormat( s );
        }
        else
            spriteIndex = SDL_DisplayFormatAlpha( s );

        SDL_FreeSurface( s );
    }

}

void PNG_ALPHA( SP_TYPE *rc )
{
    SDL_SetAlpha( spriteIndex, SDL_SRCALPHA | SDL_RLEACCEL, 255 - rc->Alpha );
    SDL_SetColorKey( spriteIndex, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey );
    BlitSurface( &rc->rcSprite );
    //SDL_BlitSurface( spriteIndex, NULL, screen, &rc->rcSprite );
}

void PNG_ALPHA_Lowspeed( SP_TYPE *rc )
{
    SDL_Color rgb1, rgb2;
    Uint32 a1, a2, a3, A, rgb3;
#pragma omp parallel for

    for ( int j = 0;j < spriteIndex->h - 1;++j )
    {
        for ( int i = 0;i < spriteIndex->w - 1;++i )
        {
            bool over = ( ( spriteIndex->w - i ) < SCREEN_WIDTH && rc->rcSprite.y + j < SCREEN_HEIGHT && rc->rcSprite.x + ( spriteIndex->w - i ) >= 0 && rc->rcSprite.y + j >= 0 );
            rgb1 = GetRGB( spriteIndex, i, j );

            if ( rgb1.unused != 0 && over )
            {
                A = ( Uint32 ) ( ( float ) rgb1.unused * ( float ) ( ( 255 - ( float ) rc->Alpha ) / 255 ) );
                //rgb1 = GetRGB( spriteIndex, i, j );
                rgb2 = GetRGB( screen, rc->rcSprite.x + i, rc->rcSprite.y + j );

                a1 = ( A * rgb1.r + ( 255 - A ) * rgb2.r ) / 255;
                a2 = ( A * rgb1.g + ( 255 - A ) * rgb2.g ) / 255;
                a3 = ( A * rgb1.b + ( 255 - A ) * rgb2.b ) / 255;
                rgb3 = ( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
                putpixel( screen, rc->rcSprite.x + i, rc->rcSprite.y + j, rgb3 );
            }
        }
    }
}

void MIRROR( SP_TYPE *rc , int p )
{
    SDL_Color rgb1, rgb2;
    Uint32 a1, a2, a3, A, rgb3, rgb4;
#pragma omp parallel for

    for ( int j = 0;j < spriteIndex->h - 1;++j )
    {
        for ( int i = spriteIndex->w - 1;i > 0;--i )
        {
            bool over = ( ( spriteIndex->w - i ) < SCREEN_WIDTH && rc->rcSprite.y + j < SCREEN_HEIGHT && rc->rcSprite.x + ( spriteIndex->w - i ) >= 0 && rc->rcSprite.y + j >= 0 );
            rgb1 = GetRGB( spriteIndex, i, j );


            if ( rgb1.unused != 0 && over )
            {
                A = ( Uint32 ) ( ( float ) rgb1.unused * ( float ) ( ( 255 - ( float ) rc->Alpha ) / 255 ) );
                rgb2 = GetRGB( screen, rc->rcSprite.x + ( spriteIndex->w - i ), rc->rcSprite.y + j );
                a1 = ( A * rgb1.r + ( 255 - A ) * rgb2.r ) / 255;
                a2 = ( A * rgb1.g + ( 255 - A ) * rgb2.g ) / 255;
                a3 = ( A * rgb1.b + ( 255 - A ) * rgb2.b ) / 255;
                rgb3 = ( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
                rgb4 = ( rgb1.r << 0x10 ) + ( rgb1.g << 0x8 ) + ( rgb1.b );

                if ( ( colorkey != rgb4 || type[ p ] == 'P' ) )
                {
                    putpixel( screen, rc->rcSprite.x + ( spriteIndex->w - i ), rc->rcSprite.y + j, rgb3 );
                }
            }
        }
    }

    //SDL_FreeSurface( spriteIndex );
}

//void PNG_ALPHA( SP_TYPE *rc )
//{
//    Uint32 a1, a2, a3, a4, a5, a6, rgb1, rgb2, rgb3, A;
// Uint32 b=255;
// float Alpha=( b - rc->Alpha ) / b;
//#pragma omp parallel for
//
//    for ( int j = 0;j < spriteIndex->h - 1;++j )
//    {
//        for ( int i = 0;i < spriteIndex->w - 1;++i )
//        {
//            rgb1 = GetPixel( spriteIndex, i, j );
//            A = ( rgb1 >> spriteIndex->format->Ashift ) & 0xff;
//
//            if ( A != 0 )
//            {
//                A *=Alpha;
//                a1 = ( rgb1 >> spriteIndex->format->Rshift ) & 0xff;
//                a2 = ( rgb1 >> spriteIndex->format->Gshift ) & 0xff;
//                a3 = ( rgb1 >> spriteIndex->format->Bshift ) & 0xff;
//                rgb2 = GetPixel( screen, rc->rcSprite.x + i, rc->rcSprite.y + j );
//                a4 = ( rgb2 >> screen->format->Rshift ) & 0xff;
//                a5 = ( rgb2 >> screen->format->Gshift ) & 0xff;
//                a6 = ( rgb2 >> screen->format->Bshift ) & 0xff;
//                a1 = ( A * a1 + ( 255 - A ) * a4 ) / 255;
//                a2 = ( A * a2 + ( 255 - A ) * a5 ) / 255;
//                a3 = ( A * a3 + ( 255 - A ) * a6 ) / 255;
//                rgb3 = ( a1 << screen->format->Rshift ) + ( a2 << screen->format->Gshift ) + ( a3 << screen->format->Bshift );
//    if(rgb3==colorkey)
//    {
//     rgb3=rgb3;
//    }
//                putpixel( screen, rc->rcSprite.x + i, rc->rcSprite.y + j, rgb3 );
//            }
//        }
//    }
//}

//void MIRROR( SP_TYPE *rc , int p )
//{
//    Uint32 a1, a2, a3, a4, a5, a6, rgb1, rgb2, rgb3, rgb4, A;
//    //SDL_Surface* spriteIndex = SDL_DisplayFormatAlpha( SpriteData[ p ] );
//#pragma omp parallel for
//
//    for ( int j = 0;j < spriteIndex->h - 1;++j )
//    {
//        for ( int i = spriteIndex->w - 1;i > 0;--i )
//        {
//            bool over = ( ( spriteIndex->w - i ) < SCREEN_WIDTH && rc->rcSprite.y + j < SCREEN_HEIGHT && rc->rcSprite.x + ( spriteIndex->w - i ) >= 0 && rc->rcSprite.y + j >= 0 );
//            rgb1 = GetPixel( spriteIndex, i, j );
//            A = ( rgb1 >> spriteIndex->format->Ashift ) & 0xff;
//
//            if ( A != 0 && over )
//            {
//                A = A * ( 255 - rc->Alpha ) / 255;
//                a1 = ( rgb1 >> spriteIndex->format->Rshift ) & 0xff;
//                a2 = ( rgb1 >> spriteIndex->format->Gshift ) & 0xff;
//                a3 = ( rgb1 >> spriteIndex->format->Bshift ) & 0xff;
//                rgb2 = GetPixel( screen, rc->rcSprite.x + ( spriteIndex->w - i ), rc->rcSprite.y + j );
//                a4 = ( rgb2 >> screen->format->Rshift ) & 0xff;
//                a5 = ( rgb2 >> screen->format->Gshift ) & 0xff;
//                a6 = ( rgb2 >> screen->format->Bshift ) & 0xff;
//                rgb4 = ( a1 << screen->format->Rshift ) + ( a2 << screen->format->Gshift ) + ( a3 << screen->format->Bshift );
//                a1 = ( A * a1 + ( 255 - A ) * a4 ) / 255;
//                a2 = ( A * a2 + ( 255 - A ) * a5 ) / 255;
//                a3 = ( A * a3 + ( 255 - A ) * a6 ) / 255;
//
//                rgb3 = ( a1 << screen->format->Rshift ) + ( a2 << screen->format->Gshift ) + ( a3 << screen->format->Bshift );
//
//                if ( ( colorkey != rgb4 || type[ p ] == 'P' ) )
//                {
//                    putpixel( screen, rc->rcSprite.x + ( spriteIndex->w - i ), rc->rcSprite.y + j, rgb3 );
//                }
//            }
//        }
//    }
//
//    //SDL_FreeSurface( spriteIndex );
//}

void FADE_to_Black( float per )
{
    Uint32 rgb3;
    SDL_Color rgb;
#pragma omp parallel for

    for ( int j = 0;j < screen->h ;++j )
        for ( int i = 0;i < screen->w ;++i )
        {
            rgb = GetRGB( screen, i, j );
            rgb3 = ( ( Uint32 ) ( rgb.r - rgb.r * per ) << 0x10 ) + ( ( Uint32 ) ( rgb.g - rgb.g * per ) << 0x8 ) + ( Uint32 ) ( rgb.b - rgb.b * per );
            putpixel( screen, i, j, rgb3 );
        }

}

void FADE_to_White( float per )
{
    Uint32 r, g, b, rgb3;
    SDL_Color rgb;
#pragma omp parallel for

    for ( int j = 0;j < screen->h ;++j )
    {
        for ( int i = 0;i < screen->w ;++i )
        {
            rgb = GetRGB( screen, i, j );
            rgb.r = ( Uint8 ) ( rgb.r + ( 0xff - rgb.r ) * per );
            rgb.g = ( Uint8 ) ( rgb.g + ( 0xff - rgb.g ) * per );
            rgb.b = ( Uint8 ) ( rgb.b + ( 0xff - rgb.b ) * per );
            r = rgb.r << 0x10;
            g = rgb.g << 0x8;
            b = rgb.b;
            rgb3 = r + g + b;
            putpixel( screen, i, j, rgb3 );
        }
    }
}

int time( void *unused )
{
    int cc = 0;
    static unsigned int bb = 0;

    while ( over == true )
    {
        cc++;
    }

    bb += cc / 10000;
    printf( "sc = %d bc = %d\n", cc, bb );
    return 0;
}

void Convey2black( int n, int p /*,float fade*/ )
{
    SpriteData[ p ] = SDL_DisplayFormatAlpha( SpriteData[ n ] );
    SDL_Color rgb1;
    Uint32 rgb3;
    Uint8 a1;
    over = true;


#pragma omp parallel for

    for ( int j = 0;j < SpriteData[ p ] ->h - 1;++j )
    {
        for ( int i = 0;i < SpriteData[ p ] ->w - 1;++i )
        {
            rgb1 = GetRGB( SpriteData[ n ], i, j );
            a1 = ( rgb1.unused * ( ( rgb1.r + rgb1.g + rgb1.b ) / 3 ) ) / 255;
            //SDL_MapRGBA(SpriteData[ p ]->format,a1,a1,a1,rgb1.unused);
            rgb3 = ( rgb1.unused << 0x18 ) + ( a1 << 0x10 ) + ( a1 << 0x8 ) + ( a1 );
            putpixel( SpriteData[ p ], i, j, rgb3 );
        }
    }

    over = false;
}

void Convey2Mirror( int n, int p )
{
    SDL_Rect rect = SpriteData[ n ] ->clip_rect;
    rect.w -= 1;
    rect.h -= 1;

    if ( type[ n ] == 'P' )
        SpriteData[ p ] = SDL_DisplayFormatAlpha( SpriteData[ n ] );
    else
        SpriteData[ p ] = SDL_DisplayFormat( SpriteData[ n ] );

    //SpriteData[ p ] = SDL_CreateRGBSurface(SpriteData[ n ]->flags,SpriteData[ n ]->w+1,SpriteData[ n ]->h+1,SpriteData[ n ]->,SpriteData[ n ]->format->Rmask,SpriteData[ n ]->format->Gmask,SpriteData[ n ]->format->Bmask,SpriteData[ n ]->format->Amask);

    SDL_Color rgb1;

    Uint32 rgb3;

#pragma omp parallel for
    for ( int j = 0;j < SpriteData[ p ] ->h - 1;++j )
    {
        for ( int i = 0;i < SpriteData[ p ] ->w - 1;++i )
        {
            rgb1 = GetRGB( SpriteData[ n ], i, j );

            if ( type[ n ] == 'P' )
                rgb3 = SDL_MapRGBA( SpriteData[ p ] ->format, rgb1.r, rgb1.g, rgb1.b, rgb1.unused ); //(rgb1.unused<<0x18)+( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
            else
                rgb3 = SDL_MapRGB( SpriteData[ p ] ->format, rgb1.r, rgb1.g, rgb1.b );

            putpixel( SpriteData[ p ], SpriteData[ p ] ->w - i, j, rgb3 );
        }
    }

    //SpriteData[ p ]->w=rect.w;
    //SpriteData[ p ]->h=rect.h;
    //SDL_SetClipRect( SpriteData[ p ], &rect );
    type[ p ] = type[ n ];
}

void Convey2Lowbilt( int n, int p )
{
    SDL_Surface * s = SDL_DisplayFormatAlpha( SpriteData[ n ] );
    SpriteData[ p ] ;
    SDL_Color rgb1;
    Uint32 rgb3;
    Uint8 a, r, g, b;
#pragma omp parallel for

    for ( int j = 0;j < s->h ;++j )
    {
        for ( int i = 0;i < s->w;++i )
        {
            rgb1 = GetRGB( SpriteData[ n ], i, j );

            if ( rgb1.unused != 0 && rgb1.unused > 80 )
            {
                a = 0xff;//( rgb1.unused - rgb1.unused / 10 );
                fuzzy( &rgb1, &a, &r, &g, &b );
                rgb3 = ( 0xff << 0x18 ) + ( r << 0x10 ) + ( g << 0x8 ) + ( b ); //SDL_MapRGBA(SpriteData[ p ]->format,r,g,b,0xff/*,rgb1.unused*/);//(rgb1.unused<<0x18)+( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
            }
            else
                rgb3 = ( 0xff << 0x18 ) + colorkey;

            putpixel( s, i, j, rgb3 );
        }
    }

    SpriteData[ p ] = SDL_DisplayFormat( s );
    SDL_FreeSurface( s );
    type[ p ] = 'B';
}

void draw_line( Point *xy, Point *to_xy, int len, Uint32 color )
{
    //color=rand()%65535;
    int k = sqrt_line( xy, to_xy );
    float x, y;
#pragma omp parallel for

    for ( int i = k;i >= 0;i-- )
    {

        x = xy->x - to_xy->x < 0 ? xy->x + ( ( ( float ) i / ( float ) k ) * abs( xy->x - to_xy->x ) ) : xy->x - ( ( ( float ) i / ( float ) k ) * abs( xy->x - to_xy->x ) );

        y = xy->y - to_xy->y < 0 ? xy->y + ( ( ( float ) i / ( float ) k ) * abs( xy->y - to_xy->y ) ) : xy->y - ( ( ( float ) i / ( float ) k ) * abs( xy->y - to_xy->y ) );

        if ( x < SCREEN_WIDTH && y < SCREEN_HEIGHT && x >= 0 && y >= 0 )
        {
            for ( int j = len;j > 0;j-- )
                putpixel( screen, ( int ) ( x + j ), ( int ) y, color );
        }
    }

}

//for ( int j = 0;j < f - 1;++j )
//   {
//       for ( int i = 0;i < k - 1;++i )
//       {
//  rgb1 = GetRGB( s, i, j );
//
//  rgb1.g=((rgb1.r+rgb1.g+rgb1.b)/3);
//  //rgb1.g=rgb1.g<=0xaf?rgb1.g-rgb1.g/5:rgb1.g-rgb1.g/10;
//  rgb1.b=rgb1.r=rgb1.g;
//  A=rgb1.unused;//*(float)(( 255 - (float)rc->Alpha ) / 255);
//  a1 = ( A * rgb1.r) / 255;
//  a2 = ( A * rgb1.g) / 255;
//  a3 = ( A * rgb1.b) / 255;
//  rgb3 = (rgb1.unused<<0x18)+( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
//  putpixel( SpriteData[ p ],i,j, rgb3 );
//       }
//   }
void Convey2Color( int n, int p, int single, float fade )
{
    SpriteData[ p ] = SDL_DisplayFormat( SpriteData[ n ] );
    SDL_Color rgb1;
    Uint32 rgb3;
    Uint8 a1, a2, a3;
    int k = SpriteData[ n ] ->w, f = SpriteData[ n ] ->h;

#pragma omp parallel for

    for ( int j = 0;j < f - 1;++j )
    {
        for ( int i = 0;i < k - 1;++i )
        {
			rgb3=GetPixel ( SpriteData[ n ], i, j );
			if(rgb3!=colorkey){
				rgb1 = GetRGB( SpriteData[ n ], i, j );
				rgb1.r = single >> 2 == 1 ? rgb1.r : fade * 0xff;
				rgb1.g = ( single >> 1 ) % 2 == 1 ? rgb1.g : fade * 0xff;
				rgb1.b = single % 2 == 1 ? rgb1.b : fade * 0xff;
				//  a1 = ( A * rgb1.r) / 255;
				//  a2 = ( A * rgb1.g) / 255;
				//  a3 = ( A * rgb1.b) / 255;
				fuzzy( &rgb1, &rgb1.unused, &a1, &a2, &a3 );
				rgb3 = ( 0xff << 0x18 ) + ( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
				//rgb3 = ( rgb1.unused << 0x18 ) + ( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
			}else
				rgb3 = /*( 0xff << 0x18 ) +*/ colorkey;
            putpixel( SpriteData[ p ], i, j, rgb3 );
        }

    }
	type[p]='B';

}

void Convey2Fuzzy( int n, int p )
{
    SpriteData[ p ] = SDL_DisplayFormatAlpha( SpriteData[ n ] );
    SDL_Color rgb1;
    Uint32 a1, a2, a3, A, rgb3;
    int k = SpriteData[ p ] ->w, f = SpriteData[ p ] ->h;
    float ws, gh;
#pragma omp parallel for

    for ( int j = 0;j < f - 1;++j )
    {
        gh = 1 - ( ( float ) j / ( float ) f );
        k = SpriteData[ p ] ->w + ( SpriteData[ p ] ->w * gh ) / 3;

        for ( int i = 0;i < k - 1;++i )
        {
            ws = ( ( float ) i / ( float ) k );
            rgb1 = GetRGB( SpriteData[ n ], ws * ( SpriteData[ p ] ->w ), j /*gh*s->h*/ );
            A = rgb1.unused; //*(float)(( 255 - (float)rc->Alpha ) / 255);
            a1 = ( A * rgb1.r ) / 255;
            a2 = ( A * rgb1.g ) / 255;
            a3 = ( A * rgb1.b ) / 255;
            rgb3 = SDL_MapRGBA( SpriteData[ p ] ->format, a1, a2, a3, rgb1.unused ); //(rgb1.unused<<0x18)+( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
            putpixel( SpriteData[ p ], i, j, rgb3 );
        }


    }
}

void Distortion2( SP_TYPE *rc, int p, int position )
{
    SDL_Surface * s = SDL_DisplayFormatAlpha( SpriteData[ p ] );
    SDL_Color rgb1, rgb2;
    Uint32 a1, a2, a3, A, rgb3;
    int k = s->w, f = s->h;
    float ws, hs;
#pragma omp parallel for

    for ( int j = position;j < f - 1;++j )
    {
        for ( int i = 0;i < k - 1;++i )
        {
            bool over = ( ( s->w - i ) < SCREEN_WIDTH && rc->rcSprite.y + j - position < SCREEN_HEIGHT && rc->rcSprite.x + ( s->w - i ) >= 0 && rc->rcSprite.y + j >= 0 );
            ws = ( ( float ) i / ( float ) k );
            hs = j < 0 ? 0 : ( ( float ) j / ( float ) f );

            if ( over )
                rgb1 = GetRGB( s, ws * s->w, hs * s->h );

            if ( j > 0 && rgb1.unused != 0 && over )
            {
                A = ( float ) rgb1.unused; //*(float)(( 255 - (float)rc->Alpha ) / 255);

                rgb2 = GetRGB( screen, rc->rcSprite.x + ( i - k / 2 ), rc->rcSprite.y + j - position );

                a1 = ( A * rgb1.r + ( 255 - A ) * rgb2.r ) / 255;
                a2 = ( A * rgb1.g + ( 255 - A ) * rgb2.g ) / 255;
                a3 = ( A * rgb1.b + ( 255 - A ) * rgb2.b ) / 255;
                rgb3 = ( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
                putpixel( screen, rc->rcSprite.x + ( i - k / 2 ), rc->rcSprite.y + j - position, rgb3 );
            }

        }

        k += 1;

    }

    SDL_FreeSurface( s );
}

void Distortion( SDL_Rect *rc, int p, float angle )
{
    SDL_Surface * s = SDL_DisplayFormatAlpha( SpriteData[ p ] );
    Uint32 a1, a2, a3, a4, a5, a6, rgb1, rgb2, rgb3, A;
    Point p1[ 600 ];
    bool over;
    int b = s->w / 2 * -1;
    angle = ( ( int ) angle + 90 ) % 360;

    for ( int i = 0;i <= s->w;i++ )
    {
        p1[ i ].x = sin( angle * PI / 180.0f ) * ( float ) i;
        p1[ i ].y = cos( angle * PI / 180.0f ) * ( float ) i;
    }

#pragma omp parallel for
    for ( int j = 0;j < s->h ;++j )
        for ( int i = s->w;i > 0 ;--i )
        {
            rgb1 = GetPixel( s, i, j );
            A = ( rgb1 >> s->format->Ashift ) & 0xff;
            over = ( rc->x + p1[ i ].x < SCREEN_WIDTH && rc->y + j + p1[ i ].y < SCREEN_HEIGHT && rc->x + p1[ i ].x >= 0 && rc->y + j + p1[ i ].y >= 0 );

            if ( A != 0 && over )
            {
                A = A * 255 / 255;
                a1 = ( rgb1 >> s->format->Rshift ) & 0xff;
                a2 = ( rgb1 >> s->format->Gshift ) & 0xff;
                a3 = ( rgb1 >> s->format->Bshift ) & 0xff;
                rgb2 = GetPixel( screen, ( float ) ( rc->x + p1[ i ].x ), ( float ) ( rc->y + j + p1[ i ].y ) );
                a4 = ( rgb2 >> screen->format->Rshift ) & 0xff;
                a5 = ( rgb2 >> screen->format->Gshift ) & 0xff;
                a6 = ( rgb2 >> screen->format->Bshift ) & 0xff;
                a1 = ( A * a1 + ( 255 - A ) * a4 ) / 255;
                a2 = ( A * a2 + ( 255 - A ) * a5 ) / 255;
                a3 = ( A * a3 + ( 255 - A ) * a6 ) / 255;
                rgb3 = ( a1 << screen->format->Rshift ) + ( a2 << screen->format->Gshift ) + ( a3 << screen->format->Bshift );

                if ( ( colorkey != rgb3 || type[ p ] == 'P' ) )
                    putpixel( screen, ( float ) ( rc->x + p1[ i ].x ), ( float ) ( rc->y + j + p1[ i ].y ) , rgb3 );
            }

        }

    SDL_FreeSurface( s );
}

int fuzzy( SDL_Color *rgb, Uint8 *a, Uint8 *r, Uint8 *g, Uint8 *b )
{
    *r = ( *a * rgb->r / 255 );
    *g = ( *a * rgb->g / 255 );
    *b = ( *a * rgb->b / 255 );
    return 1;
}

int sqrt_line( Point *a, Point *b )
{
    return sqrt( abs( pow( (float)a->x - (float)b->x, 2 ) ) + abs( pow( (float)a->y - (float)b->y, 2 ) ) );
}

Uint32 GetPixel ( SDL_Surface* pSurface , int x , int y )
{
    //SDL_Color color ;
    Uint32 col = 0 ;

    //determine position
    char* pPosition = ( char* ) pSurface->pixels ;

    //offset by y
    pPosition += ( pSurface->pitch * y ) ;

    //offset by x
    pPosition += ( pSurface->format->BytesPerPixel * x ) ;

    //copy pixel data
    memcpy ( &col , pPosition , pSurface->format->BytesPerPixel ) ;

    //convert color
    return ( col ) ;

}

SDL_Color GetRGB2 ( SDL_Surface* pSurface , int x , int y )
{
    SDL_Color color ;
    Uint32 col = 0 ;
    //    int r, g, b;
    //determine position
    char* pPosition = ( char* ) pSurface->pixels ;

    //offset by y
    pPosition += ( pSurface->pitch * y ) ;

    //offset by x
    pPosition += ( pSurface->format->BytesPerPixel * x ) ;

    //copy pixel data
    memcpy ( &col , pPosition , pSurface->format->BytesPerPixel ) ;

    //convert color
    SDL_GetRGB ( col , pSurface->format , &color.r , &color.g , &color.b ) ;

    return ( color ) ;

}
SDL_Color GetRGB ( SDL_Surface* pSurface , int x , int y )
{
    SDL_Color color ;
    Uint32 col = 0 ;
    //    int r, g, b;
    //determine position
    char* pPosition = ( char* ) pSurface->pixels ;

    //offset by y
    pPosition += ( pSurface->pitch * y ) ;

    //offset by x
    pPosition += ( pSurface->format->BytesPerPixel * x ) ;

    //copy pixel data
    memcpy ( &col , pPosition , pSurface->format->BytesPerPixel ) ;

    //convert color
    SDL_GetRGBA ( col , pSurface->format , &color.r , &color.g , &color.b , &color.unused ) ;

    return ( color ) ;

}

void putpixel( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = ( Uint8 * ) surface->pixels + y * surface->pitch + x * bpp;

    switch ( bpp )
    {
    case 1:
        *p = pixel;
        break;

    case 2:
        *( Uint16 * ) p = pixel;
        break;

    case 3:

        if ( SDL_BYTEORDER == SDL_BIG_ENDIAN )
        {
            p[ 0 ] = ( pixel >> 16 ) & 0xff;
            p[ 1 ] = ( pixel >> 8 ) & 0xff;
            p[ 2 ] = pixel & 0xff;
        }
        else
        {
            p[ 0 ] = pixel & 0xff;
            p[ 1 ] = ( pixel >> 8 ) & 0xff;
            p[ 2 ] = ( pixel >> 16 ) & 0xff;
        }

        break;

    case 4:
        *( Uint32 * ) p = pixel;
        break;
    }
}

void BlitSurface( SDL_Rect *rc )
{

	if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) return;
    }
	SDL_Rect buffer_rect={0};
	SDL_SetClipRect( spriteIndex, &buffer_rect );
	buffer_rect=spriteIndex->clip_rect;

	//rc->w=abs(screen->clip_rect.w-abs(rc->x))<spriteIndex->w?abs(screen->clip_rect.w-abs(rc->x)):spriteIndex->w;
	//buffer_rect.w=rc->w;
	buffer_rect.x=abs(screen->clip_rect.x-rc->x);

	//rc->h=abs(screen->clip_rect.h-abs(rc->y))<spriteIndex->h?abs(screen->clip_rect.h-abs(rc->y)):spriteIndex->h;
	//buffer_rect.h=rc->h;
	buffer_rect.y=abs(screen->clip_rect.h-rc->y);

	SDL_SetClipRect( spriteIndex, &buffer_rect );
    SDL_BlitSurface( spriteIndex, NULL, tempSurface, rc );

	if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }

	//SDL_UpdateRect( screen, rc->x , rc->y , spriteIndex->w, spriteIndex->h );
    //SDL_FreeSurface( spriteIndex );

    //Page_Count();
}

void Set_Buffer( BUFFER_MANAGER b )
{
    SDL_Rect buffer_rect; //={w:,h:,x:0,y:0};
    SDL_Surface *scr;
    scr = screen;

    switch ( b )
    {
    case MAIN_BUFFER:
        buffer_rect.w = SCREEN_WIDTH;
        buffer_rect.h = SCREEN_HEIGHT;
        buffer_rect.x = 0;
        buffer_rect.y = 0;

        break;
    case LEFT_BUFFER:
        buffer_rect.w = SCREEN_WIDTH / 2;
        buffer_rect.h = SCREEN_HEIGHT;
        buffer_rect.x = 0;
        buffer_rect.y = 0;
        //scr = screen_buffer[ 0 ];
        break;
    case RIGHT_BUFFER:
        buffer_rect.w = SCREEN_WIDTH / 2;
        buffer_rect.h = SCREEN_HEIGHT;
        buffer_rect.x = SCREEN_WIDTH / 2;
        buffer_rect.y = 0;
        //scr = screen_buffer[ 1 ];
        break;
    }

    SDL_SetClipRect( scr, &buffer_rect );
    tempSurface = scr;
}

void Print_Double_Buffer()
{

 //   PAGE = 1;

//	SDL_LockSurface(screen);
        

	Set_Buffer( MAIN_BUFFER );

	SDL_Flip(screen);
	//SDL_UpdateRect( screen, 0 , 0 , 0, 0 );

//	SDL_UnlockSurface(screen);



 //   /* Show, baby, show! */

}

void Print()
{
    SDL_UpdateRect( screen, 0 , 0, 0, 0 );
    //SDL_Delay( 30 );
}

void Page_Count()
{
    PAGE = PAGE / LIMIT < LAYER ? PAGE + 1 : LAYER;
    //spriteIndex=sprite[ PAGE / LIMIT ];
}

bool hitTest( SDL_Rect *rc1, SDL_Rect *rc2, int x_range, int y_range )
{
    return ( abs( rc1->x - rc2->x ) < x_range && abs( rc1->y - rc2->y ) < y_range );
}

int Sound_Init()
{
    const int TMP_FREQ = MIX_DEFAULT_FREQUENCY;
    const Uint16 TMP_FORMAT = MIX_DEFAULT_FORMAT;
    const int TMP_CHAN = 2;
    const int TMP_CHUNK_SIZE = 512;

    return Mix_OpenAudio( TMP_FREQ, TMP_FORMAT, TMP_CHAN, TMP_CHUNK_SIZE );
}

int Sound_Play( char *i )
{
    Mix_FreeMusic( mix_music );
    mix_music = Mix_LoadMUS( i );
    Mix_PlayMusic( mix_music, -1 );
    return 0;
}

void Play( char * f )
{
    Mix_FreeChunk( Chunk[ ChunkIndex ] );
    Chunk[ ChunkIndex ] = Mix_LoadWAV( f );
    Mix_PlayChannel( -1, Chunk[ ChunkIndex ] , 0 );
    ( ++ChunkIndex ) %= SOUND_CHUNK;
}
void Play_Chunk( char * f , int Chunks)
{
	if(Chunks>SOUND_CHUNK-1)
		return;
    Mix_FreeChunk( Chunk[ Chunks ] );
    Chunk[ Chunks ] = Mix_LoadWAV( f );
    Mix_PlayChannel( -1, Chunk[ Chunks ] , 0 );
    //( ++ChunkIndex ) %= SOUND_CHUNK;
}
void FreeChunk()
{
    for ( int i = 0;i < SOUND_CHUNK;i++ )
    {
        Mix_FreeChunk( Chunk[ i ] );
    }

    Mix_FreeMusic( mix_music );
}

void Load_Rom( int site, int size, int index , char*rom )
{
	PIC = index;
    char * buffer = ( char * ) malloc( size );
    SDL_Surface *sprite;
    memcpy( ( char * ) buffer, ( char * ) ( rom + site ), size );
    /*for ( int i = 0;i < size;i++ )buffer[ i ] = rom[ site + i ];*/
    type[ index ] = buffer[ 1 ] == 'P' ? 'P' : 'B';
    sprite = IMG_Load_RW( SDL_RWFromMem( buffer, size ), 0 );
    SpriteData[ index ] = type[ index ] == 'P' ? SDL_DisplayFormatAlpha( sprite ) : SDL_DisplayFormat( sprite ) ;
    SDL_FreeSurface( sprite );
    free( buffer );
}
SDL_Surface * Load_Rom_PNG( int site, int size, char*rom)
{
	//PIC = index;
    char * buffer = ( char * ) malloc( size );
    SDL_Surface *sprite;
	SDL_Surface *temp_sprite;
    memcpy( ( char * ) buffer, ( char * ) ( rom + site ), size );
    /*for ( int i = 0;i < size;i++ )buffer[ i ] = rom[ site + i ];*/
    //*type = buffer[ 1 ] == 'P' ? 'P' : 'B';
    sprite = IMG_Load_RW( SDL_RWFromMem( buffer, size ), 0 );
    temp_sprite = SDL_DisplayFormatAlpha( sprite );
    SDL_FreeSurface( sprite );
    free( buffer );
	return temp_sprite;
}

void Load_Rom_Sound( int site, int size, int index )
{
    /*for ( int i = 0;i < size;i++ )
    {
        buffer[ i ] = rom[ site + i ];
    }

    Sound[ index ] = SDL_RWFromMem( &buffer, size );*/
}

void DrawFont( FONT EFONT, int x, int y , Uint8 Alpha )
{
    if ( EFONT >= glNULL )
        return ;

    OBJ E_FONT( x -SpriteData[ FONT_MEMBER + EFONT ] ->w / 2, y - SpriteData[ FONT_MEMBER + EFONT ] ->h / 2, EFONT, FONT_MEMBER, FONT_ALPHA_MEMBER );

    E_FONT.showFlag = true;

    //E_FONT.Alpha = Alpha;

    E_FONT.draw();
}

OBJ::OBJ()
{
    init( 0, 0, 0, 0 );
}

//OBJ::OBJ( int X = 0, int Y = 0, int Frame = 0, unsigned int Frame_Index = 0 )
//{
//    init( X, Y, Frame, Frame_Index );
// types=BMP;
//}
OBJ::OBJ( int X = 0, int Y = 0, int Frame = 0, unsigned int Frame_Index = 0 , int Alpha_Index = 0 )
{
    init( X, Y, Frame, Frame_Index );

    if ( type[ Frame_Index ] == 'P' )
    {
        this->AlphaIndex = Alpha_Index;
        types = PNG;
    }
    else
        types = BMP;
}

OBJ::~OBJ()
{
    //delete [] this->group;
}

void OBJ::init( int X = 0, int Y = 0, int Frame = 0, unsigned int Seed = 0 )
{
    this->x = X;
    this->y = Y;
    this->fade = 0;
    this->inc = 1;
    this->Alpha = 0;
    this->Angle = 0;
    this->Zoom = 0;
    this->Shift = STOP;
    this->Mirror = false;
    this->frame = Frame;
    this->frameIndex = Seed;
    this->showFlag = false;
    this->setBuffer( MAIN_BUFFER );
    memset( &this->group, 0, sizeof( this->group ) );
}

unsigned int OBJ::setPng_AlphaFrame()
{
    if ( this->group.AlphaFlag == true )
        return this->AlphaIndex;
    else
        return this->frameIndex;
}

void OBJ::setBuffer( BUFFER_MANAGER b )
{
    this->buffer = b;
}

void OBJ::draw()
{
    bool effect = false;
    int Draws = 0 ;
    this->group.rcSprite.x = x;
    this->group.rcSprite.y = y;

    Set_Buffer( this->buffer );

    if ( this->buffer == RIGHT_BUFFER ) this->group.rcSprite.x += SCREEN_WIDTH/2;

    if ( showFlag )
    {
        effect = ( setAlpha() | setAngle() | setZoom() | setMirror() );
        Draws = this->frame + this->frameIndex;

        if ( effect )
        {
            if ( this->types == PNG )
            {
				if(this->setPng_AlphaFrame()!=-1){
					Draws = this->setPng_AlphaFrame() + this->frame;
					drawSpritePng( &this->group, Draws , false);
				}else
				{
					Draws = this->frame + this->frameIndex;
					drawSpritePng( &this->group, Draws , true);
				}
            }
            else if ( this->types == BMP )
            {
                Draws = this->frame + this->frameIndex;
                drawSpriteBmp( &this->group, Draws );
            }
        }
        else
        {
            if ( this->types == PNG )
            {
                drawSprite_fullcolor( &this->group.rcSprite, Draws );
            }
            else if ( this->types == BMP )
            {
                drawSprite( &this->group.rcSprite, Draws );
            }
        }
    }
}

float OBJ::setFade( bool release, float base, int max )
{
    float shift;

    if ( release ) this->fade = 0;

    switch ( Shift )
    {

    case AUTO:
        this->inc = ( fade >= 100 && inc > 0 ) || ( fade <= 1 && inc < 0 ) ? -inc : inc;
        shift = inc;
        break;

    case IN:
        this->inc = -1;
        shift = ( fade <= 1 ) ? 0 : inc;
        break;

    case OUT:
        this->inc = 1;
        shift = ( fade >= 100 ) ? 0 : inc;
        break;

    case STOP:

        shift = 0;

        break;
    }

    this->fade += shift * base;
    this->fade = this->fade <= 1 ? 1 : this->fade;
    this->fade = this->fade >= 100 ? 100 : this->fade;
    return this->fade / 100*max;
}

bool OBJ::setMirror()
{
    this->group.MirrorFlag = Mirror;
    return this-> group.MirrorFlag;
}

bool OBJ::setAlpha()
{
    this->group.Alpha = this->Alpha;
    this->group.AlphaFlag = ( this->Alpha != 0 );
    return this->group.AlphaFlag;
}

bool OBJ::setAngle()
{
    this->group.Rotataion = this->Angle;
    this->group.RotataionFlag = ( this->Angle != 0 );
    return this->group.RotataionFlag;
}

bool OBJ::setZoom()
{
    this->group.Zoom = this->Zoom;
    this->group.ZoomFlag = ( this->Zoom != 0 );
    return this->group.ZoomFlag;

}
