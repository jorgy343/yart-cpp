export module Triangle;

import <limits>;

import "Common.h";

import Geometry;
import IntersectionResult;
import IntersectionResultType;
import Material;
import Math;
import Ray;
import Vector3;

namespace Yart
{
    export class alignas(16) Triangle : public Geometry
    {
    public:
        Vector3 Vertex0{};
        Vector3 Vertex1{};
        Vector3 Vertex2{};
        Vector3 Normal{};
        const Material* AppliedMaterial{nullptr};

        inline constexpr Triangle() = default;

        inline constexpr Triangle(
            const Vector3& vertex0,
            const Vector3& vertex1,
            const Vector3& vertex2,
            const Material* appliedMaterial)
            :
            Vertex0{vertex0},
            Vertex1{vertex1},
            Vertex2{vertex2},
            Normal{Vector3{vertex0.Y * vertex1.Z - vertex0.Z * vertex1.Y, vertex0.Z * vertex1.X - vertex0.X * vertex1.Z, vertex0.X * vertex1.Y - vertex0.Y * vertex1.X}.Normalize()},
            AppliedMaterial{appliedMaterial}
        {

        }

        inline constexpr Triangle(
            const Vector3& vertex0,
            const Vector3& vertex1,
            const Vector3& vertex2,
            const Vector3& normal,
            const Material* appliedMaterial)
            :
            Vertex0{vertex0},
            Vertex1{vertex1},
            Vertex2{vertex2},
            Normal{normal},
            AppliedMaterial{appliedMaterial}
        {

        }

        inline constexpr const Material* GetMaterial() const override
        {
            return AppliedMaterial;
        }

        inline constexpr Vector3 CalculateNormal(const Ray& ray, const Vector3& hitPosition) const override
        {
            return (ray.Direction * Normal) < 0.0f ? Normal : -Normal;
        }

        IntersectionResult IntersectEntrance(const Ray& ray) const override
        {
            return {this, Intersect(ray)};
        }

        IntersectionResult IntersectExit(const Ray& ray) const override
        {
            return {this, Intersect(ray)};
        }

        force_inline constexpr float Intersect(const Ray& ray) const
        {
            // From: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
            Vector3 edge1 = Vertex1 - Vertex0;
            Vector3 edge2 = Vertex2 - Vertex0;

            Vector3 h = ray.Direction % edge2;
            float a = edge1 * h;

            // Normally you would check for a parallel ray here but we'll skip that check.

            float f = Math::rcp(a);
            Vector3 s = ray.Position - Vertex0;
            float u = f * (s * h);

            if (u < 0.0f || u > 1.0f)
            {
                return std::numeric_limits<float>::infinity();
            }

            Vector3 q = s % edge1;
            float v = f * (ray.Direction * q);

            if (v < 0.0f || u + v > 1.0f)
            {
                return std::numeric_limits<float>::infinity();
            }

            float entranceDistance = f * (edge2 * q);
            return entranceDistance >= 0.0f ? entranceDistance : std::numeric_limits<float>::infinity();
        }
    };
}