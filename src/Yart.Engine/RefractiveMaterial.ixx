export module RefractiveMaterial;

import "Common.h";

import Geometry;
import Material;
import Math;
import Random;
import Ray;
import Scene;

namespace Yart
{
    export class RefractiveMaterial : public Material
    {
    protected:
        real RefractionIndex{};

    public:
        RefractiveMaterial(real refractionIndex)
            : RefractionIndex{refractionIndex}
        {

        }

        virtual Color3 CalculateRenderingEquation(
            const Scene& scene,
            const Random& random,
            int currentDepth,
            const Geometry* hitGeometry,
            const Vector3& hitPosition,
            const Vector3& hitNormal,
            const Vector3& incomingDirection,
            real mixAmount) const override
        {
            Vector3 refractionDirection = Vector3::Refract(incomingDirection, hitNormal, real{1.0}, RefractionIndex);
            if (refractionDirection.LengthSquared() < real{0.01})
            {
                return Color3{};
            }

            refractionDirection.Normalize();

            Ray refractionRay = Ray{hitPosition, refractionDirection};
            IntersectionResult exitIntersection = hitGeometry->IntersectExit(refractionRay);

            Vector3 exitPosition = refractionRay.Position + exitIntersection.HitDistance * refractionRay.Direction;

            // Reverse the refraction direction so that the CalculateNormal method will see the ray
            // as coming in towards the geometry rather than coming out of it.
            refractionRay = Ray{refractionRay.Position, -refractionRay.Direction};

            // Because we flipped the refraction direction, the normal should be pointing away
            // from the geometry.
            Vector3 exitNormal = hitGeometry->CalculateNormal(refractionRay, exitPosition, exitIntersection.AdditionalData);
            exitPosition += exitNormal * NormalBump;

            // Create the outgoing ray. Use the non reversed refraction direction and the reversed
            // exit normal.
            Vector3 outgoingDirection = Vector3::Refract(refractionDirection, -exitNormal, RefractionIndex, real{1.0});
            if (outgoingDirection.LengthSquared() < real{0.01})
            {
                return Color3{};
            }

            outgoingDirection.Normalize();

            Ray outgoingRay{exitPosition, outgoingDirection};
            Color3 outputColor = scene.CastRayColor(outgoingRay, currentDepth + 1, random);

            return outputColor;
        }
    };
}