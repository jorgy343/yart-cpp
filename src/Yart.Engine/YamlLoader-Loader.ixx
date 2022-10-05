module;

#include "yaml-cpp/yaml.h"

export module YamlLoader:Loader;

import "Common.h";

import :Cameras;
import :Config;
import :Geometry;
import :Lights;
import :Materials;
import Camera;
import IntersectableGeometry;
import Light;

using namespace YAML;

namespace Yart::Yaml
{
    export class YamlData
    {
    public:
        std::shared_ptr<Config> Config{};
        std::shared_ptr<Camera> Camera{};
        std::shared_ptr<MaterialMap> MaterialMap{};
        std::vector<std::shared_ptr<const Light>> Lights{};
        std::shared_ptr<ParseGeometryResults> GeometryData{};
    };

    export std::shared_ptr<YamlData> LoadYaml()
    {
        Node node = LoadFile("../../../../Yart.Engine/scene.yaml");

        std::shared_ptr<Config> config = ParseConfigNode(node["config"]);
        std::shared_ptr<Camera> camera = ParseCameraNode(node["camera"]);
        std::shared_ptr<MaterialMap> materialMap = ParseMaterialsNode(node["materials"]);
        std::vector<std::shared_ptr<const Light>> lights = ParseLightsNode(node["lights"]);
        std::shared_ptr<ParseGeometryResults> geometryDataPointer = ParseSceneNode(node["geometry"], *materialMap);

        return std::make_shared<YamlData>(
            config,
            camera,
            materialMap,
            lights,
            geometryDataPointer);
    }
}