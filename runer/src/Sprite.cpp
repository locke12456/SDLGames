#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "UsrFuc.h"
#include "Sprite.h"

extern int colorkey;
extern char type[];
extern SDL_Surface * SpriteData[];
/*		Public		*/
Sprite::Sprite()
{
	init();
}
void Sprite::Catch()
{
	Amount=&AMOUNT;
	Alpha_Index=&ALPHA;
	Frame_Index=&DATA;
	Mirror_Index=&MIRROR;
	Mirror_Alpha_Index=&MIRROR_ALPHA;
	Freme_Amount=&AMOUNT;
	for(int i=0;i<TOTAL_COLOR;i++){
		Color[i]=&COLOR[i];
		Color_Mirror[i]=&COLOR_MIRROR[i];
	}
	Total=&TOTAL;
}
void Sprite::addColor(int Single,float Color)
{
	if(!loaded)
		return;
	SDL_Surface *temp;
	COLOR[TOTAL_COLOR]=DATA+TOTAL;
	for(int k=0;k<AMOUNT;k++){
		Convey2Color(ALPHA + k ,COLOR[TOTAL_COLOR] + k,Single,Color);
	}
	TOTAL_COLOR++;
	TOTAL+=AMOUNT;
}
void Sprite::addColor_Mirror(int Single,float Color)
{
	if(!loaded)
		return;
	COLOR_MIRROR[TOTAL_COLOR_MIRROR]=DATA+TOTAL;
	for(int k=0;k<AMOUNT;k++){
		Convey2Color(MIRROR_ALPHA + k ,COLOR_MIRROR[TOTAL_COLOR_MIRROR] + k,Single,Color);
	}
	TOTAL_COLOR_MIRROR++;
	TOTAL+=AMOUNT;
}
void Sprite::load(int Amount,int Index)
{
	if(loaded)
		return;
	int i=Amount;	

	DATA=Index;

	MIRROR=Index+i;
	
	AMOUNT=Amount;
	for(int k=0;k<Amount;k++)
		Convey2Mirror(Index + k ,MIRROR + k);
	
	if(type[Index]=='P')
	{
		i+=Amount;
		
		ALPHA=Index+i;
		
		for(int k=0;k<Amount;k++)
			Convey2Lowbilt(Index + k , ALPHA + k);

		i+=Amount;

		MIRROR_ALPHA=Index+i;

		for(int k=0;k<Amount;k++)
			Convey2Mirror(ALPHA + k , MIRROR_ALPHA + k);

	}else
	{
		ALPHA=DATA;

		MIRROR_ALPHA=MIRROR;
	}
	TOTAL=i+Amount;
	loaded=true;
	printf("%d\n",TOTAL+DATA);
}
void Sprite::load_Particle(int Amount,int Index,int color,int fade)
{
	if(loaded)
		return;
	int i=Amount;	

	DATA=Index;

	MIRROR=Index+i;
	
	AMOUNT=Amount;
	for(int k=0;k<Amount;k++)
		Convey2Mirror(Index + k ,MIRROR + k);
	
	if(type[Index]=='P')
	{
		i+=Amount;
		
		ALPHA=Index+i;
		
		for(int k=0;k<Amount;k++)
			Sprite_Convey2Lowbilt(Index + k , ALPHA + k,color,fade);

		i+=Amount;

		MIRROR_ALPHA=Index+i;

		for(int k=0;k<Amount;k++)
			Convey2Mirror(ALPHA + k , MIRROR_ALPHA + k);

	}else
	{
		ALPHA=DATA;

		MIRROR_ALPHA=MIRROR;
	}
	TOTAL=i+Amount;
	loaded=true;
	printf("%d\n",TOTAL+DATA);
}
/*		Private		*/
void Sprite::init()
{
	memset(this,0,sizeof(Sprite));
}
/** 
* Srite :����-(SDL_Surface *)SDL�Ϥ����hStruct�A�ﹳ�Ϥ�(�Q�নBMP����) 
* type :�u��PNG�Τ��OPNG��ءA�����쪺�b�ۤv�X�R�a
*/
SDL_Surface* Sprite_Convey2Mirror( SDL_Surface* Sprite ,string type)
{
    SDL_Surface* Mirror;
    if ( type == "PNG" )
        Mirror = SDL_DisplayFormatAlpha( Sprite );
    else
        Mirror = SDL_DisplayFormat( Sprite );

    SDL_Color rgb1;

    Uint32 rgb3;

#pragma omp parallel for
    for ( int j = 0;j < Mirror ->h - 1;++j )
    {
        for ( int i = 0;i < Mirror ->w - 1;++i )
        {
            rgb1 = GetRGB( Sprite, i, j );

            if ( type == "PNG" )
                rgb3 = SDL_MapRGBA( Sprite ->format, rgb1.r, rgb1.g, rgb1.b, rgb1.unused ); //���o�ؼ��C��RGBA�A�]�talpha
            else
                rgb3 = SDL_MapRGB( Sprite ->format, rgb1.r, rgb1.g, rgb1.b );//���o�ؼ��C��A���]�talpha

            putpixel( Mirror, Sprite ->w - i, j, rgb3 ); //�o�̴N�O���Function�����I�A�Ϥ�V���C�⼳�^�h�A�o�Ө禡����ڦA����
        }
    }
	return Mirror;
}
/** 
* Srite :����-(SDL_Surface *)SDL�Ϥ����hStruct�A�ﹳ�Ϥ�(�Q�নBMP����) 
* color :��i�Ϥ����C�� 0 ~ 0xff , -1������,PS:�p�G�Ϥ����O���г]��-1 
* fade :0xff�A�N�������h�ĪG
* **/
SDL_Surface* Sprite_Convey2Lowbilt(SDL_Surface* Sprite, int color ,int fade ,int colorkey)
{
    SDL_Surface * temp = SDL_DisplayFormatAlpha( Sprite ); //�����ƻs�@���@�˪��Ϥ�
    SDL_Color rgb1;
    Uint32 rgb3;
    Uint8 a, r, g, b;
#pragma omp parallel for

    for ( int j = 0;j < temp->h ;++j )
    {
        for ( int i = 0;i < temp->w;++i )
        {
            rgb1 = GetRGB( Sprite, i, j );

            if ( rgb1.unused != 0 && rgb1.unused > 64)  // rgb1.unused �O�z���� 0 ~ 0xff �o�̥u������W�L�@�b�~����b�z���V��
            {
			a = fade == 0xff ? fade : ( rgb1.unused - rgb1.unused / 10 ) ;
				if(color!=-1)
				{
					rgb3 = ( 0xff << 0x18 ) + color;
				}else
				{
					fuzzy( &rgb1, &a, &r, &g, &b );//���o�C��禡�A�o�Ӳ[���ڦb�t�~�ѻ�
					rgb3 = ( 0xff << 0x18 ) + ( r << 0x10 ) + ( g << 0x8 ) + ( b );
				}
			}
            else
                rgb3 = ( 0xff << 0x18 ) + colorkey;//�p�G�z���׶W�L�A�h�[�Jcolorkey���C��

            putpixel( temp, i, j, rgb3 );//��rgb�s����w���I�A�o�Ӳ[���ڷ|�b�t�~�ѻ�
        }
    }
    temp = SDL_DisplayFormat( temp );  //�̫�AFormat�@���A�Y�j�\�i��
    return temp;
}
void Sprite_Convey2Lowbilt( int n, int p ,int color ,int fade)
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

            if ( rgb1.unused != 0 && rgb1.unused > 64)
            {
				a = fade==0xff?fade:( rgb1.unused - rgb1.unused / 10 );
				if(color!=-1){
					//fuzzy( &rgb1, &a, &r, &g, &b );
					rgb3 = ( 0xff << 0x18 ) + color;//( r << 0x10 ) + ( g << 0x8 ) + ( b ); //SDL_MapRGBA(SpriteData[ p ]->format,r,g,b,0xff/*,rgb1.unused*/);//(rgb1.unused<<0x18)+( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
				}else
				{
					fuzzy( &rgb1, &a, &r, &g, &b );
					rgb3 = ( 0xff << 0x18 ) + ( r << 0x10 ) + ( g << 0x8 ) + ( b ); //SDL_MapRGBA(SpriteData[ p ]->format,r,g,b,0xff/*,rgb1.unused*/);//(rgb1.unused<<0x18)+( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
				}
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
//}
//SDL_Surface* Sprite_Convey2Lowbilt( SDL_Surface* Sprite) 
//{
//    SDL_Surface * s = SDL_DisplayFormatAlpha( Sprite );
//    SDL_Surface * Alpha;
//    SDL_Color rgb1;
//    Uint32 rgb3;
//    Uint8 a, r, g, b;
//#pragma omp parallel for
//
//    for ( int j = 0;j < s->h ;++j )
//    {
//        for ( int i = 0;i < s->w;++i )
//        {
//            rgb1 = GetRGB( Sprite, i, j );
//
//            if ( rgb1.unused != 0 && rgb1.unused > 80 )
//            {
//                a = ( rgb1.unused - rgb1.unused / 10 );
//                fuzzy( &rgb1, &a, &r, &g, &b );
//                rgb3 = ( 0xff << 0x18 ) + ( r << 0x10 ) + ( g << 0x8 ) + ( b ); //SDL_MapRGBA(SpriteData[ p ]->format,r,g,b,0xff/*,rgb1.unused*/);//(rgb1.unused<<0x18)+( a1 << 0x10 ) + ( a2 << 0x8 ) + ( a3 );
//            }
//            else
//                rgb3 = ( 0xff << 0x18 ) + colorkey;
//
//            putpixel( s, i, j, rgb3 );
//        }
//    }
//
//    Alpha = SDL_DisplayFormat( s );
//    SDL_FreeSurface( s );
//    return Alpha;
//}
