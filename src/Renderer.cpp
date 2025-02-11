//
//  Renderer.cpp
//  FracRndr
//
//  Created by Charles-Felix on 10/22/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include <tbb/tbb.h>
#include <tbb/mutex.h>

#include "Renderer.hpp"
#include "Material.hpp"
#include "Sampler.hpp"
#include "Parser.hpp"

using namespace Fr;


RenderGlobals::RenderGlobals():m_aa(1),m_max_ray_depth(4),m_output_file("/tmp/render.exr")
{
    
}

void Renderer::outputProgress(float progress) const
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

Renderer::Renderer()
{
    
}

void Renderer::setRenderGlobals(const Fr::RenderGlobals &rg)
{
    m_render_globals = rg;
}

Camera::ConstPtr Renderer::getCamera() const
{
    return m_camera;
}

void Renderer::setCamera(const Camera::Ptr &camera)
{
    m_camera = camera;
}


Film::ConstPtr Renderer::getFilm() const
{
    return m_film;
}

void Renderer::setFilm(const Film::Ptr & film)
{
    m_film = film;
}


std::shared_ptr<const Scene> Renderer::getScene() const
{
    return m_scene;
}

void Renderer::setScene(const Scene::Ptr & scene)
{
    m_scene = scene;
}

C4f Renderer::Li(const Fr::Ray &r, const RenderPrimitve::Ptr & primitives, const Background &bg) const {
    
    // terminate the ray
    if (r.depth> m_render_globals.m_max_ray_depth ) return C4f(0.f,0.f,0.f,0.f);
    HitRecord hit_record;
    bool has_hit = primitives->hit(r,0.001f,MAXFLOAT,hit_record);
    if (has_hit)
    {
        Ray ray_scattered;
        C3f attenuation;
        if (hit_record.material!=nullptr && hit_record.material->scatter(r, hit_record, attenuation, ray_scattered))
        {
            C4f c = this->Li(ray_scattered,primitives,bg);
            
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

void Renderer::render() const
{
    // TODO: assert that all the scene's data isn't null.
    
    //FR_RAND48 random = FR_RAND48::Rand48(123);
    Sampler sampler(123);
   
    const size_t n_samples = m_render_globals.m_aa*m_render_globals.m_aa;
    const size_t h = m_film->height();
    const size_t w = m_film->width();
    const float w_step = 1.f/float(w);
    const float h_step = 1.f/float(h);

    V2f ndc(0.f,0.f);
    
    float avger = 1.f/float(n_samples);

    float progress_increment = 1.f/float(h);
    tbb::atomic<float> progress = 0.f;
    //tbb::task_scheduler_init init(1);

    tbb::parallel_for(size_t(0), size_t(h), size_t(1),[&,sampler](size_t y) {
    
        for (size_t x = 0; x < w; ++x)
        {
            C4f c(0.f,0.f,0.f,1.f);
            V2f ndc(float(x)*w_step,float(y)*h_step);
            for (size_t s = 0; s < n_samples; ++s)
            {
                V2f sample_offset;
                sample_offset.x = fit(float(sampler.random()),0.f,1.f,-1.f,1.f)*w_step*0.5f;
                sample_offset.y = fit(float(sampler.random()),0.f,1.f,-1.f,1.f)*w_step*0.5f;
                
                Ray r = m_camera->unproject(ndc+sample_offset);
                
                c += this->Li(r,m_scene->getPrimitives(),*(m_scene->getBackground()));
            }
            c *= avger;
            m_film->addSample(x, h-y-1, c);
        }
        
        ndc.y+=h_step;
        ndc.x = 0;
        this->outputProgress(progress);
        progress=progress+progress_increment;
        
    });
    
    m_film->writeImage(m_render_globals.m_output_file);
}

void Renderer::initFromFile(const std::string &scenefile_path)
{
    JsonParser parser;
    parser.parse(scenefile_path);
    
    m_render_globals = *parser.getGlobals(); // all members are values, it's fine
    m_camera = parser.getCamera();
    m_film = parser.getFilm();
    
    // create the scene
    // - add background
    Scene::Ptr scene = std::make_shared<Scene>();
    
    scene->setBackground(parser.getBackground());
    
    // - create the primitive list
    std::map<std::string, RenderPrimitve::Ptr> primitives = parser.getPrimitives();
    PrimitiveList::Ptr plist = std::make_shared<PrimitiveList>();
    for (auto it = primitives.begin(); it != primitives.end(); ++it)
    {
        plist->addPrimitive(it->second);
    }
    
    // - bind the materials
    std::map<std::string, Material::Ptr> materials = parser.getMaterials();
    std::map<std::string, std::string> material_table = parser.getMaterialTable();
    
    for (auto it = material_table.begin(); it != material_table.end(); ++it)
    {
        primitives[it->first]->setMaterial(materials[it->second]);
    }
    
    // assign the primitive list
    scene->setPrimitives(plist);
    
    // assign the scene
    this->setScene(scene);
}
