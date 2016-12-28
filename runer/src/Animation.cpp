#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "UsrFuc.h"
#include "Animation.h"
/*		Public		*/
Animation::Animation()
{
	init();
}
Animation::Animation(const PLAYER_Table *table, int rate)
{
	init();
	anima_table=table;
	setFrameRate(rate);
	Frame.Name=(const char *)anima_table->anima[0].Name;
	Anima=Search_by_Name((LString)Frame.Name);
	initFrame();
}
void Animation::Speed(int speed,float speed_m)
{
	FrameSpeed=speed;
	Speed_m=speed_m;
}
int Animation::Play(LString &str)
{
	if(checkAnima(str))
		initFrame();
	return Anima->Anima[nextFrame()]-1;
}
int Animation::Play(const char *str)
{
	if(checkAnima(str))
		initFrame();
	return Anima->Anima[nextFrame()]-1;
}
bool Animation::isPlay()
{
	return Playing;
}
/*		Private		*/
void Animation::init()
{
	memset(this,0,sizeof(Animation));
}
void Animation::setFrameRate(int rate)
{
	FrameRate=rate;
}
void Animation::Next()
{
	Frame.thisFrame=Frame.next;
	Frame.next=Frame.thisFrame+1<Frame.lastFrame?Frame.next+1:0;
	Frame.pre=Frame.thisFrame-1>0?Frame.thisFrame-1:Frame.lastFrame-1;
}
void Animation::Previous()
{
	Frame.thisFrame=Frame.pre;
	Frame.next=Frame.thisFrame+1<Frame.lastFrame?Frame.next+1:0;
	Frame.pre=Frame.thisFrame-1>0?Frame.thisFrame-1:Frame.lastFrame-1;
}
void Animation::initFrame()
{
	int frame=0;
	memset(&Frame,0,sizeof(Frame));
	Frame.Name=(char *)Anima->Name;
	Frame.rate=Anima->Rate;
	Frame.lastFrame=Search_Last();
}
const ANIMA * Animation::Search_by_Name(LString str)
{
	const ANIMA * ani;
	for(int i=0;i<ANIMATION_KEYFRAME;i++){
		if(anima_table->anima[i].Name==str){
			ani = &anima_table->anima[i];
			break;
		}
	}
	return ani;
}
int Animation::nextFrame()
{
	if(Anima->Anima[Frame.thisFrame]==0xff){
		Playing=false;	
		return Frame.thisFrame-1;
	}
	Playing=true;	
	float frame_m=0;
	int frame_c=0;
	int frame_s=0;
	int frame=0;
	switch(Frame.rate)
	{
	case -1:
		frame_m= Speed_m*FrameSpeed;
		frame_c = frame_m + 1;
		frame_s =(Frame.lastFrame*(FrameSpeed+1))/2;
		frame=((thisFrame/(frame_s/frame_c)))%Frame.lastFrame;
		(++thisFrame);//%=(int)(((FrameSpeed+1)*(Frame.lastFrame*FrameSpeed))/frame_c);
		break;
	case 0:
		frame=0;
		break;
	case 0xff:
		frame=Frame.thisFrame-1;
		break;
	default:
		frame=(thisFrame);
		if(frame%Frame.rate==0)
		{
			Next();
		}
		frame=Frame.thisFrame;
		(++thisFrame)%=(FrameRate);
		break;
	}
	if(Anima->Anima[frame]==0xff){
		Playing=false;	
		return frame-1;
	}
	else
		return frame;
}
int Animation::Search_Last()
{
	int search=ANIMATION_FRAME/2,last=0;
	if(Anima->Anima[search+1]<=0||Anima->Anima[search-1]<=0){
		last=Search(search);
		if(last!=0)
			return last;
		if(Anima->Anima[search/2+1]<=0||Anima->Anima[search/2-1]<=0)
		{
			search/=2;
			last=Search(search);
			if(last!=0)
				return last;
		}
		for(int i=search+1,j=0;i>j;i--)
		{
			if(Anima->Anima[i]==-1)
				return i;
		}
	}
	else
	{
		last=Search(search);
		if(last!=0)
			return last;
		if(Anima->Anima[search+search/2+1]<=0||Anima->Anima[search+search/2-1]<=0)
		{
			search+=search/2;
			last=Search(search);
			if(last!=0)
				return last;
		}
		for(int i=ANIMATION_FRAME-1,j=search;i>j;i--)
		{
			if(Anima->Anima[i]==-1)
				return i;
		}
	}
	return search;
}
int Animation::Search(int search)
{
	if(Anima->Anima[search]==-1)
		return search;
	if(Anima->Anima[search+1]==-1)
		return search+1;
	if(Anima->Anima[search-1]==-1)
		return search-1;
	return 0;
}
bool Animation::checkAnima(LString &str)
{
	if((char *)Frame.Name==str)
		return false;
	else{
		Anima=Search_by_Name(str);
		return true;
	}
}
bool Animation::checkAnima(const char *str)
{
	LString str2=LString(Frame.Name);
	if(str2==str)
		return false;
	else{
		str2=LString(str);
		Anima=Search_by_Name(str2);
		return true;
	}
}


