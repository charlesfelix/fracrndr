//
//  Parser.cpp
//  FracRndr
//
//  Created by Charles-Felix on 10/23/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include <fstream>
#include <string>
#include "json.hpp"
//#include <glog/logging.h>
#include "easylogging++.h"

#include "Parser.hpp"
#include "ImageBuffer.hpp"
#include "ImageFileExr.hpp"
#include "Sphere.hpp"




using namespace Fr;

using json = nlohmann::json;

#define JSONGETI(jit,token,dv) int default_##token = dv; int token = jsonGet(#token,jit,default_##token)

#define JSONGETSTR(jit,token,dv) std::string default_##token = dv; std::string token = jsonGet(#token,jit,default_##token)

#define JSONGETF(jit,token,dv) float default_##token = dv; float token = jsonGet(#token,jit,default_##token)

#define JSONGETF3(jit,token,dv0,dv1,dv2) std::vector<float> default_##token = {dv0, dv1, dv2}; std::vector<float> __##token = jsonGet(#token,jit,default_##token); V3f token = V3f(__##token[0],__##token[1],__##token[2])


int JsonParser::parse(const std::string & filepath)
{
    std::ifstream f = std::ifstream(filepath.c_str());
    
    json j = json::parse(f);
    
    
    std::cout << j.dump() << std::endl;
    {
        parseGlobals(j);
        parseCamera(j);
        parseFilm(j);
        parseBackground(j);
        parseMaterials(j);
        parsePrimitives(j);
        parseMaterialTable(j);
    }
    return -1;
}

void JsonParser::parseGlobals(const json & j)
{
    json::const_iterator jit = j.find("globals");
    
    if (jit != j.end())
    {
        JSONGETI(jit,aa,1);
        JSONGETI(jit,max_ray_depth,1);
        JSONGETSTR(jit,outputfile,"/tmp/render.exr");
        
        LOG(INFO) << "globals";
        LOG(INFO) << "\t aa" << aa;
        LOG(INFO) << "\t max_ray_depth " << max_ray_depth;
        LOG(INFO) << "\t output_file" << outputfile;
        
        
        m_globals = std::make_shared<RenderGlobals>();
        m_globals->m_aa = aa;
        m_globals->m_max_ray_depth = max_ray_depth;
        m_globals->m_output_file = outputfile;
    }
    else
    {
        LOG(WARNING) << "JsonParser: no globals block in scene file.";
    }

}

void JsonParser::parseCamera(const json & j)
{
    json::const_iterator jit = j.find("camera");
    
    if (jit != j.end())
    {
        JSONGETF3(jit,position,0.f,0.f,0.f);
        JSONGETF3(jit,rotation,0.f,0.f,0.f);
        JSONGETF(jit,focal,1.f);
        JSONGETF(jit,aspect_ratio,1.f);
        JSONGETF(jit,fov,45.f);
        JSONGETF(jit,near,1.f);
        JSONGETF(jit,far,1.f);
        
        LOG(INFO) << "camera ";
        LOG(INFO) << "\t position " << position.x << " " << position.y << " " << position.z;
        LOG(INFO) << "\t focal " << focal;
        LOG(INFO) << "\t aspect_ratio " << aspect_ratio;
        LOG(INFO) << "\t fov " << fov;
        LOG(INFO) << "\t near " << near;
        LOG(INFO) << "\t near " << far;
        
        m_camera = std::make_shared<Camera>(focal,aspect_ratio,fov,near,far);
        m_camera->setPosition(position);
        M44f xform = m_camera->getTransform();
        xform.rotate(rotation);
        m_camera->setTransform(xform);
        
        LOG(INFO) << "\n" <<  xform;
    }
    else
    {
        LOG(WARNING) << "JsonParser: no camera block in scene file.";
    }

}

void JsonParser::parseFilm(const json & j)
{
    json::const_iterator jit = j.find("film");
    
    if (jit != j.end())
    {
        JSONGETI(jit,width, 640);
        JSONGETI(jit,height, 480);
        
        LOG(INFO) << "film";
        LOG(INFO) << "\t width " << width;
        LOG(INFO) << "\t height " << height;
        
        m_film = std::make_shared<Film>(width,height);
    }
    else
    {
        LOG(WARNING) << "JsonParser: no film block in scene file.";
    }
}

void JsonParser::parseBackground(const json & j)
{
    json::const_iterator jit = j.find("background");
    
    if (jit != j.end())
    {
        JSONGETSTR(jit,imagefile, "noimage");
        
        LOG(INFO) << "background";
        LOG(INFO) << "\t " << imagefile;
        
        ImageBuffer::Ptr imgbuf_ptr = std::make_shared<ImageBuffer>(1,1);
        ImageFileExr().read(imagefile,*imgbuf_ptr);
        ImageTexture::Ptr imgtex_ptr = std::make_shared<ImageTexture>(imgbuf_ptr);
        m_bg = std::make_shared<Background>(imgtex_ptr);
    }
    else
    {
        LOG(WARNING) << "JsonParser: no background block in scene file.";
    }

}


