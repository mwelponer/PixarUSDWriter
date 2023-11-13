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

    std::vector<pxr::GfVec3f> vertices;
    std::vector<pxr::GfVec3f> normals;
    pxr::VtArray<int> vtFaceVertexCounts;
    pxr::VtArray<int> vtFaceVertexIndices;

    void ImportPly(std::ifstream& ifs);
    void ImportObj(std::ifstream& ifs);
    //void ImportH(std::ifstream& ifs);

public:
    // Constructor, passing the path of the main USD output file
    PixarUSDWriter(const std::string& filePath);

    // Import mesh from an external file
    void ImportMesh(const std::string& filePath);

    // Add a mesh to the stage 
    void AddMesh(const std::string& meshName);

    // Add a layer to the stage
    void AddLayer(const std::string& layerPath);

    // Save the stage to disk
    void SaveStage();
};