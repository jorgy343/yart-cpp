export module Geometry;

import "Common.h";

import GeometryDecl;
import IntersectableGeometry;
import Material;
import Math;
import Ray;

namespace Yart
{
    export class __declspec(dllexport) Geometry : public IntersectableGeometry
    {
    public:
        virtual constexpr const Material* GetMaterial() const = 0;

        virtual constexpr Vector3 CalculateNormal(const Ray& ray, const Vector3& hitPosition, float additionalData) const = 0;
    };

    export template<typename T>
    concept GeometryConcept = std::is_base_of<Geometry, T>::value;
}