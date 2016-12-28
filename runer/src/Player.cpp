#include "SDL.h"
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "SDL_mixer.h"
#include "test2.h"
#include "UsrFuc.h"
#include "map.h"
#include "map_table.h"
#include "math.h"
#include "Animation.h"
#include "Player.h"
#include "Player_table.h"
/*		Public		*/
extern SDL_Surface * SpriteData[];
Player::Player()
{
	init();
}
Player::Player(const PLAYER_Table *Player,int x, int y, int frame_index,int frame_amount)
{
	init();
	select=Animation(Player,30);
	Ani=Player;
	Offset.offset.x=x;
	Offset.offset.y=y;
	Offset.offset.w=SpriteData[frame_index]->w;
	Offset.offset.h=SpriteData[frame_index]->h;
	Offset.LEFT=true;
	Frame_Index=frame_index;
	Frame_Amount=frame_amount;
}
void Player::setStatus(SDL_Rect *offset, int shadow, int mirror, int alpha)
{
	setOffset(offset);
	//setAlpha(alpha);
	setMirror(mirror);
	setShadow(shadow,shadow);
}
void Player::setRate(float frame_rate)
{
	if(frame_rate>=1)
		frame_rate=1;
	rate=abs(frame_rate);
}
void Player::setOffset(SDL_Rect *offset)
{
	this->Offset.offset=(SDL_Rect)(*offset);
}
void Player::setAlpha(int frame_index,int mirror_frame_index)
{
	show_Alpha=true;
	this->ALPHA_Frame=frame_index;
	this->ALPHA_Mirror_Frame=mirror_frame_index;
}
void Player::setMirror(int frame_index)
{
	this->MIRROR_Frame=frame_index;
}
void Player::setShadow(int frame_index,int frame_index2)
{
	this->SHADOW_Frame=frame_index;
	this->SHADOW_Frame2=frame_index2;
}
void Player::setShadow_Mirror(int frame_index,int frame_index2)
{
	SHADOW_Mirror_Frame=frame_index;
	SHADOW_Mirror_Frame2=frame_index2;
}
void Player::setSpeed(int speed)
{
	Speed = speed;
}
void Player::setMap(SDL_Rect Map)
{
	Map_Site=Map;
}
void Player::LEFT(bool left)
{
	Offset.LEFT=left;
}
void Player::showShadow(bool show)
{
	show_ShadowFlag=show;
}
//void Player::setHeight(int height)
//{
//	jump_height = height;
//}

