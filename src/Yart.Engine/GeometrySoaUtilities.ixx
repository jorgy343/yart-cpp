module;

#include "range/v3/view/chunk.hpp"

#include "Vcl.h"

export module GeometrySoaUtilities;

import <memory>;
import <vector>;

import AxisAlignedBox;
import AxisAlignedBoxSoa;
import GeometrySoa;
import IntersectableGeometry;
import Parallelogram;
import ParallelogramSoa;
import Plane;
import PlaneSoa;
import Sphere;
import SphereSoa;
import Triangle;
import TriangleSoa;

namespace Yart
{
    export void CreateGeometrySoaStructures(
        const std::vector<const IntersectableGeometry*>& inputGeometries,
        std::vector<const IntersectableGeometry*>& outputGeometries,
        std::vector<std::shared_ptr<const IntersectableGeometry>>& geometryPointers)
    {
        std::vector<const AxisAlignedBox*> axisAlignedBoxes{};
        std::vector<const Parallelogram*> parallelograms{};
        std::vector<const Plane*> planes{};
        std::vector<const Sphere*> spheres{};
        std::vector<const Triangle*> triangles{};

        for (const auto* inputGeometry : inputGeometries)
        {
            if (auto axisAlignedBox = dynamic_cast<const AxisAlignedBox*>(inputGeometry))
            {
                axisAlignedBoxes.push_back(axisAlignedBox);
            }
            else if (auto parallelogram = dynamic_cast<const Parallelogram*>(inputGeometry))
            {
                parallelograms.push_back(parallelogram);
            }
            else if (auto plane = dynamic_cast<const Plane*>(inputGeometry))
            {
                planes.push_back(plane);
            }
            else if (auto sphere = dynamic_cast<const Sphere*>(inputGeometry))
            {
                spheres.push_back(sphere);
            }
            else if (auto triangle = dynamic_cast<const Triangle*>(inputGeometry))
            {
                triangles.push_back(triangle);
            }
        }

        if (axisAlignedBoxes.size() > 0)
        {
            CreateGeometrySoaStructure<AxisAlignedBox, AxisAlignedBoxSoa>(axisAlignedBoxes, outputGeometries, geometryPointers);
        }

        if (parallelograms.size() > 0)
        {
            CreateGeometrySoaStructure<Parallelogram, ParallelogramSoa>(parallelograms, outputGeometries, geometryPointers);
        }

        if (planes.size() > 0)
        {
            CreateGeometrySoaStructure<Plane, PlaneSoa>(planes, outputGeometries, geometryPointers);
        }

        if (spheres.size() > 0)
        {
            CreateGeometrySoaStructure<Sphere, SphereSoa>(spheres, outputGeometries, geometryPointers);
        }

        if (triangles.size() > 0)
        {
            if (triangles.size() <= 4)
            {
                CreateGeometrySoaStructure<Triangle, TriangleSoa<4>>(triangles, outputGeometries, geometryPointers, 4);
            }
            else
            {
                CreateGeometrySoaStructure<Triangle, TriangleSoa<8>>(triangles, outputGeometries, geometryPointers, 8);
            }
        }
    }
}