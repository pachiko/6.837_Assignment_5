#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>
#include <list>
using namespace std;

///TODO: 
///Implement Group
///Add data structure to store a list of Object* 
class Group:public Object3D
{
public:

  Group(){
  }
	
  Group( int num_objects ){
	  list<Object3D*> objs = list<Object3D*>{ unsigned (num_objects) };
  }

  ~Group(){
	  for (auto o : objs) {
		  delete o;
	  }
  }

  virtual bool intersect( const Ray& r , Hit& h , float tmin) {
	  bool hit = false;
	  for (auto o : objs) {
		  bool b = o->intersect(r, h, tmin);
		  if (!hit && b) {
			  hit = true;
		  }
	  }
	  return hit;
   }

   bool intersectShadow( const Ray& r , Hit& h , float tmin) {
	  bool hit = false;
	  for (auto o : objs) {
		  bool b = o->intersect(r, h, tmin);
		  if (!hit && b) {
			  hit = true;
			  return hit;
		  }
	  }
	  return hit;
   }
	
  void addObject( int index , Object3D* obj ){
	  if (index > objs.size() - 1) {
		  objs.push_back(obj);
	  }
	  else {
		  auto it = objs.begin();
		  advance(it, index);
		  objs.insert(it, obj);
	  }
  }

  int getGroupSize(){ 
	  return objs.size();
  }

 private:
	 list<Object3D*> objs;
};

#endif
	
