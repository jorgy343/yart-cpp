module;

#include "yaml-cpp/yaml.h"

export module YamlNodes:Scene;

import <memory>;
import <unordered_map>;

import :Materials;
import :Vectors;
import AreaLight;
import IntersectableGeometry;
import Material;
import Parallelogram;
import Plane;
import Sphere;
import Vector3;

using namespace YAML;

namespace Yart::Yaml
{
	export class SceneConfig
	{
	public:
		std::vector<std::shared_ptr<const AreaLight>> AreaLights{};

        std::vector<std::shared_ptr<const Sphere>> Spheres{};
        std::vector<std::shared_ptr<const Plane>> Planes{};
        std::vector<std::shared_ptr<const Parallelogram>> Parallelograms{};
	};

	std::shared_ptr<const Sphere> ParseSphereNode(const Node& node, const MaterialMap& materialMap)
	{
		auto materialName = node["material"].as<std::string>();
		auto material = materialMap.find(materialName)->second;

		auto position = node["position"].as<Vector3>();
		auto radius = node["radius"].as<float>();

		return std::make_shared<const Sphere>(position, radius, material);
	}

	std::shared_ptr<const Plane> ParsePlaneNode(const Node& node, const MaterialMap& materialMap)
	{
		auto materialName = node["material"].as<std::string>();
		auto material = materialMap.find(materialName)->second;

		auto normal = node["normal"].as<Vector3>();
		auto point = node["point"].as<Vector3>();

		return std::make_shared<const Plane>(normal, point, material);
	}

	std::shared_ptr<const Parallelogram> ParseParallelogramNode(const Node& node, const MaterialMap& materialMap)
	{
		auto materialName = node["material"].as<std::string>();
		auto material = materialMap.find(materialName)->second;

		auto position = node["position"].as<Vector3>();
		auto edge1 = node["edge1"].as<Vector3>();
		auto edge2 = node["edge2"].as<Vector3>();

		return std::make_shared<const Parallelogram>(position, edge1, edge2, material);
	}

	std::shared_ptr<const AreaLight> ParseAreaLightNode(const Node& node, SceneConfig& sceneConfig, const MaterialMap& materialMap)
	{
		auto parallelogramNode = node["parallelogram"];

		if (parallelogramNode)
		{
            auto geometry = ParseParallelogramNode(parallelogramNode, materialMap);
            sceneConfig.Parallelograms.push_back(geometry);

			return geometry;
		}
        else
        {
            return std::shared_ptr<const AreaLight>{};
		}
	}

	std::shared_ptr<const IntersectableGeometry> ParseGeometryNode(const Node& node, SceneConfig& sceneConfig, const MaterialMap& materialMap)
	{
		auto sphereNode = node["sphere"];
		auto planeNode = node["plane"];
		auto parallelogramNode = node["parallelogram"];

		if (sphereNode)
		{
            auto geometry = ParseSphereNode(sphereNode, materialMap);
            sceneConfig.Spheres.push_back(geometry);

            return geometry;
		}
		else if (planeNode)
		{
            auto geometry = ParsePlaneNode(planeNode, materialMap);
            sceneConfig.Planes.push_back(geometry);

            return geometry;
		}
		else if (parallelogramNode)
		{
            auto geometry = ParseParallelogramNode(parallelogramNode, materialMap);
            sceneConfig.Parallelograms.push_back(geometry);

            return geometry;
		}
		else
		{
            return std::shared_ptr<const IntersectableGeometry>{};
		}
	}

	export std::unique_ptr<SceneConfig> ParseSceneNode(const Node& node, const MaterialMap& materialMap)
	{
        auto config = std::unique_ptr<SceneConfig>(new SceneConfig{});

		for (const Node& childNode : node["areaLights"])
		{
			auto areaLight = ParseAreaLightNode(childNode, *config, materialMap);
			config->AreaLights.push_back(areaLight);
		}

		for (const Node& childNode : node["geometries"])
		{
			auto geometry = ParseGeometryNode(childNode, *config, materialMap);
		}

		return config;
	}
}