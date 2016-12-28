//#include "string"

#include "lstring.h"
//using namespace std;
#define ANIMATION_KEYFRAME 15
#define ANIMATION_FRAME 32
typedef struct
{
	int thisFrame;
	int lastFrame;
	const char * Name;
	int rate;
	int next;
	int pre;
}FRAME;
typedef struct
{
	int Rate;
	int Anima[ANIMATION_FRAME];
	char Name[STRING_LEN];
}ANIMA;
typedef struct
{
	int Rate;
	int Anima[ANIMATION_FRAME];
	const char * Name;
}ANIMA2;

typedef struct{
	int x,y;
	ANIMA anima[ANIMATION_KEYFRAME];
}PLAYER_Table;
typedef struct{
	int x,y;
	ANIMA2 anima[ANIMATION_KEYFRAME];
}PLAYER_Table2;
class Animation
{
public:
	Animation();
	Animation(const PLAYER_Table * table,int rate);
	void Speed(int speed,float speed_m);
	int Play(LString &str);
	int Play(const char *str);
	bool isPlay();
private:
	bool checkAnima(LString &str);
	bool checkAnima(const char *str);
	bool Playing;
	void init();
	void Next();
	void Previous();
	void setFrameRate(int rate);
	void initFrame();
	const ANIMA * Search_by_Name(LString str);
	const PLAYER_Table * anima_table;
	const ANIMA * Anima;
	FRAME Frame;
	float Speed_m;
	int Search_Last();
	int Search(int search);
	int FrameRate;
	int FrameSpeed;
	int thisFrame;
	int nextFrame();
	int preFrame();
};