//void Player::Jump_To(SDL_Rect *site)
//{
//	tempxy=(SDL_Rect)(*site);
//}
//void Player::Move_To(SDL_Rect *site)
//{
//	tempxy=(SDL_Rect)(*site);
//}
//void Player::Move_Left(float speed)
//{
//	if( speed == 0)
//		return;
//	int boundary=speed>0?0:MAP_WIDTH-Offset.offset.w;
//	int move_space=(int)(Speed*speed);
//	Offset.offset.x=(speed>0&&Offset.offset.x-move_space>boundary)||(speed<0&&Offset.offset.x-move_space<boundary)?Offset.offset.x-move_space:boundary;
//}
//void Player::Move(float speed)
//{
//	rate=&speed;
//	Offset.X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( Offset.offset.x - Map_Site.x ) );
//	Offset.Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( Offset.offset.y - Map_Site.y ) );
//	if(!Offset.LEFT)
//	{
//		isRight(speed);
//	}
//	else
//	{
//		isLeft(speed);
//	}
//
//}
void Player::Update()
{
	LString status=LString((char *)Ani->anima[Status].Name);
	select.Speed(Speed,rate*1.12f);
	Offset.FRAME=select.Play(status);
}
bool Player::Update(const char *str)
{
	//LString status=LString((char *)Ani->anima[Status].Name);
	select.Speed(Speed,rate*1.12f);
	Offset.FRAME=select.Play(str);
	return select.isPlay();
}
void Player::draw(BUFFER_MANAGER buffer)
{
	
	//showShadow();
	
	//float frame_m= (rate) * Speed;
	//int frame_c = frame_m + 1;
	//int frane_s =(Frame_Amount*(Speed+1))/2;
	//=((Frame/(frane_s/frame_c)))%Frame_Amount;

	Offset.X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( Offset.offset.x - Map_Site.x ) );
	Offset.Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( Offset.offset.y - Map_Site.y ) );
	if(show_Alpha)
	{
		if(Offset.LEFT)
		{
			draw_player=OBJ(Offset.X,Offset.Y,Offset.FRAME,Frame_Index,ALPHA_Frame);
		}else
		{
			draw_player=OBJ(Offset.X,Offset.Y,Offset.FRAME,MIRROR_Frame,ALPHA_Mirror_Frame);
		}
		draw_player.Alpha=Alpha;
	}else{
		if(Offset.LEFT)
		{
			draw_player=OBJ(Offset.X,Offset.Y,Offset.FRAME,Frame_Index,-1);
		}else
		{
			draw_player=OBJ(Offset.X,Offset.Y,Offset.FRAME,MIRROR_Frame,-1);
		}
	}
	draw_player.setBuffer(buffer);
	draw_player.showFlag=abs(draw_player.x)<BUFFER_WIDTH&&abs(draw_player.y)<BUFFER_HEIGHT;
	draw_player.draw();
	//(++Frame)%=(int)(((Speed+1)*(Frame_Amount*Speed))/frame_c);
	
	
}
/*		Private		*/
void Player::init()
{
	memset(this,0,sizeof(Player));
}
//void Player::Jump()
//{
//	Offset.offset.y = Map_Site.y + ( BUFFER_HEIGHT/2+1 ) + jump_velue;
//
//	jump_velue+=move_up;
//	if(jump_velue>0&&move_up>0){
//		move_up=(int)(jump_height*(float)(*rate))*-1;
//	}else
//	{
//		move_up+=2;
//	}
//}
//void Player::isLeft(float speed)
//{
//	if(Offset.X<Offset.offset.w)
//	{
//		Move_Left(-speed);
//	}else
//	{
//		int move_space=(int)(Speed*speed);
//		Move_Left(-speed);
//		Map_Site.x = Map_Site.x < MAP_WIDTH - SCREEN_WIDTH / 2 ? (Sint16)(Map_Site.x + move_space) : (Sint16)(MAP_WIDTH - SCREEN_WIDTH / 2);
//		if ( (Map_Site.x == MAP_WIDTH - SCREEN_WIDTH / 2) && ( Offset.X == BUFFER_WIDTH - Offset.offset.w ))
//        {
//			Offset.offset.x = MAP_WIDTH - Offset.offset.w;
//			Offset.LEFT &= 0;
//		}
//	}
//}
//void Player::isRight(float speed)
//{
//	if(Offset.X>Offset.offset.w)
//	{
//		Move_Left(speed);
//	}else
//	{
//		int move_space=(int)(Speed*speed);
//		Move_Left(speed);
//		Map_Site.x = Map_Site.x > 0 ? (Sint16)(Map_Site.x - move_space) : 0;
//		if((Offset.X==0)&&(Map_Site.x==0))
//		{
//			Offset.offset.x=0;
//			Offset.LEFT = true;
//		}
//	}
//}
void Player::addShadow(PLAYER *shadow)
{
	for(int i=0;i<SHADOW_AMOUNT;i++)
	{
		if(!Shadow[i].showFlag)
		{
			Shadow[i]=(PLAYER)(*shadow);
			//Shadow[i].offset.x-=rand()%3+2;
			//Shadow[i].offset.y-=rand()%10+2;
			Shadow[i].showFlag=true;
			Shadow[i].ALPHA=8;
			break;
		}
	}

}
void Player::showShadow()
{

	for(int i=0;i<SHADOW_AMOUNT;i++)
	{
		if(Shadow[i].showFlag)
		{
			int index;
			if(Shadow[i].LEFT)
				index =i%2? SHADOW_Frame:SHADOW_Frame2;
			else
				index =i%2? SHADOW_Mirror_Frame:SHADOW_Mirror_Frame2;
			int x;
			if(Shadow[i].ALPHA<128){
				/*if(Shadow[i].LEFT==Offset.LEFT)
					x=(Shadow[i].LEFT)?(Offset.offset.x+Shadow[i].ALPHA):(Offset.offset.x-Shadow[i].ALPHA);
				else
					x=Shadow[i].offset.x;*/
				x=Shadow[i].offset.x;
			}else
			{
				//x=(Shadow[i].LEFT)?(Shadow[i].offset.x-(Offset.offset.x-Shadow[i].offset.x)/10):(Shadow[i].offset.x+(Offset.offset.x-Shadow[i].offset.x)/10);
				x=Shadow[i].offset.x=Shadow[i].offset.x+(Offset.offset.x-Shadow[i].offset.x)/5;
				Shadow[i].offset.y=Shadow[i].offset.y+(Offset.offset.y-Shadow[i].offset.y)/5;
			}
			
			int frame=Shadow[i].FRAME/*(Offset.FRAME+Shadow[i].FRAME)%4*/;
			Shadow[i].X = BUFFER_WIDTH - ( BUFFER_WIDTH - ( (x)- Map_Site.x ) );
			Shadow[i].Y = BUFFER_HEIGHT - ( BUFFER_HEIGHT - ( Shadow[i].offset.y - Map_Site.y ) );
			draw_player=OBJ(Shadow[i].X,Shadow[i].Y,frame,index,-1);
			draw_player.Alpha=(Shadow[i].ALPHA+64)<255?(Shadow[i].ALPHA+32):255;
			draw_player.showFlag=true;
			draw_player.draw();
			Shadow[i].ALPHA+=12/*abs((Offset.offset.x-Shadow[i].offset.x)/30)*/;
			Shadow[i].showFlag=(Shadow[i].ALPHA)<225;
		}
	}
	if(show_ShadowFlag){
		
		Shadow_sce=Shadow_sce>0?Shadow_sce-1:0;
		if(Shadow_sce==0)
		{
			addShadow(&Offset);
			Shadow_sce=(1.2-rate)*25;
		}
	}
}