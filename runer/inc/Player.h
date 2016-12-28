#define SHADOW_AMOUNT 5

typedef struct{
	SDL_Rect offset;
	Uint8 ALPHA;
	int FRAME;
	int X,Y;
	bool showFlag;
	bool LEFT;
}PLAYER;


enum PLAYER_STATUS
{	STAND,RUN,JUMP_UP,JUMP_DOWN,HIT,CLIMB,CLIMBING1,CLIMBING2,CLIMBED,JUMPING};
class Player
{
public:
	Player();
	Player(const PLAYER_Table *Player,int x, int y, int frame_index,int frame_amount);
	int Status;//�ʵe�s��
	//Player operator+(const Player&);
	//Player operator-(const Player&);
	SDL_Rect getOffset(){return Offset.offset;};//���o�y��
	bool getShowFlag(){return draw_player.showFlag;};//���o��ܪ��A
	int getFrame(){return Offset.FRAME;};//���o�ثe����v��
	void setFrame(int frame){ Offset.FRAME=frame; };//���w�v��
	void ALPHA(int value){ Alpha=value; };//�]�w�z����
	void setStatus(SDL_Rect *offset,int shadow,int mirror,int alpha);
	void setOffset(SDL_Rect *offset);//�]�w�y�Ц�m
	void setRate(float frame_rate);//��ʼ���t�v 0~1
	void setShadow(int frame_index,int frame_index2);//no use
	void setShadow_Mirror(int frame_index,int frame_index2);//nou use
	void setMirror(int frame_index);//��g�Ϥ���m
	void setAlpha(int frame_index,int mirror_frame_index);//�i�z���Ϥ���m
	void setSpeed(int speed);//�]�w��ʵ��ȼ���ʵe���A�ֳ̧t�v
	//void setHeight(int height);
	void setMap(SDL_Rect Map);//�]�w�ثe���Ϧ�m
	//void setAnimation(int * frameIndex,int * frame);//no use
	void showShadow(bool show);//no use
	void LEFT(bool left);//�O�_����g
	void Update();//��s���U�@�i�ʵe�A�H�ʵe�s������ɨϥ�
	bool Update(const char *str);//���w�ʵe�W�ٰ�����ʧ@
	PLAYER Offset;
	//SDL_Rect getOffset(){ return Offset.offset;};
	//SDL_Rect getSite(){ return Map_Site;};
	//
	//void Jump_To(SDL_Rect *site);
	//void Move_To(SDL_Rect *site);
	//void Move_Left(float speed);
	//void Move_Up(float speed);
	//void Move(float speed);
	void draw(BUFFER_MANAGER buffer);//ø�Ϧb���wbuffer�W
private:
	Animation select;
	OBJ	draw_player;
	
	PLAYER Shadow[SHADOW_AMOUNT];
	const PLAYER_Table *Ani;
	//SDL_Rect Camera;
	//SDL_Rect tempxy;
	SDL_Rect Map_Site;
	//void (*X)();
	//void (*Y)();
	void init();
	void addShadow(PLAYER *shadow);
	void showShadow();

	//bool jump;
	//int jump_height;
	//int jump_velue;
	//int move_up;
	//void Jump();
	//
	//void isRight(float speed);
	//void isLeft(float speed);

	int Speed;
	int Shadow_sce;
	bool show_ShadowFlag;
	bool show_Alpha;
	float rate;
	int Alpha;
	int Frame;
	int Frame_Amount;
	int Frame_Index;
	int ALPHA_Frame;
	int ALPHA_Mirror_Frame;
	int MIRROR_Frame;
	int SHADOW_Frame;
	int SHADOW_Frame2;
	int SHADOW_Mirror_Frame;
	int SHADOW_Mirror_Frame2;
	//int Status;
};