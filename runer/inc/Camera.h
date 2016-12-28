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
	void setAuto(bool Auto);//��v���۰ʸ��H
	void setSpeed(float speed);//�]�w���H�t��
	float getSpeed();
	void setCenter(int x,int y);//�]�w���H�ؼ�
	SDL_Rect getCenter();//���o�ثe��v���y��

	void moveTo(SDL_Rect *site,GRADE speed);//�]�w�t�סA���w�y��
	void move();//���ʦܫ��w�y��
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
