//
//  main.cpp
//  FracRndr
//
//  Created by Charles-Felix on 4/10/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//
#include <iostream>
#include <tbb/tbb.h>
#include <tbb/mutex.h>
#include "frMath.h"
#include "Camera.hpp"
#include "ImageBuffer.hpp"
#include "ImageFilePng.hpp"
#include "ImageFileExr.hpp"
#include "Sphere.hpp"
#include "Sampler.hpp"
#include "Material.hpp"
#include "Mandelbulb.hpp"
#include "DistanceField.hpp"

//TODO: Ray Differentials
//TODO: UVs & Pattern material to test Ray differentials
//TODO: Importance Sampling

using namespace Fr;

class ProgressReport
{
public:
    
};

class Background
{
public:
    Background(const std::shared_ptr<ImageBuffer> & imgbuf_ptr):m_imgbuf_ptr(imgbuf_ptr)
    {
    }
    C4f color ( const Ray & r ) const
    {
        V3f d = r.direction.normalized();
        float u = .5f*(1.f + atan2(d.x,-d.z) / M_PI);
        float v = acos(d.y) / M_PI;
        
        return m_imgbuf_ptr->getPixel(u*(m_imgbuf_ptr->width()-1), v*(m_imgbuf_ptr->height()-1));
    }
    
    
private:
    std::shared_ptr<ImageBuffer> m_imgbuf_ptr;
};

C4f color ( const Ray & r,const PrimitiveList & prim_list, const Background & bg)
{
    // terminate the ray
    if (r.depth>4 ) return C4f(0.f,0.f,0.f,0.f);

    HitRecord hit_record;

    bool has_hit = prim_list.hit(r,0.001f,MAXFLOAT,hit_record);
    //V3f cn = 0.5f*(hit_record.normal+V3f(1.f,1.f,1.f));
    //return C4f(cn.x,cn.y,cn.z,1.f);
    if (has_hit)
    {
        Ray ray_scattered;
        C3f attenuation;
        if (hit_record.material!=nullptr && hit_record.material->scatter(r, hit_record, attenuation, ray_scattered))
        {
            C4f c = color(ray_scattered,prim_list,bg);

            return C4f(attenuation.x*c.r,attenuation.y*c.g,attenuation.z*c.b,1.0f);
        }
        return C4f();
    }
    else
    {
        C4f c = bg.color(r);
        c.a = .0f;
        return c;
    }
}

void outputProgress(float progress)
{
    
    static tbb::mutex mutx;
    tbb::mutex::scoped_lock lock;
    
    lock.acquire(mutx);
    {
        int barWidth = 70;
        int pos = barWidth * progress;
        std::cout << "[";
        
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();
    }
    lock.release();
}

