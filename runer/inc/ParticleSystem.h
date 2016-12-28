
#define PARTICLE_NUM 72

enum PARTICLE_TYPE
{
	RANDOM,BRAKES
};
typedef struct PARTICLE_STATUS
{
	float StartX;
	float StartY;
	float EndX;
	float EndY;
	float Left_time;
	int num;
	int x,y;
	//float Acceleration;
};
typedef struct SHAPE
{
	float (*x)(float);
	float (*y)(float);
	float (*z)(float);
};

class Particle
{
	typedef struct PARTICLE_FUNCTION
	{
		void (Particle::*init)(void);
		void (Particle::*shift)(void);
	};
public:
	Particle();
	Particle(Sprite *sp,int types);
	void Shape(float (*x)(float),float (*y)(float),float (*z)(float));
	void Switch();
	void SetCenter(int sw,int x,int y);
	void SetLeft(bool left);
	void SetBuffer(int buffer);
	void Start(int sw,float timer);
	
private:
	void init();
	PARTICLE_FUNCTION runing;
	PARTICLE_STATUS Engine[PARTICLE_NUM];
	PARTICLE_TYPE type;
	SHAPE shape;
	SDL_Rect center,temp[2];
	Sprite *sprite;
	int Amount;
	int CenterX;
	int CenterY;
	int Left;
	int index;
	int Buffer;
	float time;
	void Set();

	void Set_rand();

	void Shift();
	void Brakes_Set();
	void Brakes_Shift();
};