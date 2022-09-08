export module RayTracer.GeometrySoa;

import RayTracer.Ray;
import RayTracer.Geometry;
import RayTracer.IntersectionResult;

namespace RayTracer
{
    export template<GeometryConcept TGeometry>
    class GeometrySoa
    {
    public:
        virtual char GetCount() const = 0;
        virtual void Add(const TGeometry* geometry) = 0;
        virtual void Finalize() = 0;

        virtual IntersectionResult<TGeometry> IntersectEntrance(const Ray& ray) const = 0;
        virtual IntersectionResult<TGeometry> IntersectExit(const Ray& ray) const = 0;
    };
}