void renderScene()
{
    FR_RAND48 random = FR_RAND48::Rand48(123);
    
    const char *file_in = "/Users/charles-felix/Documents/Development/FracRndr/Images/uffizi-large.exr";
    
    std::shared_ptr<ImageBuffer> uffizi_ptr = std::make_shared<ImageBuffer>(1,1);
    ImageFileExr().read(file_in,*uffizi_ptr);
    
    Background bg(uffizi_ptr);

    size_t w = 1920;
    size_t h = 1080;
    ImageBuffer buf(w,h,Fr::C4f(0.f,0.f,0.f,1.f));
    float aspect_ratio = w/float(h);
    Camera cam = Camera(1.f,aspect_ratio);
    cam.setPosition({0.f,0.f,2.f});
    
    std::shared_ptr<Material> m0 = std::shared_ptr<Material>(new Lambertian(C3f(0.5,0.5,0.5)));
    std::shared_ptr<Material> m1 = std::shared_ptr<Material>(new SimpleMetal(C3f(.8f,.8f,.8f)));
    std::shared_ptr<Material> m2 = std::shared_ptr<Material>(new Lambertian(C3f(0.6,0.6,0.6)));
    std::shared_ptr<Material> m3 = std::shared_ptr<Material>(new SimpleMetal(C3f(0.6,0.6,0.6),.2f));
    
    PrimitiveList prim_list;
    
    std::shared_ptr<Primitive> p0 = std::shared_ptr<Primitive>(new Sphere(V3f(-0.2f,0.f,0.f),0.25f,m0));
    prim_list.addPrimitive(p0);
    
    std::shared_ptr<Primitive> p1 = std::shared_ptr<Primitive>(new Sphere(V3f(0.6f,0.f,0.f),0.5f,m1));
    prim_list.addPrimitive(p1);
    
    std::shared_ptr<Primitive> p2 = std::shared_ptr<Primitive>(new Sphere(V3f(0.5f,-100.5f,0.f),100.f,m2));
    prim_list.addPrimitive(p2);
    
    std::shared_ptr<Primitive> p3 = std::shared_ptr<Primitive>(new Sphere(V3f(-0.75f,0.f,0.f),0.3f,m3));
    prim_list.addPrimitive(p3);

    
    const float w_step = 1.f/float(w);
    const float h_step = 1.f/float(h);
    V2f ndc(0.f,0.f);
    size_t n_samples = 1000;
    float avger = 1.f/float(n_samples);
    tbb::atomic<float> progress_increment = 1.f/float(h);
    float progress = 0;
    for (size_t y = 0; y < h ; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            C4f c(0.f,0.f,0.f,1.f);
            for (size_t s = 0; s < n_samples; ++s)
            {
                V2f sample_offset;
                sample_offset.x = fit(float(random.nextf()),0.f,1.f,-1.f,1.f)*w_step*0.5f;
                sample_offset.y = fit(float(random.nextf()),0.f,1.f,-1.f,1.f)*w_step*0.5f;
                Ray r = cam.unproject(ndc+sample_offset);
                c += color(r,prim_list,bg);
            }
            c *= avger;
            buf.setPixel(x, buf.height()-y-1, c);
            ndc.x += w_step;
        }
        
        ndc.y+=h_step;
        ndc.x = 0;
        outputProgress(progress);
        progress+=progress_increment;
    }
    
    ImageFileExr fout;
    fout.write("/Users/charles-felix/Documents/Development/FracRndr/Images/render_scene1080_2.exr",buf);
    
}

