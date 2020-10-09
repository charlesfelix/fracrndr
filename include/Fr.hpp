//
//  Fr.hpp
//  FracRndr
//
//  Created by Charles-Felix on 4/10/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Fr_hpp__
#define Fr_hpp__

#include <ctime>


#include <limits>
#include <half.h>
#include <ImathHalfLimits.h>

#include <ImathBox.h>
#include <ImathBoxAlgo.h>
#include <ImathColor.h>
#include <ImathMatrix.h>
#include <ImathVec.h>

#include <ImathRoots.h>
#include <ImathMatrixAlgo.h>
#include <ImathRandom.h>
#include <ImathPlane.h>
#include <ImathQuat.h>

#include <cassert>

#define DEF_SHARED_PTR_TYPES(Class) typedef std::shared_ptr<Class> Ptr; typedef std::shared_ptr<const Class> ConstPtr

namespace Fr {
    
    typedef double Real;
    typedef ::half               half;
    typedef Imath::V2i           V2i;
    typedef Imath::V2f           V2f;
    typedef Imath::V2d           V2d;
    typedef Imath::Color3<float> C3f;
    typedef Imath::Color3<half>  C3h;
    typedef Imath::Color4<float> C4f;
    typedef Imath::V3i           V3i;
    typedef Imath::Vec3<half>    V3h;
    typedef Imath::V3d           V3f;
    typedef Imath::V3d           V3d;
    typedef Imath::Box2f         Box2f;
    typedef Imath::Box2i         Box2i;
    typedef Imath::Box3i         Box3i;
    typedef Imath::Box3d         Box3f;
    typedef Imath::Box3d         Box3d;
    typedef Imath::M33f          M33f;
    typedef Imath::M44f          M44f;
    typedef Imath::M44d          M44d;
    typedef Imath::Plane3d       Plane3d;
    typedef Imath::Line3d        Line3d;
    typedef Imath::Quatd         Quatd;
    typedef Imath::Quatd         Quat;
    

    #define FR_CLIP         Imath::clip
    #define FR_LERP         Imath::lerp
    #define FR_LERPFACTOR   Imath::lerpfactor
    #define FR_EXTRACT_SHRT Imath::extractSHRT

    #define FR_RAND48       Imath::Rand48
    #define FR_RAND32       Imath::Rand32
    #define FR_SOLIDSPHERERAND Imath::solidSphereRand
    #define FR_HALF_LIMITS Imath::limits<Fr::half>
    
    #define FR_LUMINANCE(c) (0.2126f*c[0] + 0.7152f*c[1] + 0.0722f*c[2])


    // Simple Timer
    class Timer
    {
    public:
        Timer() { clock_gettime(CLOCK_REALTIME, &beg_); }
        
        double elapsed() {
            clock_gettime(CLOCK_REALTIME, &end_);
            return end_.tv_sec - beg_.tv_sec +
            (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
        }
        
        void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }
        
    private:
        timespec beg_, end_;
    };
    
    // radians to degrees conversion
    class Radians
    {
    public:
        static inline Real toDegrees(Real radians) { return radians/M_PI*180.f; }
        static inline Real fromDegrees(Real degrees) { return degrees/180.f*M_PI; }
    };
    
    // unclamped fit
    template <typename t>
    inline t ufit(const t& v,const t& a,const t& b,const t& a2, const t& b2)
    {
        return (v-a)/(b-a)*(b2-a2)+a2;
    }

    // clamped fit
    template <typename t>
    inline t fit(const t& v,const t& a,const t& b,const t& a2, const t& b2)
    {
        return std::max(std::min(ufit(v,a,b,a2,b2),b2),a2);
    }
    
    // linear interpolation
    template <typename t>
    inline t lerp(const t& a,const t& b,t k)
    {
        return (1.f-k)*a + (k)*b;
    }
    
    // vector reflection
    template <typename t>
    inline Imath::Vec3<t> reflect(const Imath::Vec3<t> & v, const Imath::Vec3<t> & n)
    {
        return v - t(2.0)*v.dot(n)*n;
    }
    
    class Basis {
    public:
        static inline void build(const V3f & N, V3f &tangent, V3f &bitangent)
        {
            // from https://graphics.pixar.com/library/OrthonormalB/paper.pdf
            Real sign = copysignf(1.0f, N.z);
            const Real a = -1.0f / (sign + N.z);
            const Real b = N.x * N.y * a;
            tangent = V3f(1.0f + sign * N.x * N.x * a, sign * b, -sign * N.x);
            bitangent = V3f(b, sign + N.y * N.y * a, -N.y);
            return;
        }
        static inline M33f build(const V3f & N)
        {
            M33f rotation;
            V3f tangent;
            V3f bitangent;
            
            Basis::build(N,tangent,bitangent);
            
            rotation[0][0] = N.x;
            rotation[0][1] = N.y;
            rotation[0][2] = N.z;
            rotation[1][0] = tangent.x;
            rotation[1][1] = tangent.y;
            rotation[1][2] = tangent.z;
            rotation[2][0] = bitangent.x;
            rotation[2][1] = bitangent.y;
            rotation[2][2] = bitangent.z;
            
            return rotation;
        }
    };
    
    
    
    // Ray Box intersection
    //https://raytracing.github.io/books/RayTracingTheNextWeek.html
    inline bool intersectAABB(const Box3f & box, const V3f & origin, const V3f & direction, Real &tmin, Real &tmax )
    {
        for (int a = 0; a < 3; a++) {
            const Real invd = 1/direction[a];
            auto t0 = fmin((box.min[a] - origin[a]) * invd,
                           (box.max[a] - origin[a]) * invd );
            auto t1 = fmax((box.min[a] - origin[a]) * invd,
                           (box.max[a] - origin[a]) * invd );
            tmin = fmax(t0, tmin);
            tmax = fmin(t1, tmax);
            if (tmax < tmin)
                return false;
        }
        return true;
    }
    inline double randomDouble(double min, double max)
    {
        return drand48()*(max-min)+min;
    }

    inline int randomInt(int min, int max) {
        // Returns a random integer in [min,max].
        return static_cast<int>(randomDouble(static_cast<double>(min), static_cast<double>(max+1)));
    }

}; // namespace Fr

#endif /* Fr_hpp */
