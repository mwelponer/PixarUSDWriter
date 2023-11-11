#include <iostream>
#include <fstream>
#include <vector>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/points.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/vt/array.h>


// Function to read a .ply file and store vertices and faces
bool readPlyFile(const std::string& filename,
                 std::vector<pxr::GfVec3f>& vertices,
                 pxr::VtArray<int>& vtFaceVertexCounts, 
                 pxr::VtArray<int>& vtFaceVertexIndices) {

   std::ifstream plyFile(filename);
    if (!plyFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    std::string line;
    int numVertices, vertexCount = 0;
    bool readingVertices = false;
    bool readingFaces = false;

    while (std::getline(plyFile, line)) {
        //std::cout << "line: " << line << std::endl;
        std::istringstream iss(line);

        if (readingVertices) {
            pxr::GfVec3f vertex;
            iss >> vertex[0] >> vertex[1] >> vertex[2];
            vertices.push_back(vertex);

            if (++vertexCount == numVertices){
                readingVertices = false;
                readingFaces = true;
            }

        } else if (readingFaces) {
            int numVerticesInFace;

            iss >> numVerticesInFace;
            vtFaceVertexCounts.push_back(numVerticesInFace);

            int vertexIndex;
            for (int i = 0; i < numVerticesInFace; ++i) {
                iss >> vertexIndex;
                vtFaceVertexIndices.push_back(vertexIndex);
            }

        } else if (line.find("end_header") != std::string::npos) {
            readingVertices = true;

        } else if (line.find("element vertex") != std::string::npos) {
            iss >> line >> line >> numVertices;
            vertices.reserve(numVertices);

        } else if (line.find("element face") != std::string::npos) {
            int numFaces;
            iss >> line >> line >> numFaces;
        }
    }

    return true;
}

bool writeUsdFile(const std::string& filename, 
                 std::vector<pxr::GfVec3f>& vertices,
                 pxr::VtArray<int>& vtFaceVertexCounts, 
                 pxr::VtArray<int>& vtFaceVertexIndices){

   // Create a new USD stage
    pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew(filename);

    if (!stage) {
        std::cerr << "Failed to create USD stage!" << std::endl;
        return false;
    } else {
        std::cout << "stage created." << std::endl;
    }

    // Convert the vector of vertices to VtArray
    pxr::VtArray<pxr::GfVec3f> vtVertices(vertices.begin(), vertices.end());

    // Create a points primitive on the stage
    pxr::UsdGeomPoints points = pxr::UsdGeomPoints::Define(stage, pxr::SdfPath("/myPoints"));
    points.GetPointsAttr().Set(vtVertices);

    std::cout << "point primitive created." << std::endl;

    // Create a mesh primitive on the stage
    pxr::UsdGeomMesh mesh = pxr::UsdGeomMesh::Define(stage, pxr::SdfPath("/myMesh"));
    mesh.GetFaceVertexCountsAttr().Set(vtFaceVertexCounts);
    mesh.GetFaceVertexIndicesAttr().Set(vtFaceVertexIndices);

    std::cout << "mesh primitive created." << std::endl;

    // Save the stage to the .usda file
    if (!stage->GetRootLayer()->Save()) {
        std::cerr << "Failed to save USD stage to file!" << std::endl;
        return false;
    }

    return true;
}

int main() {
    //std::cout << "Hello World" << std::endl;

    std::string plyFilePath = "../data/f16.ply";
    std::string usdaFilePath = "../data/f16.usdc";

    std::vector<pxr::GfVec3f> vertices;
    //std::vector<std::vector<int>> faces;
    pxr::VtArray<int> vtFaceVertexCounts;
    pxr::VtArray<int> vtFaceVertexIndices;

    readPlyFile(plyFilePath, vertices, vtFaceVertexCounts, vtFaceVertexIndices);

    // if (readPlyFile(plyFilePath, vertices, vtFaceVertexCounts, vtFaceVertexIndices)) {
    //     // Print vertices
    //     std::cout << "Vertices:" << std::endl;
    //     for (const auto& vertex : vertices) {
    //         std::cout << "  vertex " << vertex << std::endl;
    //     }
    //     // Print faces
    //     std::cout << "Faces:" << std::endl;
    //     int begin = 0;
    //     for (int vtCount : vtFaceVertexCounts) {
    //         std::cout << "  face (" << vtCount << ") ";

    //         int end = begin + vtCount;
    //         for (int v=begin; v < end; v++) {
    //             std::cout << vtFaceVertexIndices[v] << " ";
    //             begin++;
    //         }
    //         std::cout << std::endl;
    //     }
    // }

    if (writeUsdFile(usdaFilePath, vertices, vtFaceVertexCounts, vtFaceVertexIndices)){
        std::cout << "Conversion successful. Output saved to: " << usdaFilePath << std::endl;
    }

    return 0;
}   