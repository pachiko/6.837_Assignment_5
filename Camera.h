#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979f
#endif

class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ; 
	
	virtual float getTMin() const = 0 ; 
	virtual ~Camera(){}
protected:
	Vector3f center; 
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;

};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera: public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction,const Vector3f& up , float angle){
		// NB. UP may not be perpendicular to DIRECTION
		// u = horizontal, v = up, w = direction
		this->center = center;
		this->direction = direction.normalized(); // w
		this->horizontal = Vector3f::cross(direction, up.normalized()).normalized(); // u
		this->up = Vector3f::cross(this->horizontal, this->direction).normalized(); // v
		this->direction *= 1.f/tanf(angle/2.f); // D = 1/tan(alpha/2); Always used to scale w.
	}

	virtual Ray generateRay( const Vector2f& point){
		// (-1, -1) < point < (1, 1) ie. Normalized Image Coordinates
		Vector3f rayD = (direction + point.x()*horizontal + point.y()*up).normalized();
		return Ray(center, rayD);
	}

	virtual float getTMin() const { 
		return 0.0f;
	}

private:
};

#endif //CAMERA_H
