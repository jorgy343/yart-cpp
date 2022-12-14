module;

#include "Vcl.h"

export module AtmosphereMissShader;

import "Common.h";

import Math;
import MissShader;
import Random;
import Ray;
import Sphere;

using namespace vcl;

namespace Yart
{
    export class AtmosphereMissShader : public MissShader
    {
    protected:
        Sphere Atmosphere{};

        Vector3 Offset{};

        Vector3 SunDirectionReversed{};
        Color3 SunIntensity{};

        real Lambda{};

        real PlanetRadius{};
        real AtmosphereHeight{};

        real RayleighScaleHeight{};
        real MieScaleHeight{};

        real IndexOfRefractionAtSeaLevel{};
        real NumberDensityOfAirAtSeaLevel{};
        real NumberDensityOfAerosolsAtSeaLevel{};

        real MieU{};
        real MieX{};
        real MieG{};

        Color3 RayleighBeta{};
        Color3 MieBeta{};

        static constexpr Color3 Wavelength{real{6.80e-7}, real{5.50e-7}, real{4.40e-7}};
        static constexpr Color3 WavelengthPowFour = Wavelength * Wavelength * Wavelength * Wavelength;

        //static constexpr Color3 Br{real{5.8e-6}, real{13.5e-6}, real{33.1e-6}};
        //static constexpr Color3 Bm{real{21e-6}};

    public:
        AtmosphereMissShader(
            const Vector3& offset,
            const Vector3& sunDirection,
            const Color3& sunIntensity,
            real lambda,
            real planetRadius,
            real atmosphereHeight,
            real rayleighScaleHeight,
            real mieScaleHeight,
            real indexOfRefractionAtSeaLevel,
            real numberDensityOfAirAtSeaLevel,
            real numberDensityOfAerosolsAtSeaLevel,
            real mieU)
            :
            Atmosphere{-offset, planetRadius + atmosphereHeight, nullptr},
            Offset{offset},
            SunDirectionReversed{-sunDirection},
            SunIntensity{sunIntensity},
            Lambda{lambda},
            PlanetRadius{planetRadius},
            AtmosphereHeight{atmosphereHeight},
            RayleighScaleHeight{rayleighScaleHeight},
            MieScaleHeight{mieScaleHeight},
            IndexOfRefractionAtSeaLevel{indexOfRefractionAtSeaLevel},
            NumberDensityOfAirAtSeaLevel{numberDensityOfAirAtSeaLevel},
            NumberDensityOfAerosolsAtSeaLevel{numberDensityOfAerosolsAtSeaLevel},
            MieU{mieU},
            MieX{(real{5} / real{9}) * MieU + (real{125} / real{729}) * MieU * MieU * MieU + Math::sqrt(real{64} / real{27} - (real{325} / real{243}) * MieU * MieU + (real{1250} / real{2187}) * MieU * MieU * MieU * MieU)},
            MieG{(real{5} / real{9}) * MieU - ((real{4} / real{3}) - (real{25} / real{81}) * MieU * MieU) * Math::pow(MieX, real{-1} / real{3}) + Math::pow(MieX, real{1} / real{3})},
            RayleighBeta{(real{8} *Pi * Pi * Pi * (IndexOfRefractionAtSeaLevel * IndexOfRefractionAtSeaLevel - real{1}) * (IndexOfRefractionAtSeaLevel * IndexOfRefractionAtSeaLevel - real{1})) / (real{3} *NumberDensityOfAirAtSeaLevel) * Color3::Reciprical(WavelengthPowFour)},
            MieBeta{(real{8} *Pi * Pi * Pi * (IndexOfRefractionAtSeaLevel * IndexOfRefractionAtSeaLevel - real{1}) * (IndexOfRefractionAtSeaLevel * IndexOfRefractionAtSeaLevel - real{1})) / (real{3} *NumberDensityOfAerosolsAtSeaLevel)}
        {

        }

        virtual Color3 CalculateColor(const Ray& ray, const Random& random) const override
        {
            real cosTheta = ray.Direction * SunDirectionReversed;

            // Calculate view ray.
            Vector3 viewRayStart = ray.Position + Offset;
            real viewRayDistance = Atmosphere.IntersectExit(ray).HitDistance;

            auto [viewRayRandomNumber, viewRayInversePdf] = random.GetExponentialRandomAndInversePdf(viewRayDistance, Lambda);

            real viewRaySampleDistance = viewRayRandomNumber * viewRayDistance;
            Vector3 viewRaySamplePoint = viewRayStart + viewRaySampleDistance * ray.Direction;
            real viewRaySamplePointHeight = viewRaySamplePoint.Length() - PlanetRadius;

            real viewRaySamplePointDensityR = Density(viewRaySamplePointHeight, RayleighScaleHeight);
            real viewRaySamplePointDensityM = Density(viewRaySamplePointHeight, MieScaleHeight);

            Color3 viewRayOpticalDepth = OpticalDepth(viewRayStart, viewRaySampleDistance, random);
            //Color3 viewRayOpticalDepth = OpticalDepthTrapazoidal(viewRayStart, ray.Direction, viewRaySampleDistance, random);

            // Calculate for the sun ray.
            Vector3 sunRayStart = viewRaySamplePoint;
            Ray sunRay{sunRayStart - Offset, SunDirectionReversed};
            real sunRayDistance = Atmosphere.IntersectExit(sunRay).HitDistance;

            Color3 sunRayOpticalDepth = OpticalDepth(sunRayStart, sunRayDistance, random);
            //Color3 sunRayOpticalDepth = OpticalDepthTrapazoidal(sunRayStart, sunRay.Direction, sunRayDistance, random);

            // Calculate final result.
            Color3 coefficient = (RayleighBeta * viewRaySamplePointDensityR * RayleighPhase(cosTheta)) + (MieBeta * viewRaySamplePointDensityM * MiePhase(cosTheta));
            Color3 outScattering = (-sunRayOpticalDepth - viewRayOpticalDepth).Exp();

            Color3 result = SunIntensity * coefficient * outScattering * viewRayInversePdf;
            return result;
        }

