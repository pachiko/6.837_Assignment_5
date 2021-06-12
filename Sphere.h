#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere(){ 
		//unit ball at the center
		cent = Vector3f();
		rad = 1.f;
		material = NULL;
	}

	Sphere( Vector3f center , float radius , Material* material ):Object3D(material){
		cent = center;
		rad = radius;
	}
	

	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		Vector3f o = r.getOrigin() - cent; // actually ray.o - center
		Vector3f d = r.getDirection();

		float dto = Vector3f::dot(d, o);
		float oto = Vector3f::dot(o, o);
		float dtd = Vector3f::dot(d, d);

		float discrim = dto * dto - dtd * (oto - rad*rad);

		if (discrim > 0) {
			discrim = sqrtf(discrim);
			float t1 = (-dto - discrim) / dtd;
			float t2 = (-dto + discrim) / dtd;

			if (t1 > tmin && t1 < h.getT()) {
				h.set(t1, material, (r.pointAtParameter(t1) - cent).normalized());
				return true;
			}
			else if (t2 > tmin && t2 < h.getT()) {
				h.set(t2, material, (r.pointAtParameter(t2) - cent).normalized());
				return true;
			}
		}

		return false;
	}

protected:
	Vector3f cent;
	float rad;

};


#endif
