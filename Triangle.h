#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
public:
	Triangle();
        ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
          hasTex = false;
		  this->a = a;
		  this->e1 = b - a;
		  this->e2 = c - a;
	}

	virtual bool intersect( const Ray& ray,  Hit& hit , float tmin){
		//  Moller-Trumbore

		Vector3f s = ray.getOrigin() - a;
		Vector3f d = ray.getDirection();
		Vector3f e1d = Vector3f::cross(e1, d);
		
		float det = Vector3f::dot(e1d, e2);
		if (det > 0) {
			float invDet = 1.f / det;
			Vector3f se2 = Vector3f::cross(s, e2);
			float u = -Vector3f::dot(se2, d) * invDet;
			float v = Vector3f::dot(e1d, s) * invDet;
			float t = -Vector3f::dot(se2, e1) * invDet;

			if (t > tmin && t < hit.getT() && u >= 0.f && v >= 0.f && (u + v) <= 1.f) {
				Vector3f n = (1.f - u - v) * normals[0] + u * normals[1] + v * normals[2];
				if (hasTex) { // Mesh intersection sets this
					// interpolate texture coords
					Vector2f texC = (1.f - u - v) * texCoords[0] + u * texCoords[1] + v * texCoords[2];
					hit.setTexCoord(texC);
				}
				hit.set(t, material, n.normalized());
				return true;
			}
		}

		return false;
	}
	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];
protected:
	Vector3f a, e1, e2;
};

#endif //TRIANGLE_H
