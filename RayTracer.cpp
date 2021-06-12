#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define EPSILON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming)
{
  return (incoming - 2.f * Vector3f::dot(incoming, normal) * normal).normalized();
}

bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                          float index_n, float &index_nt,
                          Vector3f &transmitted)
{
  float dN = Vector3f::dot(normal, incoming);
  float sqr = 1.f - pow(index_n, 2.f) * (1.f - pow(dN, 2.f)) / (pow(index_nt, 2.f));

  if (sqr < 0.f)
  {
    return false;
  }
  else
  {
    transmitted = index_n / index_nt * (incoming - normal * dN) - normal * sqrt(sqr);
    transmitted.normalize();
    return true;
  }
}

RayTracer::RayTracer(SceneParser *scene, int max_bounces, bool shadows) : m_scene(scene), m_maxBounces(max_bounces), shadows(shadows)
{
  g = scene->getGroup();
}

RayTracer::~RayTracer()
{
}

Vector3f RayTracer::traceRay(Ray &ray, float tmin, int bounces,
                             float refr_index, Hit &hit) const
{
  if (bounces > m_maxBounces)
    return Vector3f::ZERO; // no contribution

  bool isHit = g->intersect(ray, hit, tmin);

  if (!isHit)
    return m_scene->getBackgroundColor(ray.getDirection());

  // Vector3f col;
  Vector3f col = m_scene->getAmbientLight() * hit.getMaterial()->getDiffuseColor(); // ambient
  Vector3f hitPoint = ray.pointAtParameter(hit.getT());

  for (size_t k = 0; k < m_scene->getNumLights(); k++)
  {
    Light *light = m_scene->getLight(k);
    Vector3f cLight, lightDir; // light intensity/color, direction to light
    float dist2Light;          // check if the shadow ray hits something before the light

    light->getIllumination(hitPoint, lightDir, cLight, dist2Light); // does NOT alter hitPoint

    if (shadows)
    {                                                  // Shadows
      Hit shadowHit(dist2Light - EPSILON, NULL, NULL); // set max_t
      Ray shadowRay(hitPoint, lightDir);
      bool isShadow = g->intersectShadow(shadowRay, shadowHit, tmin + EPSILON);
      if (isShadow)
        continue;
    }

    col += hit.getMaterial()->Shade(ray, hit, lightDir, cLight); // diffuse
  }

  // Reflect
  Vector3f reflectCol;
  Vector3f ks = hit.getMaterial()->getSpecularColor();
  Vector3f reflectDir;
  if (ks > Vector3f::ZERO)
  {
    reflectDir = mirrorDirection(hit.getNormal(), ray.getDirection());
    Ray reflect(hitPoint, reflectDir);
    Hit reflectHit;
    reflectCol = ks * traceRay(reflect, tmin + EPSILON, bounces + 1, refr_index, reflectHit);
  }

  float nt = hit.getMaterial()->getRefractionIndex();
  if (nt > 0)
  {
    // Refract
    Vector3f refractCol;
    Vector3f refractDir;
    Vector3f n = hit.getNormal();

    // check if inside obj, swap nt to 1.f if so.
    if (Vector3f::dot(ray.getDirection(), n) > 0.f)
    {
      nt = 1.f;
      n *= (-1.f);
    }

    if (transmittedDirection(n, ray.getDirection(), refr_index, nt, refractDir))
    {
      Ray refract(hitPoint, refractDir);
      Hit refractHit;

      refractCol = ks * traceRay(refract, tmin + EPSILON, bounces + 1, nt, refractHit);

      // Fresnel/Schlick
      float schlick = pow((nt - refr_index) / (nt + refr_index), 2.f);
      // float cos = abs(Vector3f::dot((refr_index > nt)? refractDir : reflectDir, n));

      float cos = abs(Vector3f::dot(ray.getDirection(), n));
      schlick += (1.f - schlick) * pow(1.f - cos, 5.f);

      col += schlick * reflectCol + (1.f - schlick) * refractCol;
      return col;
    }
  }

  return col + reflectCol;
}
