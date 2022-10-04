export module PhongMaterial;

import "Common.h";

import Geometry;
import Material;
import Math;
import Random;
import Scene;
import Vector3;

namespace Yart
{
    export class PhongMaterial : public Material
    {
    protected:
        Vector3 AmbientColor{};
        Vector3 DiffuseColor{};
        Vector3 SpecularColor{};

        float Shininess{};

    public:
        constexpr PhongMaterial(
            const Vector3& ambientColor,
            const Vector3& diffuseColor,
            const Vector3& specularColor,
            float shininess)
            :
            AmbientColor{ambientColor},
            DiffuseColor{diffuseColor},
            SpecularColor{specularColor},
            Shininess{shininess}
        {

        }

        constexpr Vector3 CalculateRenderingEquation(
            const Scene& scene,
            const Random& random,
            int currentDepth,
            const Geometry* hitGeometry,
            const Vector3& hitPosition,
            const Vector3& hitNormal,
            const Vector3& incomingDirection) const override
        {
            Vector3 ambientComponent = AmbientColor;
            Vector3 diffuseComponent{};
            Vector3 specularComponent{};

            for (const auto& light : scene.Lights)
            {
                Vector3 directionToLight = light->GetDirectionTowardsLight(hitPosition, hitNormal);

                if (light->IsInShadow(scene, hitPosition, hitNormal, directionToLight))
                {
                    continue;
                }

                float lightDotNormal = directionToLight * hitNormal;
                if (lightDotNormal >= 0.0f)
                {
                    diffuseComponent += lightDotNormal * DiffuseColor.ComponentwiseMultiply(light->Color);

                    Vector3 reflectionDirection = directionToLight.Reflect(hitNormal);
                    float reflectionDotView = reflectionDirection * incomingDirection;

                    if (reflectionDotView >= 0.0f)
                    {
                        specularComponent += Math::pow(reflectionDotView, Shininess) * SpecularColor.ComponentwiseMultiply(light->Color);
                    }
                }
            }

            for (const auto& areaLight : scene.AreaLights)
            {
                Vector3 pointOnLight = areaLight->GetPointOnLight(random, hitPosition, hitNormal);
                Vector3 directionToLight = (pointOnLight - hitPosition).Normalize();

                if (areaLight->IsInShadow(scene, hitPosition, hitNormal, pointOnLight))
                {
                    continue;
                }

                float lightDotNormal = directionToLight * hitNormal;
                float areaLightInversePdf = areaLight->CalculateInversePdf(random, hitPosition, hitNormal, incomingDirection, directionToLight);

                if (lightDotNormal >= 0.0f)
                {
                    diffuseComponent += lightDotNormal * areaLightInversePdf * DiffuseColor; // TODO: Multiply by the light's color. Not sure how to obtain the light's color right now.

                    Vector3 reflectionDirection = directionToLight.Reflect(hitNormal);
                    float reflectionDotView = reflectionDirection * incomingDirection;

                    if (reflectionDotView >= 0.0f)
                    {
                        specularComponent += Math::pow(reflectionDotView, Shininess) * areaLightInversePdf * SpecularColor; // TODO: Multiply by the light's color. Not sure how to obtain the light's color right now.
                    }
                }
            }

            return ambientComponent + diffuseComponent + specularComponent;
        }
    };
}