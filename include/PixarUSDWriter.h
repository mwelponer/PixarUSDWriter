#pragma once

#include <vector>
#include <string>
#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/usd/stage.h>
class PixarUSDWriter {

private:
    std::string filePath;
    pxr::UsdStageRefPtr stage;
    pxr::SdfLayerRefPtr rootLayer;

public:
    // Constructor, passing the path of the main USD file
    PixarUSDWriter(const std::string& filePath);

    // Add a mesh to the stage with vertices and faces
    void AddMesh(const std::string& meshName, 
        const std::vector<pxr::GfVec3f>& vertices, 
        const pxr::VtArray<int>& vtFaceVertexCounts, 
        const pxr::VtArray<int>& vtFaceVertexIndices);

    // Add a layer to the stage
    void AddLayer(const std::string& layerPath);

    // Save the stage to disk
    void SaveStage();
};