module;

#include "yaml-cpp/yaml.h"

export module YamlLoader:MissShaders;

import <functional>;

import "Common.h";

import :Vectors;
import Math;
import MissShader;
import ConstantMissShader;
import AtmosphereMissShader;

using namespace YAML;

namespace Yart::Yaml
{
    std::shared_ptr<ConstantMissShader> ParseConstantMissShader(const Node& node)
    {
        Vector3 color = node["color"].as<Vector3>();

        return std::make_unique<ConstantMissShader>(color);
    }

    std::shared_ptr<AtmosphereMissShader> ParseAtmosphereMissShader(const Node& node)
    {
        Vector3 sunDirection = node["sunDirection"].as<Vector3>();
        Vector3 origin = node["origin"].as<Vector3>();

        return std::make_unique<AtmosphereMissShader>(sunDirection, origin);
    }

    static std::vector<std::tuple<std::string, std::function<std::shared_ptr<MissShader>(const Node&)>>> MissShaderMapFunctions
    {
        {"constant", &ParseConstantMissShader},
        {"atmosphere", &ParseAtmosphereMissShader},
    };

    export std::shared_ptr<MissShader> ParseMissShaderNode(const Node& node)
    {
        for (const auto& [nodeName, functionPointer] : MissShaderMapFunctions)
        {
            auto childNode = node[nodeName];
            if (childNode)
            {
                auto missShader = functionPointer(childNode);
                return missShader;
            }
        }

        return {};
    }
}