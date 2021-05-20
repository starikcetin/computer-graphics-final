#pragma once

#include <stdio.h>
#include <math.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define D2R 0.01745329251

using namespace glm;

class Camera
{

public:

	vec3 pos ;
	float  yaw ;   // angle between x and z 
	float  pitch  ;   // angle between y and z
    float  speed ;
    vec3 dir;

    inline Camera( float p_x, float p_y, float p_z,float p_lr, float p_ud, float p_spd )
	{
	  SetPosition(vec3(p_x, p_y, p_z));
	  SetLeftRight(p_lr);
	  SetUpDown(p_ud);
	  SetSpeed(p_spd);
	  Update();
	}

	inline void SetPosition( vec3 pNewPos  )
	{
		pos = pNewPos;
	}

	inline void SetLeftRight( float p_fLR )
	{
		yaw = p_fLR ;
		Update() ;
	}

	inline void SetUpDown( float p_fUD )
	{
		pitch = p_fUD ;
	}

	inline void SetSpeed( float p_fSpd )
	{
		speed = p_fSpd  ;
	}

	inline void Forward()
	{
        Update() ;
		pos += dir;
	}

	inline void Backward()
	{
        Update() ;
		pos -= dir;
	}

  inline void TurnRightLeft( float p_angle )
	{
		yaw = fmod(yaw + p_angle, 360); // (((int)yaw + (int)p_angle) % 360);
		
        Update() ;
		
	}
   
	inline void TurnUpDown( float p_angle )
	{
		
	  pitch = fmod(pitch + p_angle, 360);  // ( ((int) pitch  + (int) p_angle)  % 360 ) ;
	
	  if ( pitch >= 90 )
		  pitch = 89 ;

	  if ( pitch <= -90 )
		  pitch = -89 ;

	  
	  Update() ;
	}

  inline void Update()
	{
		dir.x = (float) (-speed * sin(yaw * D2R ) * cos(pitch * D2R)) ;
        dir.y = (float) ( speed * sin(pitch * D2R)) ;
		dir.z = (float) (-speed * cos(yaw * D2R) * cos(pitch * D2R));
	}

	inline glm::mat4 LookAt()
	{
		return glm::lookAt(pos, pos + dir , vec3(0, 1, 0));  // gluLookAt()
	}

    
    inline float getLeftRight()
	{
		return yaw ;
	}

	inline void Upward()
	{
		pos += vec3(0,speed,0) ;
	}

	inline void Downward()
	{
		pos -= vec3(0, speed, 0);
	}

	inline void Left()
	{
	   pos += vec3( dir.z, 0, -dir.x);
	}

	inline void Right()
	{
	   pos -= vec3(dir.z, 0, -dir.x);
  }

} ;
