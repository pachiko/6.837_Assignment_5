#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
  Transform(){}
 Transform( const Matrix4f& m, Object3D* obj ):o(obj){
     t = m.inverse(); // m is the transformation to original/world. obj is original.
     // so t should be inverse to go to transformed obj
  }
  ~Transform(){
  }
  virtual bool intersect( const Ray& r , Hit& h , float tmin){
      Vector4f ot = t * Vector4f(r.getOrigin(), 1.f);
      Vector4f dt = t * Vector4f(r.getDirection(), 0.f); // ray dir ignores translation
      Ray rt(ot.xyz(), dt.xyz());

      bool res = o->intersect(rt, h, tmin);

      if (res) {
          Matrix4f tn = t.transposed(); // normal is in transformed space. t is world to transformed.
          // we want normal to be in world, so t(-1)(-1).T = t.T
          Vector4f n(h.getNormal(), 0.f);
          h.set(h.getT(), h.getMaterial(), (tn*n).xyz().normalized());
      }

      return res;
  }

 protected:
  Object3D* o; //un-transformed object
  Matrix4f t;
};

#endif //TRANSFORM_H
