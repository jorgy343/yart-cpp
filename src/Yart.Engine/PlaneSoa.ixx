module;

#include "Vcl.h"

export module PlaneSoa;

import <cassert>;
import <initializer_list>;
import <limits>;

import "Common.h";

import Alignment;
import GeometrySoa;
import IntersectionResult;
import IntersectionResultType;
import Math;
import Plane;
import Ray;

using namespace vcl;

namespace Yart
{
    export template<SoaSize Size>
        class __declspec(dllexport) alignas(64) PlaneSoa : public GeometrySoa<Plane>
    {
    public:
        static constexpr size_t Elements = std::same_as<real, float> ? (Size == SoaSize::_256 ? 8 : 4) : (Size == SoaSize::_256 ? 4 : 2);

    private:
        using VclVec = std::conditional_t<std::same_as<real, float>, std::conditional_t<Size == SoaSize::_256, Vec8f, Vec4f>, std::conditional_t<Size == SoaSize::_256, Vec4d, Vec2d>>;

        alignas(Elements * sizeof(real)) real _normalX[Elements];
        alignas(Elements * sizeof(real)) real _normalY[Elements];
        alignas(Elements * sizeof(real)) real _normalZ[Elements];
        alignas(Elements * sizeof(real)) real _distance[Elements];

        const Plane* _geometries[Elements];

    public:
        PlaneSoa()
        {
            for (int i = 0; i < Elements; i++)
            {
                _normalX[i] = std::numeric_limits<real>::infinity();
                _normalY[i] = std::numeric_limits<real>::infinity();
                _normalZ[i] = std::numeric_limits<real>::infinity();
                _distance[i] = std::numeric_limits<real>::infinity();

                _geometries[i] = nullptr;
            }
        }

        explicit PlaneSoa(std::initializer_list<const Plane*> list)
            : PlaneSoa{}
        {
            size_t index = 0;

            for (auto geometry : list)
            {
                if (index >= Elements)
                {
                    break;
                }

                Insert(index++, geometry);
            }
        }

        virtual void Insert(size_t index, const Plane* geometry) override
        {
            assert(index >= 0 && index < Elements);

            _normalX[index] = geometry->Normal.X;
            _normalY[index] = geometry->Normal.Y;
            _normalZ[index] = geometry->Normal.Z;
            _distance[index] = geometry->Distance;

            _geometries[index] = geometry;
        }

        virtual IntersectionResult IntersectEntrance(const Ray& ray) const override
        {
            return Intersect(ray);
        }

        virtual IntersectionResult IntersectExit(const Ray& ray) const override
        {
            return Intersect(ray);
        }

    private:
        force_inline IntersectionResult Intersect(const Ray& ray) const
        {
            VectorVec3<VclVec> rayDirection{ray.Direction};
            VectorVec3<VclVec> rayPosition{ray.Position};
            VectorVec3<VclVec> normal{_normalX, _normalY, _normalZ};

            VclVec normalDotDirection = VectorVec3<VclVec>::Dot(normal, rayDirection);
            VclVec normalDotRayPosition = VectorVec3<VclVec>::Dot(normal, rayPosition);

            VclVec distance = VclVec{}.load_a(_distance);
            VclVec entranceDistance = -(distance + normalDotRayPosition) * approx_recipr(normalDotDirection);

            // Make sure infinite8f() is second so nans are replaced with inf.
            VclVec clampedEntranceDistance = select(entranceDistance >= VclVec{real{0.0}}, entranceDistance, VclVec{std::numeric_limits<real>::infinity()});

            real minimumEntranceDistance = horizontal_min1(clampedEntranceDistance);
            int minimumIndex = horizontal_find_first(VclVec(minimumEntranceDistance) == clampedEntranceDistance);

            return {
                _geometries[minimumIndex == -1 ? 0 : minimumIndex],
                minimumEntranceDistance,
            };
        }
    };
}