void renderScene1()
{
    Sampler sampler(123);
    
    const char *file_in = "/Users/charles-felix/Documents/Development/FracRndr/Images/uffizi-large.exr";
    
    std::shared_ptr<ImageBuffer> uffizi_ptr = std::make_shared<ImageBuffer>(1,1);
    ImageFileExr().read(file_in,*uffizi_ptr);
    
    Background bg(uffizi_ptr);
    
    size_t w = 512;
    size_t h = 256;
    ImageBuffer buf(w,h,Fr::C4f(0.f,0.f,0.f,1.f));
    float aspect_ratio = w/float(h);
    Camera cam = Camera(1.f,aspect_ratio);
    cam.setPosition({0.f,0.f,2.f});
    
    std::shared_ptr<Material> m0 = std::shared_ptr<Material>(new Lambertian(C3f(0.5,0.5,0.5)));
    std::shared_ptr<Material> m1 = std::shared_ptr<Material>(new SimpleMetal(C3f(.8f,.8f,.8f)));
    std::shared_ptr<Material> m2 = std::shared_ptr<Material>(new Lambertian(C3f(0.6,0.6,0.6)));
    std::shared_ptr<Material> m3 = std::shared_ptr<Material>(new Glass(1.4,.2f));
    std::shared_ptr<Material> m4 = std::shared_ptr<Material>(new Glass(0.f,.2f));
    
    PrimitiveList prim_list;
    
    std::shared_ptr<Primitive> p0 = std::shared_ptr<Primitive>(new Sphere(V3f(-0.2f,0.f,0.f),0.25f,m0));
    //prim_list.addPrimitive(p0);
    
    std::shared_ptr<Primitive> p1 = std::shared_ptr<Primitive>(new Sphere(V3f(0.6f,0.f,0.f),0.5f,m3));
    prim_list.addPrimitive(p1);
    std::shared_ptr<Primitive> p2 = std::shared_ptr<Primitive>(new Sphere(V3f(0.6f,0.f,0.f),0.25f,m4));
    prim_list.addPrimitive(p2);
    
    std::shared_ptr<Primitive> p4 = std::shared_ptr<Primitive>(new Sphere(V3f(0.5f,-100.5f,0.f),100.f,m2));
    prim_list.addPrimitive(p4);
    
    std::shared_ptr<Primitive> p3 = std::shared_ptr<Primitive>(new Sphere(V3f(-0.75f,0.f,0.f),0.3f,m3));
    //prim_list.addPrimitive(p3);
    
    
    const float w_step = 1.f/float(w);
    const float h_step = 1.f/float(h);
    V2f ndc(0.f,0.f);
    size_t n_samples = 2;
    float avger = 1.f/float(n_samples);
    float progress_increment = 1.f/float(h);
    tbb::atomic<float> progress = 0.f;
    
    //tbb::task_scheduler_init init(2);
    tbb::parallel_for(size_t(0),size_t(h),size_t(1),[&,sampler](size_t y){
        for (size_t x = 0; x < w; ++x)
        {
            C4f c(0.f,0.f,0.f,1.f);
            V2f ndc(float(x)*w_step,float(y)*h_step);
            for (size_t s = 0; s < n_samples; ++s)
            {
                V2f sample_offset;
                sample_offset.x = fit(float(sampler.random()),0.f,1.f,-1.f,1.f)*w_step*0.5f;
                sample_offset.y = fit(float(sampler.random()),0.f,1.f,-1.f,1.f)*w_step*0.5f;
                Ray r = cam.unproject(ndc+sample_offset);
                c += color(r,prim_list,bg);
            }
            c *= avger;
            buf.setPixel(x, buf.height()-y-1, c);
            
        }
        
        ndc.y+=h_step;
        ndc.x = 0.f;
        outputProgress(progress);
        progress=progress+progress_increment;
    });
    
    ImageFileExr fout;
    fout.write("/Users/charles-felix/Documents/Development/FracRndr/Images/render_scene_glass2.exr",buf);
    
}


