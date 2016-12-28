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
	int Status;//動畫編號
	//Player operator+(const Player&);
	//Player operator-(const Player&);
	SDL_Rect getOffset(){return Offset.offset;};//取得座標
	bool getShowFlag(){return draw_player.showFlag;};//取得顯示狀態
	int getFrame(){return Offset.FRAME;};//取得目前撥放影格
	void setFrame(int frame){ Offset.FRAME=frame; };//指定影格
	void ALPHA(int value){ Alpha=value; };//設定透明值
	void setStatus(SDL_Rect *offset,int shadow,int mirror,int alpha);
	void setOffset(SDL_Rect *offset);//設定座標位置
	void setRate(float frame_rate);//手動播放速率 0~1
	void setShadow(int frame_index,int frame_index2);//no use
	void setShadow_Mirror(int frame_index,int frame_index2);//nou use
	void setMirror(int frame_index);//鏡射圖片位置
	void setAlpha(int frame_index,int mirror_frame_index);//可透明圖片位置
	void setSpeed(int speed);//設定手動給值播放動畫之，最快速率
	//void setHeight(int height);
	void setMap(SDL_Rect Map);//設定目前底圖位置
	//void setAnimation(int * frameIndex,int * frame);//no use
	void showShadow(bool show);//no use
	void LEFT(bool left);//是否做鏡射
	void Update();//更新為下一張動畫，以動畫編號播放時使用
	bool Update(const char *str);//指定動畫名稱做撥放動作
	PLAYER Offset;
	//SDL_Rect getOffset(){ return Offset.offset;};
	//SDL_Rect getSite(){ return Map_Site;};
	//
	//void Jump_To(SDL_Rect *site);
	//void Move_To(SDL_Rect *site);
	//void Move_Left(float speed);
	//void Move_Up(float speed);
	//void Move(float speed);
	void draw(BUFFER_MANAGER buffer);//繪圖在指定buffer上
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