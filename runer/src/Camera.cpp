#include "SDL.h"
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "SDL_mixer.h"
#include "test2.h"
#include "UsrFuc.h"
#include "map.h"
#include "map_table.h"
#include "math.h"
#include "Camera.h"
Camera::Camera()
{
    init();
}

Camera::Camera( int x1, int y1, float max_speed )
{
    int x=x1;
    int y=y1;
    init();
    center.x = x ;
    center.y = y ;
    Speed = max_speed;
}

SDL_Rect Camera::operator +( const Camera& to )
{
    SDL_Rect site;
    site.x = this->center.x + to.center.x;
    site.y = this->center.y + to.center.y;
    return site;
}

SDL_Rect Camera::operator -( const Camera& to )
{
    SDL_Rect site;
    site.x = this->center.x - to.center.x;
    site.y = this->center.y - to.center.y;
    return site;
}

SDL_Rect Camera::operator *( const Camera& to )
{
    SDL_Rect site;
    site.x = this->center.x * to.center.x;
    site.y = this->center.y * to.center.y;
    return site;
}

SDL_Rect Camera::operator /( const Camera& to )
{
    SDL_Rect site;
    site.x = this->center.x / to.center.x;
    site.y = this->center.y / to.center.y;
    return site;
}

void Camera::setCenter( int x1, int y1 )
{
    int x=x1;
    int y=y1;
    center.x = x ;
    center.y = y ;
}

SDL_Rect Camera::getCenter()
{
    return center;
}

void Camera::moveUp( float speed )
{
    if ( speed == 0 )
        return ;

    int height = speed > 0 ? 0 : MAP_HEIGHT - BUFFER_HEIGHT;

    if ( speed > 0 )
        speed = ( speed > 1.0f ) ? 1.0f : speed;
    else if ( speed < 0 )
        speed = ( speed < -1.0f ) ? -1.0f : speed;

    center.y = ( ( speed > 0 && center.y - ( Speed * speed ) > height ) || ( speed < 0 && center.y - ( Speed * speed ) < height ) ) ? center.y - ( int ) ( Speed * speed ) : height;
}

void Camera::moveLeft( float speed )
{
    if ( speed == 0 )
        return ;

    int width = speed > 0 ? 0 : MAP_WIDTH - BUFFER_WIDTH;

    if ( speed > 0 )
        speed = ( speed > 1.0f ) ? 1.0f : speed;
    else if ( speed < 0 )
        speed = ( speed < -1.0f ) ? -1.0f : speed;

    center.x = ( ( speed > 0 && center.x - ( Speed * speed ) > width ) || ( speed < 0 && center.x - ( Speed * speed ) < width ) ) ? center.x - ( int ) ( Speed * speed ) : width;
}

void Camera::moveTo( SDL_Rect *site, GRADE speed )
{
    tempSite = site;
    Grade = speed;
    moving = ( tempSite->x != center.x ) || ( tempSite->y != center.y );
}

float Camera::gradeChange()
{
    float grade;

    switch ( Grade )
    {
    case ZERO_SPEED:
        grade = 0;
        break;
    case SPEED1:
        grade = 0.1f;
        break;
    case SPEED2:
        grade = 0.2f;
        break;
    case SPEED3:
        grade = 0.4f;
        break;
    case SPEED4:
        grade = 0.8f;
        break;
    case SPEED5:
        grade = 1.0f;
        break;
    }

    return grade;
}

float Camera::getSpeed()
{
    return Speed;
}

void Camera::setSpeed( float speed )
{
    Speed = speed;
}

void Camera::setAuto( bool Auto )
{
    auto_speed = Auto;
}

void Camera::move()
{
    int XSpeed;
    int YSpeed;

    if ( auto_speed )
    {
        XSpeed = abs( center.x - tempSite->x );
        YSpeed = abs( center.y - tempSite->y );
    }

    if ( moving )
    {
        if ( auto_speed ) Speed = XSpeed;

        if ( center.x < tempSite->x )
        {
            this->moveLeft( -gradeChange() );
        }

        if ( center.x > tempSite->x )
        {
            this->moveLeft( gradeChange() );
        }

        if ( auto_speed ) Speed = YSpeed;

        if ( center.y < tempSite->y )
        {
            this->moveUp( -gradeChange() );
        }

        if ( center.y > tempSite->y )
        {
            this->moveUp( gradeChange() );
        }

        if ( abs( center.y - tempSite->y ) <= 2 && abs( center.x - tempSite->x ) <= 2 )
        {
            moving &= 0;
            center = ( SDL_Rect ) ( *tempSite );
        }
    }
}
void Camera::init()
{
	memset(this,0,sizeof(Camera));
}
