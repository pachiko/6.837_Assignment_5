#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include "RayTracer.h"
#include <string.h>
#include "bitmap_image.hpp"

using namespace std;

void filterImage(Image*& image, bool vert=false);

int main(int argc, char *argv[])
{
    // Fill in your implementation here.

    bool showDepth = false;
    Image *depthIm;
    string depthFileName;
    float minDepth, maxDepth, depthRange;

    bool showNormals = false;
    Image *normalIm;
    string normalFileName;

    // RayTracer args
    bool shadows = false;
    int max_bounces = 4;
    bool jitter = false;
    bool filter = false;

    // This loop loops over each of the input arguments.
    // argNum is initialized to 1 because the first
    // "argument" provided to the program is actually the
    // name of the executable (in our case, "a4").
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;

        if (strcmp(argv[argNum], "-depth") == 0)
        {
            showDepth = true;
            minDepth = stof(argv[argNum + 1]);
            maxDepth = stof(argv[argNum + 2]);
            depthRange = maxDepth - minDepth;
            depthFileName = argv[argNum + 3];
        }
        else if (strcmp(argv[argNum], "-normals") == 0)
        {
            showNormals = true;
            normalFileName = argv[argNum + 1];
        }
        else if (strcmp(argv[argNum], "-bounces") == 0)
        {
            max_bounces = clamp(stoi(argv[argNum + 1]), 0, 6);
        }
        else if (strcmp(argv[argNum], "-shadows") == 0)
        {
            shadows = true;
        }
        else if (strcmp(argv[argNum], "-jitter") == 0)
        {
            jitter = true;
        }
        else if (strcmp(argv[argNum], "-filter") == 0)
        {
            filter = true;
        }
    }

    // arguments are spaced-separated

    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.

    SceneParser parser(argv[2]);
    int w = stoi(argv[4]);
    int h = stoi(argv[5]);

    // 3x3 sub-grids per pixel
    if (jitter)
    {
        w *= 3;
        h *= 3;
    }

    // Can't use reference, since can't change from original to filtered Image
    // we must create a new Image for filtering
    Image* image = new Image(w, h);

    RayTracer rt(&parser, max_bounces, shadows);

    if (showDepth)
    {
        depthIm = new Image(w, h);
    }
    if (showNormals)
    {
        normalIm = new Image(w, h);
    }

    Camera *cam = parser.getCamera();
    float tmin = cam->getTMin();

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            float dx = jitter? (rand() / RAND_MAX - 0.5f) : 0.5f;
            float dy = jitter? (rand() / RAND_MAX - 0.5f) : 0.5f;

            Vector2f p((j + dx) * 2.f / w - 1.f, (i + dy) * 2.f / h - 1.f);
            Ray r = cam->generateRay(p);
            Hit h;

            Vector3f col = rt.traceRay(r, tmin, 0, 1.f, h); // we start from 0 bounce and air
            image->SetPixel(j, i, col);

            if (showDepth)
            {
                float depth = (maxDepth - clamp(h.getT(), minDepth, maxDepth)) / depthRange;
                depthIm->SetPixel(j, i, depth * Vector3f(1.f, 1.f, 1.f));
            }
            if (showNormals)
            {
                Vector3f n = h.getNormal();
                if (n[0] < 0)
                    n[0] = -n[0];
                if (n[1] < 0)
                    n[1] = -n[1];
                if (n[2] < 0)
                    n[2] = -n[2];
                normalIm->SetPixel(j, i, n);
            }
        }
    }


    if (filter)
    {
        filterImage(image, false);
        filterImage(image, true);
    }


    if (jitter)
    {
        Image finalImage(w/3, h/3);

        for (int i = 0; i < h; i+=3)
        {
            for (int j = 0; j < w; j+=3)
            {
                Vector3f col;

                for (int k = 0; k < 3; k++)
                {
                    for (int l = 0; l < 3; l++)
                    {
                        col += image->GetPixel(j + l, i + k);
                    }
                }

                col *= (1.f/9.f);
                finalImage.SetPixel(j/3, i/3, col);
            }
        }

        finalImage.SaveImage(argv[7]);
    }
    else
    {
        image->SaveImage(argv[7]);
    }
    delete image;

    if (showDepth)
    {
        depthIm->SaveImage(&depthFileName[0]);
        delete depthIm;
    }

    if (showNormals)
    {
        normalIm->SaveImage(&normalFileName[0]);
        delete normalIm;
    }

    return 0;
}


// 1D Gaussian Filter of image
void filterImage(Image*& image, bool vert) {
    float filt[5] = {0.1201f, 0.2339f, 0.2931f, 0.2339f, 0.1201f};

    int w = image->Width();
    int h = image->Height();

    Image* filtImage = new Image(w, h);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            Vector3f col;

            for (int k = -2;  k < 3; k++) {
                int kk = (vert? i : j) + k;

                if (kk < (vert? h : w) && kk >= 0) {
                    col += (filt[k+2])*(image->GetPixel((vert? j : kk), (vert? kk : i)));
                }
            }

            filtImage->SetPixel(j, i, col);
        }
    }

    delete image;
    image = filtImage;
}