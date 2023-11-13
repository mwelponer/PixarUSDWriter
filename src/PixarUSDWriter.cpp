#include "PixarUSDWriter.h"
#include <pxr/base/vt/array.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xform.h>

PixarUSDWriter::PixarUSDWriter(const std::string& filePath) : filePath(filePath) {
    // Create a new USD stage
    stage = pxr::UsdStage::CreateNew(filePath);
    if (!stage) {
        throw std::runtime_error("Failed to create USD stage.");
    }

    // Define a root layer for the stage
    rootLayer = stage->GetRootLayer();
}

// Add a mesh to the stage with vertices and faces
void PixarUSDWriter::AddMesh(const std::string& meshName, 
    const std::vector<pxr::GfVec3f>& vertices, 
    const pxr::VtArray<int>& vtFaceVertexCounts, 
    const pxr::VtArray<int>& vtFaceVertexIndices) {

    // Create an Xform (transform) for the mesh
    pxr::UsdGeomXform xform = 
        pxr::UsdGeomXform::Define(stage, pxr::SdfPath("/" + meshName));

    // Create a Mesh under the Xform
    pxr::UsdGeomMesh mesh = 
        pxr::UsdGeomMesh::Define(stage, xform.GetPath().AppendChild(pxr::TfToken(meshName)));

    // Convert the vector of vertices to VtArray
    pxr::VtArray<pxr::GfVec3f> vtVertices(vertices.begin(), vertices.end());
    // Set the points attribute for the mesh
    pxr::UsdAttribute pointsAttr = mesh.GetPointsAttr();
    pointsAttr.Set(vtVertices);

    // Set the faceVertexCounts and faceVertexIndices attributes for the mesh
    pxr::UsdAttribute faceVertexCountsAttr = mesh.GetFaceVertexCountsAttr();
    faceVertexCountsAttr.Set(vtFaceVertexCounts);
    pxr::UsdAttribute faceVertexIndicesAttr = mesh.GetFaceVertexIndicesAttr();
    faceVertexIndicesAttr.Set(vtFaceVertexIndices);
}

// Add a layer to the stage
void PixarUSDWriter::AddLayer(const std::string& layerPath) {
    // Make the layer a subLayer of the root layer
    pxr::SdfLayerRefPtr subLayer = pxr::SdfLayer::FindOrOpen(layerPath);
    rootLayer->GetSubLayerPaths().push_back(subLayer->GetIdentifier());
}

// Save the stage to disk
void PixarUSDWriter::SaveStage() {
    rootLayer->Save();
}
