#ifndef INFINITESPHERE_HPP_
#define INFINITESPHERE_HPP_

#include "Primitive.hpp"
#include "Mesh.hpp"

namespace Tungsten
{

class InfiniteSphere : public Primitive
{
    struct InfiniteSphereIntersection
    {
        Vec3f p;
        Vec3f w;
    };

    Mat4f _rotTransform;
    Mat4f _invRotTransform;
    bool _doSample;

    std::shared_ptr<TriangleMesh> _proxy;

public:
    InfiniteSphere()
    : _doSample(true)
    {
    }

    void fromJson(const rapidjson::Value &v, const Scene &scene) override
    {
        Primitive::fromJson(v, scene);
        JsonUtils::fromJson(v, "doSample", _doSample);
    }
    rapidjson::Value toJson(Allocator &allocator) const override
    {
        rapidjson::Value v = Primitive::toJson(allocator);
        v.AddMember("type", "infinite_sphere", allocator);
        v.AddMember("doSample", _doSample, allocator);
        return std::move(v);
    }

    virtual bool intersect(Ray &ray, IntersectionTemporary &data) const
    {
        InfiniteSphereIntersection *isect = data.as<InfiniteSphereIntersection>();
        isect->p = ray.pos();
        isect->w = ray.dir();
        data.primitive = this;
        return true;
    }

    virtual bool occluded(const Ray &/*ray*/) const
    {
        return true;
    }

    virtual bool hitBackside(const IntersectionTemporary &/*data*/) const
    {
        return false;
    }

    virtual void intersectionInfo(const IntersectionTemporary &data, IntersectionInfo &info) const
    {
        const InfiniteSphereIntersection *isect = data.as<InfiniteSphereIntersection>();
        Vec3f wLocal = _rotTransform*isect->w;
        info.Ng = info.Ns = -isect->w;
        info.p = isect->p;
        info.uv = Vec2f(std::atan2(wLocal.z(), wLocal.x())*INV_TWO_PI + 0.5f, std::acos(wLocal.y())*INV_PI);
        info.primitive = this;
    }

    virtual bool tangentSpace(const IntersectionTemporary &/*data*/, const IntersectionInfo &/*info*/, Vec3f &/*T*/, Vec3f &/*B*/) const
    {
        return false;
    }

    virtual bool isSamplable() const
    {
        return _doSample && !_emission->isConstant();
    }

    virtual void makeSamplable()
    {
    }

    virtual float inboundPdf(const IntersectionTemporary &/*data*/, const Vec3f &/*p*/, const Vec3f &/*d*/) const
    {
        if (_emission->isConstant()) {
            return INV_FOUR_PI;
        } else {
            /* TODO */
            return 0.0f;
        }
    }

    virtual bool sampleInboundDirection(LightSample &sample) const
    {
        if (_emission->isConstant()) {
            sample.d = Sample::uniformSphere(sample.sampler->next2D());
            sample.dist = 1e30f;
            sample.pdf = INV_FOUR_PI;
            return true;
        } else {
            /* TODO */
            return false;
        }
    }

    virtual bool sampleOutboundDirection(LightSample &/*sample*/) const
    {
        return false;
    }

    virtual bool invertParametrization(Vec2f /*uv*/, Vec3f &/*pos*/) const
    {
        return false;
    }


    virtual bool isDelta() const
    {
        return false;
    }

    virtual bool isInfinite() const
    {
        return true;
    }

    virtual Box3f bounds() const
    {
        return Box3f(Vec3f(-1e30f), Vec3f(1e30f));
    }

    void buildProxy()
    {
        _proxy = std::make_shared<TriangleMesh>(std::vector<Vertex>(), std::vector<TriangleI>(), _bsdf, "Sphere", false);
        _proxy->makeSphere(0.05f);
    }

    virtual const TriangleMesh &asTriangleMesh()
    {
        if (!_proxy)
            buildProxy();
        return *_proxy;
    }

    virtual void prepareForRender()
    {
        _rotTransform = _transform.extractRotation();
        _invRotTransform = _rotTransform.transpose();
    }

    virtual void cleanupAfterRender()
    {
    }

    virtual float area() const
    {
        return 1e30f;
    }

    virtual Primitive *clone()
    {
        return new InfiniteSphere(*this);
    }
};

}


#endif /* INFINITESPHERE_HPP_ */
