enum GRADE
{
	ZERO_SPEED,SPEED1,SPEED2,SPEED3,SPEED4,SPEED5
};
class Camera
{
public:
	Camera();
	Camera(int x, int y,float max_speed);
	/*		Operator		*/
	SDL_Rect operator +(const Camera& to);
	SDL_Rect operator -(const Camera& to);
	SDL_Rect operator *(const Camera& to);
	SDL_Rect operator /(const Camera& to);
	void setAuto(bool Auto);//攝影機自動跟隨
	void setSpeed(float speed);//設定跟隨速度
	float getSpeed();
	void setCenter(int x,int y);//設定跟隨目標
	SDL_Rect getCenter();//取得目前攝影機座標

	void moveTo(SDL_Rect *site,GRADE speed);//設定速度，指定座標
	void move();//移動至指定座標
private:
	GRADE	Grade;
	SDL_Rect center;
	SDL_Rect *tempSite;
	void init();
	float Speed;
	bool moving;
	bool auto_speed;
	void moveUp(float speed);
	void moveLeft(float speed);
	float gradeChange();
};