void renderScene2()
{
    FR_RAND48 random = FR_RAND48::Rand48(123);
    
    const char *file_in = "/Users/charles-felix/Documents/Development/FracRndr/Images/uffizi_large.exr";
    
    std::shared_ptr<ImageBuffer> uffizi_ptr = std::make_shared<ImageBuffer>(1,1);
    ImageFileExr().read(file_in,*uffizi_ptr);
 
    Background bg(uffizi_ptr);
    
    Camera cam = Camera(1.f,2.f);
    cam.setPosition({0.f,0.f,2.5f});
    
    std::shared_ptr<Material> m0 = std::shared_ptr<Material>(new Lambertian(C3f(0.5,0.5,0.5)));
    std::shared_ptr<Material> m1 = std::shared_ptr<Material>(new SimpleMetal(C3f(.8f,.8f,.8f)));
    std::shared_ptr<Material> m2 = std::shared_ptr<Material>(new Lambertian(C3f(0.6,0.6,0.6)));
    std::shared_ptr<Material> m3 = std::shared_ptr<Material>(new SimpleMetal(C3f(0.6,0.6,0.6),.2f));

    PrimitiveList prim_list;
    SphereDE sde(V3f(0.0f,0.f,0.f),3.0f,0.001);
    Mandelbulb mb = Mandelbulb(8, V3f(0.f,0.f,0.f), V3f(0.f,0.f,0.f),10);
    std::shared_ptr<Primitive> p0 = std::shared_ptr<Primitive>(new ImplicitDistanceField<Mandelbulb>(mb,100,m0));
    std::shared_ptr<Primitive> p4 = std::shared_ptr<Primitive>(new ImplicitDistanceField<SphereDE>(sde,100,m0));
    prim_list.addPrimitive(p0);
    
    std::shared_ptr<Primitive> p1 = std::shared_ptr<Primitive>(new Sphere(V3f(0.0f,0.f,0.f),3.0f,m1));
    //prim_list.addPrimitive(p1);

    std::shared_ptr<Primitive> p2 = std::shared_ptr<Primitive>(new Sphere(V3f(0.5f,-100.5f,0.f),100.f,m2));
    prim_list.addPrimitive(p2);

    std::shared_ptr<Primitive> p3 = std::shared_ptr<Primitive>(new Sphere(V3f(-1.75f,0.f,0.f),.8f,m1));
    prim_list.addPrimitive(p3);
    
    
    size_t w = 512*0.25;
    size_t h = w/2;
    ImageBuffer buf(w,h,Fr::C4f(0.f,0.f,0.f,1.f));
    const float w_step = 1.f/float(w);
    const float h_step = 1.f/float(h);
    V2f ndc(0.f,0.f);
    size_t n_samples = 1024;
    float avger = 1.f/float(n_samples);
    float progress_increment = 1.f/float(h);
    float progress = 0;
    for (size_t y = 0; y < h ; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            C4f c(0.f,0.f,0.f,1.f);
            for (size_t s = 0; s < n_samples; ++s)
            {
                V2f sample_offset;
                sample_offset.x = fit(float(random.nextf()),0.f,1.f,-1.f,1.f)*w_step*0.5f;
                sample_offset.y = fit(float(random.nextf()),0.f,1.f,-1.f,1.f)*w_step*0.5f;
                Ray r = cam.unproject(ndc+sample_offset);
                c += color(r,prim_list,bg);
            }
            c *= avger;
            buf.setPixel(x, buf.height()-y-1, c);
            ndc.x += w_step;
        }

        ndc.y+=h_step;
        ndc.x = 0;
        outputProgress(progress);
        progress+=progress_increment;
    }
    
    ImageFilePng fout;
    fout.write("/Users/charles-felix/Documents/Development/FracRndr/Images/scene7.png",buf);

}

void testFileExr()
{
    const char *file_in = "/Users/charles-felix/Documents/Development/FracRndr/Images/uffizi-large.exr";
    const char *file_out = "/Users/charles-felix/Documents/Development/FracRndr/Images/exr-test.png";

    ImageBuffer imgbuf(1,1);
    ImageFileExr().read(file_in,imgbuf);
    ImageFileExr().write(file_out,imgbuf);
}

void testFileExrMM()
{
    const char *file_in = "/Users/charles-felix/Documents/Development/FracRndr/Images/uffizi-large-mm.exr";
    const char *file_out = "/Users/charles-felix/Documents/Development/FracRndr/Images/exr-mmtest.exr";
    
    MipMapBuffer mmbuf;
    ImageFileExr().readmm(file_in,mmbuf);
    std::cout << mmbuf.level(3)->width() << std::endl;
    ImageFileExr().write(file_out,*(mmbuf.level(10)));

}
void
testSphereHit()
{
    std::shared_ptr<Material> m0 = std::shared_ptr<Material>(new Lambertian(C3f(0.5,0.5,0.5)));
    Sphere sphere(V3f(0.0f,0.f,0.f),0.25f,m0);
    
    Ray r = Ray(V3f(0.f,0.f,1.1f), V3f(0,0,1));
    float t;
    bool hit = sphere.intersect(r, t);

    std::cout << "intersection " << hit << " " << t << std::endl;
}


void testTbb()
{
    tbb::parallel_for(0, 10, 1, [](int y){int x = y;});
}

int main(int argc, const char * argv[]) {
    //renderScene();
    //testFileExr();
    //testFileExrMM();
    renderScene1();
    
    //testSphereHit();
    return 0;
}
