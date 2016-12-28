#include "SDL.h"
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "SDL_mixer.h"
#include "math.h"
#include "effects.h"
#include "UsrFuc.h"
extern SDL_Surface *SpriteData[];
extern SDL_Surface * screen, *sprite[];
extern SDL_RWops *Sound[];
extern int colorkey;
extern int PAGE;
extern char type[];
ELF::ELF(int page)
{
	this->temp=SDL_DisplayFormatAlpha( SpriteData[ page ] );
	this->Len=this->temp->h*this->temp->w;
	this->xy=(POINT*)malloc(sizeof(POINT)*this->Len);
	for(int i=0;i<this->Len;i++)
	{
		xy[i].x=i%2?rand()%this->temp->w*2:rand()%this->temp->w*-2;
		xy[i].y=i%2?rand()%this->temp->h*2:rand()%this->temp->h*-2;
	}
}
void ELF::combine()
{

}
void ELF::Release()
{
	SDL_FreeSurface( temp );
	free(xy);
}