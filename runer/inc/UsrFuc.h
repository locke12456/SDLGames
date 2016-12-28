/**************************************************************
Author : Locke

**************************************************************/

#define SCREEN_WIDTH	1440
#define SCREEN_HEIGHT	900
#define TOP_HEIGHT		800
#define SUB_HEIGHT		0
#define SPRITE_SIZE		2128
#define FONT_MEMBER		2037
#define FONT_ALPHA_MEMBER		339
#define SOUND_CHUNK 32
#define PI				3.1415926535897932384626433832795

/**************************************************************
 Struct SP_TYPE
 This struct is for effect setting,
 if would like setting position or width and height,user could used 'rcSprite'
 and Zoom was control Zoom in or Zoom out,
 ex: if you want Zoom in rate 1.5 or more than
		Zoom = 1.5
	 Zoom out
 **************************************************************/

typedef struct{
	SDL_Rect rcSprite;	/* 設置XY */
	bool RotataionFlag; /* 旋轉旗標，欲使其旋轉則設為TRUE，反之則不做更動 */
	bool ZoomFlag;		/* 縮放旗標 */
	bool AlphaFlag;		/* 透明旗標 */
	bool MirrorFlag;	/* 鏡射 */
	float Rotataion;	/* 旋轉角度，1~360 */
	float Zoom;			/* 縮放，放大設其大於1，縮小則為小於1至0 */
	Uint8 Alpha;		/* 值為，不透明0	完全透明255 */
}SP_TYPE;
//enum ANIMA
//{
//	AUTO,NEXT,PRE,STOP/* AUTO:自動播放  NEXT:下一張  PRE:上一張  STOP:停止 */
//};
enum FADE
{
	AUTO,IN,OUT,STOP/* AUTO:自動播放  IN:遞減  OUT:遞增  STOP:停止 */
};
enum SPRITE_TYPE
{
	NOTYPE,BMP,PNG,TGA,JPG
};
enum FONT
{
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,glNULL
};
enum BUFFER_MANAGER
{
	MAIN_BUFFER,LEFT_BUFFER,RIGHT_BUFFER
};
typedef struct
{
    FONT graphic;
    int x;
    int y;
}Font;
typedef struct
{
    float x, y;
}fPoint;
typedef struct
{
    int x, y;
}Point;
typedef struct
{
    SDL_Rect XY;
	fPoint gpoint;
    unsigned int frame;
    unsigned int Kind;
    int Status;
    bool Left;
    bool showFlag;
	float angle;

}OBJECT;
typedef struct
{
	SDL_Rect XY;
	unsigned int frame[10];
	Uint8 alpha;
	bool showFlag;
}ITEAM;

class OBJ
{
	/*	使用方法

		OBJ XXX=OBJ(起始X值,Y值,目標頁,起始頁);
		繪圖頁=起始頁(不變)+目標頁(使用者自訂)
												*/
public:
	OBJ();
	//OBJ( int X , int Y, int Frame , unsigned int Frame_Index );							//
	OBJ( int X , int Y, int Frame , unsigned int Frame_Index ,int Alpha_Index );	// Alpha_Index >0 for png Alpha_Index = -1 for bitmap
	~OBJ();
	void	setBuffer(BUFFER_MANAGER b);
	void	draw();						/* 繪圖指令									*/
	float	setFade(bool release,float base,int max);

	FADE	Shift;
	float	fade;
	int		x;
	int		y;
	int		frame;						/* 目標頁									*/

	Uint8	Alpha;						/* 透明度	0~255 不透明:0，完全透明:255	*/
	float	Angle;						/* 旋轉角度 0~360							*/
	float	Zoom;						/* 縮放尺寸 0~2	  注意，超過2倍會影響效能	*/
	bool	Mirror;						/* 鏡射旗標									*/
	bool	showFlag;					/* 繪圖旗標，若showFlag為false則不繪圖		*/

private:
	BUFFER_MANAGER buffer;
	SPRITE_TYPE types;
	SP_TYPE group;
	float	inc;
    unsigned int frameIndex;			/* 起始頁									*/
	unsigned int AlphaIndex;			/* PNG透明頁								*/
	unsigned int setPng_AlphaFrame();
	void	init(int,int,int,unsigned int);
	bool	setAlpha();
	bool	setAngle();
	bool	setZoom();
	bool	setMirror();
};
void Load_Rom(int,int,int,char*);	//////////////////////////////////////////////////////
									/*	Load_Rom( RES_XX + PAGE_XX[ XX ], XX[  ], XX )
										RES_XX	在rom.h			rom的起始位置
										PAGE_XX	在page.h		為圖片群組的起始位置
										XX[]	在Spritemap.h	讀入哪張圖片
										XX		陣列位置
																						*/
									//////////////////////////////////////////////////////
SDL_Surface * Load_Rom_PNG( int site, int size, char*rom);
void Load_Rom_Sound(int,int,int);	/* no use */
void Print();
void Print_Double_Buffer();
int fuzzy(SDL_Color *rgb,Uint8 *a,Uint8 *r,Uint8 *g, Uint8 *b);//混色
void drawSpriteBmp(SP_TYPE *,int);
void drawSpritePng( SP_TYPE *rc, int p ,bool pngalpha);
void drawSprite(SDL_Rect *,int);
void drawSprite_fullcolor(SDL_Rect *,int);
void PNG_ALPHA_Lowspeed( SP_TYPE *rc );
void Play(char *);
void Play_Chunk( char * f , int Chunks);
void FreeChunk();
void draw_line(Point *xy,Point *to_xy,int len,Uint32 color);
void Distortion(SDL_Rect *,int ,float );	//2D扭曲
void Distortion2( SP_TYPE *,int ,int);		//2.5D扭曲
void Convey2Mirror(int n,int p);			//鏡射轉換
void Convey2Lowbilt(int n,int p);			//低畫質轉換
void Convey2black(int n,int p);				//全彩->黑白轉換
void Convey2Fuzzy(int n, int p);
void Convey2Color(int n,int p,int signal,float fade);

/*
			signal:		1	~	7
		-------------------------------------
		|	r	|	g	|	b	|	signal	|
		|-----------------------------------|
		|	0	|	0	|	1	|		1	|
		|	0	|	1	|	0	|		2	|
		|	0	|	1	|	1	|		3	|
		|	1	|	0	|	0	|		4	|
		|	1	|	0	|	1	|		5	|
		|	1	|	1	|	0	|		6	|
		|	1	|	1	|	1	|		7	|
		-------------------------------------
*/
//void Convey2Red(int n,int p,float fade);
//void Convey2Green(int n,int p,float fade);
//void Convey2Blue(int n,int p,float fade);
bool hitTest(SDL_Rect *,SDL_Rect *,int,int);
void FADE_to_Black( float );	/* 漸暗效果 0~1		1為全黑 */
void FADE_to_White( float );	/* 漸亮效果 0~1		1為全白 */
void DrawFont( FONT EFONT, int x, int y ,Uint8 Alpha);
void Set_Buffer(BUFFER_MANAGER b);
int Sound_Init();
int Sound_Play(char*);
SDL_Color GetRGB ( SDL_Surface* pSurface , int x , int y );
SDL_Color GetRGB2 ( SDL_Surface* pSurface , int x , int y );
void putpixel( SDL_Surface *surface, int x, int y, Uint32 pixel );
//SDL_Surface* Sprite_Convey2Mirror( SDL_Surface* Sprite ,string type);
