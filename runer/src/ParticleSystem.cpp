#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "math.h"
#include "UsrFuc.h"
#include "Sprite.h"
#include "ParticleSystem.h"

extern SDL_Surface * SpriteData[];

/*		Public		*/
Particle::Particle()
{
	init();
}
Particle::Particle(Sprite *sp,int types)
{
	init();
	sprite=sp;
	type=(PARTICLE_TYPE )types;
	Switch();
	index=(int)*sprite->Alpha_Index;
	Amount=(int)*sprite->Amount;
	Left=1;

}
void Particle::Shape(float (*x)(float),float (*y)(float),float (*z)(float))
{
	shape.x=x;
	shape.y=y;
	shape.z=z;
}
void Particle::SetLeft(bool left)
{
	Left=left?1:-1;
}
void Particle::SetCenter(int sw,int x, int y)
{
	temp[sw].x=x;
	temp[sw].y=y;
}

void Particle::SetBuffer(int buffer)
{
	Buffer=buffer;
}
void Particle::Start(int sw,float timer)
{
	CenterX=temp[sw].x;
	CenterY=temp[sw].y;
	timer=timer<1?timer:1;
	timer=timer>0?timer:0;
	time=timer;
	//static int inc =1;
	//inc = ( time >= 0.95f && inc > 0 ) || ( time <= 0.0f && inc < 0 ) ? -inc : inc;
	(this->*runing.shift)();

	//time+=0.01f*inc;
	//time=0.5f;
	//time+=((1.0f-time)/60)*inc;
	//time=(time+((1.0f-time)/40)*inc>=0.95f)?0.0f:time+((1.0f-time)/40)*inc/*((1.0f-time)/30)*/;
}
/*		Private		*/
void Particle::init()
{
	memset(this,0,sizeof(Particle));
}
void Particle::Switch()
{
	void (Particle::*SET)(void)=0;
	void (Particle::*SHIFT)(void)=0;
	switch(type)
	{
	case RANDOM:
		//Shape(sin,cos,tan);
		SET=&Particle::Set;
		SHIFT=&Particle::Shift;
		break;
	case BRAKES:
		SET=&Particle::Brakes_Set;
		SHIFT=&Particle::Brakes_Shift;
		break;
	}
	runing.init=SET;
	runing.shift=SHIFT;
	(this->*runing.init)();
}
void Particle::Set()
{
	for(int i=0;i<PARTICLE_NUM;i++)
	{
		float theta=(((float)i/PARTICLE_NUM)*360.0f)*PI/180.0f;
		Engine[i].Left_time=shape.y(theta)*0.125f;//( (float)(rand() % 10000) / 10000.0f );
		Engine[i].EndX=shape.y(theta)*0.8f;
		Engine[i].EndY=shape.x(theta)*0.8f;
		Engine[i].StartX=shape.x(theta)*0.125f;//( (float)(rand() % 10000) / 10000.0f )*0.125f;//
		Engine[i].StartY=shape.y(theta)*0.125f;//( (float)(rand() % 10000) / 10000.0f )*0.125f;//
		Engine[i].num=i;
	}
}

void Particle::Set_rand()
{
	for(int i=0;i<PARTICLE_NUM;i++)
	{
		float theta=(((float)i/PARTICLE_NUM)*360.0f)*PI/180.0f;
		Engine[i].Left_time=( (float)(rand() % 10000) / 30000.0f )+1.0f /*- 0.25f*/;
		Engine[i].EndX=( (float)(rand() % 20000) / 10000.0f ) - 1.0f;
		Engine[i].EndY=( (float)(rand() % 20000) / 10000.0f ) - 1.0f;
		Engine[i].StartX=( (float)(rand() % 10000) / 50000.0f )-0.125f;//
		Engine[i].StartY=( (float)(rand() % 10000) / 50000.0f )-0.125f;//
		Engine[i].num=i;
	}
}
void Particle::Shift()
{
	OBJ draw;
	for(int i=0;i<PARTICLE_NUM;i++)
	{
		int j=(++Engine[i].num)%PARTICLE_NUM;
		float time_m=Engine[i].Left_time+(1-time);

		Engine[i].x=center.x+((Engine[i].StartX+((Engine[i].StartX-Engine[j].EndX)*time))*time_m)*center.w;
		Engine[i].y=center.y+((Engine[i].StartY+((Engine[i].StartY-Engine[j].EndY)*time))*time_m)*center.h;

		draw=OBJ(Engine[i].x,Engine[i].y,0/*(int)(time*5)*/,index,index);
		draw.showFlag=true;
		draw.Alpha=((time*232)+64)>255?255:((time*232)+64);
		draw.draw();
	}
}
void Particle::Brakes_Set()
{
	for(int i=0;i<PARTICLE_NUM;i++)
	{
		//float theta=(((float)i/PARTICLE_NUM)*60.0f)*PI/180.0f;
		Engine[i].Left_time=(1.0f-(float)i/PARTICLE_NUM)*0.8f;
		Engine[i].EndX=( (float)(rand() % 10000) / 10000.0f )*0.8f+((float)i/PARTICLE_NUM)*0.8f;
		Engine[i].EndY=-( (float)(rand() % 10000) / 10000.0f )*3.5f;
		Engine[i].StartX=( (float)(rand() % 10000) / 10000.0f )*0.125f;//shape.x(theta)*0.125f;//( (float)(rand() % 10000) / 10000.0f )*0.125f;//
		Engine[i].StartY=( (float)(rand() % 10000) / 10000.0f )*0.125f;//shape.y(theta)*0.125f;//( (float)(rand() % 10000) / 10000.0f )*0.125f;//
		Engine[i].num=i;
	}
}
void Particle::Brakes_Shift()
{
	OBJ draw;

	for(int i=0;i<PARTICLE_NUM;i++)
	{
		int num=((int)(Engine[i].Left_time*Amount)+(int)(time*Amount))%10;
		float time_m=1;//Engine[i].Left_time;
		float time_mov=(((float)i/PARTICLE_NUM)*(128*Engine[i].EndY)*time);
		center.x=CenterX-SpriteData[index+num]->w/2-(time_mov*Left);
		center.y=CenterY-SpriteData[index+num]->h/2;
		center.w=SpriteData[index]->w*(time*3);
		center.h=SpriteData[index]->h*(time*3);
		Engine[i].x=center.x+(Engine[i].StartX+(Engine[i].EndX*time_m)*time)*center.w;
		Engine[i].y=center.y+(Engine[i].StartY+(Engine[i].EndY*time_m)*time)*center.h;
		draw=OBJ(Engine[i].x,Engine[i].y,num,index,index);
		draw.showFlag=true;
		draw.setBuffer((BUFFER_MANAGER)Buffer);
		draw.Alpha=((time*175)+100)>255?255:((time*175)+100);
		draw.draw();
	}
}
