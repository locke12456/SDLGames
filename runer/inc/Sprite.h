#include "SDL.h"
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "SDL_mixer.h"
#include <string>
using namespace std;
class Sprite
{
public:
	Sprite();
	//Sprite(int Amount,int Index);
	void Catch();
	void load(int Amount,int Index);
	void load_Particle(int Amount,int Index,int color,int fade);
	void addColor(int Single,float Color);
	void addColor_Mirror(int Single,float Color);
	const int *Amount;
	const int *Alpha_Index;
	const int *Frame_Index;
	const int *Mirror_Index;
	const int *Mirror_Alpha_Index;
	const int *Color[10];
	const int *Color_Mirror[10];
	const int *Freme_Amount;
	const int *Total;
	string Type;
	//void draw();
private:
	bool loaded;
	int DATA;
	int ALPHA;
	int MIRROR;
	int MIRROR_ALPHA;
	int COLOR[10];
	int COLOR_MIRROR[10];
	int TOTAL_COLOR;
	int TOTAL_COLOR_MIRROR;
	int AMOUNT;
	int TOTAL;
	
	void init();
	
	//SDL_Surface *MIRROR_ALPHA;
};
SDL_Surface* Sprite_Convey2Lowbilt(SDL_Surface* Sprite, int color ,int fade ,int colorkey);
SDL_Surface* Sprite_Convey2Mirror( SDL_Surface* Sprite ,string type);
void Sprite_Convey2Lowbilt( int n, int p ,int color ,int fade);