#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane(){}
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m){
		n = normal.normalized();
		this->d = d;
	}
	~Plane(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		float nRd = Vector3f::dot(n, r.getDirection());
		if (abs(nRd) > 0) {
			// P.n = d; Therefore, P.n - d = 0;
			float t = (d - Vector3f::dot(n, r.getOrigin())) / nRd;
			if (t > tmin && t < h.getT()) {
				h.set(t, material, n);
				return true;
			}
		}
		return false;
	}

protected:
	Vector3f n;
	float d;
};
#endif //PLANE_H
		