void JsonParser::parseMaterials(const json & j)
{
    json::const_iterator jit = j.find("materials");
    
    if (jit != j.end())
    {
        for (json::const_iterator it = jit->begin(); it != jit->end(); ++it) {
            
            LOG(INFO) << "material : " << it.key() << " - " << it.value();
            
            std::string material_tag = it.key();

            JSONGETSTR(it,name,"nodefault");
            
            if (m_materials.find(name) == m_materials.end())
            {
                if (name == "lambertian") // parse lambertian
                {
                    JSONGETF3(it,albedo,0.99,0.01,0.99);
                    Material::Ptr mat_ptr = Material::Ptr(new Lambertian(albedo));
                    m_materials[material_tag] = mat_ptr;
                }
                else if (name == "glass") // parse glass
                {
                    JSONGETF(it,refraction_index,1/1.33f);
                    Material::Ptr mat_ptr = Material::Ptr(new Glass(refraction_index));
                    m_materials[material_tag] = mat_ptr;
                }
                else if (name == "simplemetal") // parse lambertian
                {
                    JSONGETF3(it,albedo,0.99,0.01,0.99);
                    JSONGETF(it,roughness,.33f);
                    Material::Ptr mat_ptr = Material::Ptr(new SimpleMetal(albedo,roughness));
                    m_materials[material_tag] = mat_ptr;
                }
                else
                {
                    LOG(WARNING) << "JsonParser: unknown material " << name << " ... skipped";
                }
            
            }
            else
            {
                    LOG(WARNING) << "JsonParser: skipping material, duplicate key: " << material_tag;
            }
        }
        
        LOG(INFO) << "material list";
        for (auto it = m_materials.begin(); it != m_materials.end(); ++it)
            LOG(INFO) << "\t-" << it->first;
        
    }
    else
    {
        LOG(WARNING) << "JsonParser: no materials block in scene file.";
    }
}



void JsonParser::parsePrimitives(const json & j)
{
    json::const_iterator jit = j.find("primitives");
    
    if (jit != j.end())
    {
        for (json::const_iterator it = jit->begin(); it != jit->end(); ++it) {
            LOG(INFO) << "primitive : " << it.key() << " - " << it.value();
            
            std::string primitive_tag = it.key();
            
            JSONGETSTR(it,name,"nodefault");
            
            if (m_primitives.find(primitive_tag) == m_primitives.end())
            {
                if (name == "sphere") // parse lambertian
                {
                    JSONGETF3(it,position,0.99f,0.01f,0.99f);
                    JSONGETF(it,radius,1.f);
                    RenderPrimitve::Ptr prim_ptr =
                        RenderPrimitve::Ptr(new Sphere(position,radius));
                    m_primitives[primitive_tag] = prim_ptr;
                }
                else
                {
                    LOG(WARNING) << "JsonParser: unknown primitive " << name << " ... skipped";
                }
            }
            else
            {
                LOG(WARNING) << "JsonParser: skipping material, duplicate key: " << primitive_tag;
            }
        }
        
        LOG(INFO) << "primitives list";
        for (auto it = m_primitives.begin(); it != m_primitives.end(); ++it)
            LOG(INFO) << "\t-" << it->first;
        
    }
    else
    {
        LOG(WARNING) << "JsonParser: no primitives block in scene file.";
    }
}

void JsonParser::parseMaterialTable(const json & j)
{
    json::const_iterator jit = j.find("material_table");
    
    if (jit != j.end())
    {
        for (json::const_iterator it = jit->begin(); it != jit->end(); ++it) {
            LOG(INFO) << "material bind : " << it.key() << " <- " << it.value();
            m_material_table[it.key()] = it.value();

        }
    }
    else
    {
        LOG(WARNING) << "JsonParser: no material_table block in scene file.";
    }

}

const std::shared_ptr<RenderGlobals> & JsonParser::getGlobals() const
{
    return m_globals;
}
const Camera::Ptr & JsonParser::getCamera() const
{
    return m_camera;
}
const Scene::Ptr & JsonParser::getScene() const
{
    return m_scene;
}
const Film::Ptr & JsonParser::getFilm() const
{
    return m_film;
}

const Background::Ptr & JsonParser::getBackground() const
{
    return m_bg;
}

const std::map<std::string,Material::Ptr > & JsonParser::getMaterials() const
{
    return m_materials;
};

const std::map<std::string,RenderPrimitve::Ptr > & JsonParser::getPrimitives() const
{
    return m_primitives;
}

const std::map<std::string, std::string> & JsonParser::getMaterialTable() const
{
    return m_material_table;
}
