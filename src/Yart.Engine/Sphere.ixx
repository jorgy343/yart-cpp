export module Sphere;

import <limits>;

import BoundingBox;
import Geometry;
import IntersectionResult;
import IntersectionResultType;
import Material;
import Math;
import Ray;

namespace Yart
{
    export class __declspec(dllexport) alignas(16) Sphere : public Geometry
    {
    public:
        Vector3 Position{};
        float Radius{0.0f};
        const Material* AppliedMaterial{nullptr};

        inline constexpr Sphere() = default;

        inline constexpr Sphere(const Vector3& position, float radius, const Material* appliedMaterial)
            : Position{position}, Radius{radius}, AppliedMaterial{appliedMaterial}
        {

        }

        constexpr BoundingBox CalculateBoundingBox() const override
        {
            return BoundingBox{
                Position - Radius,
                Position + Radius,
            };
        }

        inline constexpr const Material* GetMaterial() const override
        {
            return AppliedMaterial;
        }

        inline constexpr Vector3 CalculateNormal(const Ray& ray, const Vector3& hitPosition, float additionalData) const override
        {
            return (hitPosition - Position).Normalize();
        }

        IntersectionResult IntersectEntrance(const Ray& ray) const override
        {
            return {this, Intersect<IntersectionResultType::Entrance>(ray)};
        }

        IntersectionResult IntersectExit(const Ray& ray) const override
        {
            return {this, Intersect<IntersectionResultType::Exit>(ray)};
        }

        template <IntersectionResultType TIntersectionResultType>
        inline constexpr float Intersect(const Ray& ray) const
        {
            Vector3 v = ray.Position - Position;

            float a = ray.Direction * ray.Direction;
            float b = v * ray.Direction;
            float c = (v * v) - (Radius * Radius);

            float discriminant = (b * b) - (a * c);
            if (discriminant < 0.0f)
            {
                return std::numeric_limits<float>::infinity();
            }

            float discriminantSqrt = Math::sqrt(discriminant);

            float reciprocalA = Math::rcp(a);
            float negativeB = -b;

            float exitDistance = (negativeB + discriminantSqrt) * reciprocalA;

            float result;
            if constexpr (TIntersectionResultType == IntersectionResultType::Entrance)
            {
                float entranceDistance = (negativeB - discriminantSqrt) * reciprocalA;
                result = entranceDistance;
            }
            else
            {
                result = exitDistance;
            }

            return exitDistance >= 0.0f ? result : std::numeric_limits<float>::infinity();
        }
    };
}