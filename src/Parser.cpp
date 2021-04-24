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
#include "TriMesh.hpp"

#include "tinyobjloader.hpp"


using namespace Fr;

using json = nlohmann::json;

#define JSONGETI(jit,token,dv) int default_##token = dv;\
    int token = jsonGet(#token,jit,default_##token)

#define JSONGETSTR(jit,token,dv) std::string default_##token = dv; \
    std::string token = jsonGet(#token,jit,default_##token)

#define JSONGETF(jit,token,dv) float default_##token = dv; \
    float token = jsonGet(#token,jit,default_##token)

#define JSONGETF3(jit,token,dv0,dv1,dv2) std::vector<float> default_##token = {dv0, dv1, dv2}; \
    std::vector<float> __##token = jsonGet(#token,jit,default_##token); \
    V3f token = V3f(__##token[0],__##token[1],__##token[2])


int JsonParser::parse(const std::string & filepath)
{
    std::ifstream f = std::ifstream(filepath.c_str());
    
    if (f.good())
    {
        LOG(INFO) << "Parsing " << filepath;
        try
        {
            json j = json::parse(f);
        
            std::cout << j.dump() << std::endl;
            
            {
                parseGlobals(j);
                parseFilm(j);
                parseCamera(j);
                parseBackground(j);
                parseMaterials(j);
                parsePrimitives(j);
                parseMaterialTable(j);
            }
        
        }
        catch (std::exception & e)
        {
            LOG(ERROR) << "JsonParser::parse: parse error:" << std::endl;
            LOG(ERROR) << e.what() << std::endl;
            return 1;
        }
    }
    else
    {
        LOG(ERROR) << "JsonParser::parse: Missing File: " << filepath << std::endl;
        return 1;
    }
    
    return 0;
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
    
        // check intputs
        // - is output file valid
        // - is aa valid
        // - is max_ray_depth valid
        std::fstream of = std::fstream(outputfile);
        if (!of.good())
        {
            throw std::invalid_argument("JsonParser::parseGlobals:: output_file: invalid path");
        }
       
        if (aa < 1)
            throw std::invalid_argument("JsonParser::parseGlobals:: aa < 1. aa should be greater than 0");

        if (max_ray_depth < 1)
            throw std::invalid_argument("JsonParser::parseGlobals:: max_ray_depth < 1. aa should be greater than 0");
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
        float aspect_ratio = m_film->width()/float(m_film->height());
        
        JSONGETF3(jit,position,0.f,0.f,0.f);
        JSONGETF3(jit,rotation,0.f,0.f,0.f);
        JSONGETF(jit,focal,50.f);
        JSONGETF(jit,aperture,41.4241f);
        JSONGETF(jit,near,.1f);
        JSONGETF(jit,far,1000.f);
        
        LOG(INFO) << "camera ";
        LOG(INFO) << "\t position " << position.x << " " << position.y << " " << position.z;
        LOG(INFO) << "\t focal " << focal;
        LOG(INFO) << "\t aperture " << focal;
        LOG(INFO) << "\t aspect_ratio " << aspect_ratio;
        //LOG(INFO) << "\t fov " << fov;
        LOG(INFO) << "\t near " << near;
        LOG(INFO) << "\t far " << far;
        
        if (near > far)
            throw std::invalid_argument("JsonParser::parseCamera:: near should be greater than far");

        if (near < 0.001f)
            throw std::invalid_argument("JsonParser::parseCamera:: near should be greater than 0.001f");
        
        m_camera = std::make_shared<Camera>(focal,aperture,aspect_ratio,near,far);
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

        if (width < 1)
            throw std::invalid_argument("JsonParser::parseFilm:: invalid width, should be greater than 1");
        if (height < 1)
            throw std::invalid_argument("JsonParser::parseFilm:: invalid height, should be greater than 1");
        
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
        JSONGETF3(jit,vcolor,1.f,1.f,1.f);
        C4f color = C4f(vcolor.x,vcolor.y,vcolor.z,1.f);
        LOG(INFO) << "background";
        LOG(INFO) << "\t " << imagefile;
        
        // check if the imagefile exists
        std::ifstream f = std::ifstream(imagefile);
        if (f.good())
        {
            ImageBuffer::Ptr imgbuf_ptr = nullptr;
            ImageTexture::Ptr imgtex_ptr = nullptr;
            if (imagefile != "noimage")
            {
                imgbuf_ptr = std::make_shared<ImageBuffer>(1,1);
                ImageFileExr().read(imagefile,*imgbuf_ptr);
                imgtex_ptr = std::make_shared<ImageTexture>(imgbuf_ptr);
            }
            m_bg = std::make_shared<Background>(imgtex_ptr,color);
        }
        else
        {
            throw std::invalid_argument("JsonParser::parseBackground:: invalid background image path.");
        }
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
                else if (name == "simplemetal") // parse simplemetal
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
                if (name == "sphere") // parse sphere
                {
                    JSONGETF3(it,position,0.99f,0.01f,0.99f);
                    JSONGETF(it,radius,1.f);
                    RenderPrimitive::Ptr prim_ptr =
                        RenderPrimitive::Ptr(new Sphere(position,radius));
                    m_primitives[primitive_tag] = prim_ptr;
                }
                else if (name == "box") // parse box
                {
                    JSONGETF3(it,position,0.99f,0.01f,0.99f);
                    JSONGETF(it,width,1.f);
                    std::vector<V3f> box_positions;
                    box_positions.resize(8);
                    box_positions[0] = {0.f,0.f,0.f}; box_positions[0] = box_positions[0]*width + position;
                    box_positions[1] = {0.f,1.f,0.f}; box_positions[1] = box_positions[1]*width + position;
                    box_positions[2] = {1.f,1.f,0.f}; box_positions[2] = box_positions[2]*width + position;
                    box_positions[3] = {1.f,0.f,0.f}; box_positions[3] = box_positions[3]*width + position;

                    box_positions[4] = {0.f,0.f,1.f}; box_positions[4] = box_positions[4]*width + position;
                    box_positions[5] = {1.f,0.f,1.f}; box_positions[5] = box_positions[5]*width + position;
                    box_positions[6] = {1.f,1.f,1.f}; box_positions[6] = box_positions[6]*width + position;
                    box_positions[7] = {0.f,1.f,1.f}; box_positions[7] = box_positions[7]*width + position;
                    
                    std::vector<unsigned> triangles = {
                            0, 1, 2, // back
                            0, 2, 3,
                            5, 6 ,7, // front
                            5, 7, 4,
                            5, 3, 6, // right
                            3, 2, 6,
                            0, 4, 1, // left
                            4, 7, 1,
                            1, 7, 6, // top
                            1, 6, 2,
                            0, 5, 4, // bottom
                            0, 3, 5 };
                        
                    std::vector<V3f> normals;
                    std::vector<V3f> uvs;
                    
                    TriangleMesh * mesh = new TriangleMesh(box_positions,uvs,normals,triangles);
                    mesh->recomputeNormals();
                    RenderPrimitive::Ptr prim_ptr =
                        RenderPrimitive::Ptr( mesh );
                    m_primitives[primitive_tag] = prim_ptr;
                }
                else if (name == "trimesh")
                {
                    JSONGETSTR(it,filename, "none");
                    JSONGETF3(it,position,0,0.0f,0.f);
                    JSONGETF(it,scale,1.f);
                    
                    tinyobj::ObjReaderConfig config;
                    config.triangulate = true;
                    config.vertex_color = false;
                    
                    tinyobj::ObjReader reader;
                    std::string fn = filename;
                    reader.ParseFromFile(fn, config);
                    const std::vector<tinyobj::shape_t> & shapes = reader.GetShapes();
                    
                    const tinyobj::attrib_t attrs = reader.GetAttrib();
                    const size_t npoints = attrs.GetVertices().size() / 3;
                    std::vector<V3f> positions;
                    positions.resize(npoints);
                    auto pos_it = positions.begin();
                    for (size_t i = 0; i < npoints; ++i)
                    {
                        V3f p;
                        p.x = attrs.GetVertices()[i*3];
                        p.y = attrs.GetVertices()[i*3+1];
                        p.z = attrs.GetVertices()[i*3+2];
                        *pos_it++ = p*scale + position;
                    }
                    
                    for (auto & s : shapes)
                    {
                        LOG(INFO) << "shape name: " << s.name;
                        tinyobj::mesh_t m = s.mesh;
                        
                    }
                    std::vector<unsigned> triangles;
                    triangles.resize(shapes[0].mesh.indices.size());
                    auto t_it = triangles.begin();
                    for (auto & i: shapes[0].mesh.indices)
                    {
                        *t_it++ = static_cast<unsigned>(i.vertex_index);
                    }
                    
                    
                    std::vector<V3f> normals;
                    std::vector<V3f> uvs;
                    
                    TriangleMesh * mesh = new TriangleMesh(positions,uvs,normals,triangles);
                    mesh->recomputeNormals();
                    RenderPrimitive::Ptr prim_ptr =
                        RenderPrimitive::Ptr( mesh );
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

const std::map<std::string,RenderPrimitive::Ptr > & JsonParser::getPrimitives() const
{
    return m_primitives;
}

const std::map<std::string, std::string> & JsonParser::getMaterialTable() const
{
    return m_material_table;
}