    protected:
        real Density(real altitude, real scaleHeight) const
        {
            return Math::exp(-altitude / scaleHeight);
        }

        real RayleighPhase(real cosTheta) const
        {
            real coefficient = real{3} / (real{16} *Pi);
            return coefficient * (real{1} + cosTheta * cosTheta);
        }

        real MiePhase(real cosTheta) const
        {
            real coefficient = real{3} / EightPi;

            real numerator = (real{1} - MieG * MieG) * (real{1} + cosTheta * cosTheta);
            real denominator = (real{2} + MieG * MieG) * Math::pow(real{1} + MieG * MieG - real{2} *MieG * cosTheta, real{1.5});

            return coefficient * (numerator / denominator);
        }

        Color3 OpticalDepth(const Vector3& startingPoint, real distance, const Random& random) const
        {
            auto [randomNumber, inversePdf] = random.GetExponentialRandomAndInversePdf(distance, Lambda);

            Vector3 samplePoint = startingPoint + randomNumber * distance * SunDirectionReversed;
            real samplePointHeight = samplePoint.Length() - PlanetRadius;

            real densityR = Density(samplePointHeight, RayleighScaleHeight);
            real densityM = Density(samplePointHeight, MieScaleHeight);

            return ((densityR * RayleighBeta) + (densityM * MieBeta)) * inversePdf;
        }

        // TODO: Doesn't seem to work. Probably just vectorize the main CalculateColor function.
        Color3 OpticalDepthVec(const Vector3& startingPoint, real distance, const Random& random) const
        {
            real_vec normalizedRandom = random.GetNormalizedVec();
            real_vec randomNumber = random.ExponentialRandomVec(normalizedRandom, Lambda);
            real_vec inversePdf = real_vec{distance} *random.ExponentialRandomPdfVec(normalizedRandom, Lambda);

            VectorVec3<real_vec> samplePoint = VectorVec3<real_vec>{startingPoint} + randomNumber * real_vec{distance} *VectorVec3<real_vec>{SunDirectionReversed};
            real_vec samplePointHeight = samplePoint.Length() - real_vec{PlanetRadius};

            real_vec densityR = exp(-samplePointHeight / real_vec{RayleighScaleHeight});
            real_vec densityM = exp(-samplePointHeight / real_vec{MieScaleHeight});

            VectorVec3<real_vec> result = ((densityR * VectorVec3<real_vec>{RayleighBeta}) + (densityM * VectorVec3<real_vec>{MieBeta}))* inversePdf;

            return
            {
                horizontal_add(result.X) / RealVecElements,
                horizontal_add(result.Y) / RealVecElements,
                horizontal_add(result.Z) / RealVecElements,
            };
        }

        Color3 OpticalDepthTrapazoidal(const Vector3& startingPoint, const Vector3& direction, real distance, const Random& random) const
        {
            constexpr int sampleCount = 8;

            Vector3 segmentIncrement = direction * (distance / static_cast<real>(sampleCount));

            real resultR{0};
            real resultM{0};

            real firstSampleHeight = startingPoint.Length() - PlanetRadius;
            real lastSampleHeight = (startingPoint + direction * distance).Length() - PlanetRadius;

            resultR += Density(firstSampleHeight, RayleighScaleHeight);
            resultM += Density(firstSampleHeight, MieScaleHeight);

            resultR += Density(lastSampleHeight, RayleighScaleHeight);
            resultM += Density(lastSampleHeight, MieScaleHeight);

            for (int i = 1; i < sampleCount - 1; i++)
            {
                real samplePointHeight = (startingPoint + static_cast<real>(i) * segmentIncrement).Length() - PlanetRadius;

                resultR += Density(samplePointHeight, RayleighScaleHeight);
                resultM += Density(samplePointHeight, MieScaleHeight);
            }

            real integrationThing = distance / static_cast<real>(sampleCount) / real{2};

            resultR *= integrationThing;
            resultM *= integrationThing;

            return resultR * RayleighBeta + resultM * MieBeta;
        }
    };
}