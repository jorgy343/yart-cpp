module;

#include "Vcl.h"

export module Scene;

import <utility>;

import "Common.h";

import Alignment;
import AreaLight;
import Geometry;
import IntersectableGeometry;
import Light;
import Material;
import Math;
import MissShader;
import Random;
import Ray;

namespace Yart
{
    export class Scene
    {
    private:
        const MissShader* _missShader{};

    public:
        const IntersectableGeometry* RootGeometry{};
        std::vector<const Light*> Lights{};
        std::vector<const AreaLight*> AreaLights{};

        inline constexpr Scene(const IntersectableGeometry* rootGeometry, const MissShader* missShader)
            : RootGeometry{rootGeometry}, _missShader{missShader}
        {

        }

        inline constexpr void AddLight(const Light* light)
        {
            Lights.push_back(light);
        }

        inline void AddAreaLight(const AreaLight* areaLight)
        {
            AreaLights.push_back(areaLight);
        }

        inline Color3 CastRayColor(const Ray& ray, const Random& random) const
        {
            return CastRayColor(ray, 1, random);
        }

        Color3 CastRayColor(const Ray& ray, int depth, const Random& random) const
        {
            if (depth > 7)
            {
                return Color3{};
            }

            IntersectionResult intersection = RootGeometry->IntersectEntrance(ray);
            Color3 outputColor{};

            if (intersection.HitGeometry)
            {
                // TODO: The material override feels a bit hacky. It's used to support signed distance field ray marching results.
                const Material* material = intersection.MaterialOverride != nullptr ? intersection.MaterialOverride : intersection.HitGeometry->GetMaterial();

                Vector3 hitPosition = ray.Position + intersection.HitDistance * ray.Direction;
                Vector3 hitNormal = intersection.HitGeometry->CalculateNormal(ray, hitPosition, intersection.AdditionalData);
                hitPosition += hitNormal * NormalBump;

                outputColor = material->CalculateRenderingEquation(
                    *this,
                    random,
                    depth,
                    intersection.HitGeometry,
                    hitPosition,
                    hitNormal,
                    ray.Direction,
                    intersection.AdditionalData);
            }
            else
            {
                outputColor = _missShader->CalculateColor(ray, random);
            }

            return outputColor;
        }

        real CastRayDistance(const Ray& ray) const
        {
            IntersectionResult intersection = RootGeometry->IntersectEntrance(ray);
            return Math::max(real{0.0}, intersection.HitDistance);
        }
    };
}