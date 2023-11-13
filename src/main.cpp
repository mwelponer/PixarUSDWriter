#include <iostream>
#include <fstream>
#include <vector>
#include "PixarUSDWriter.h"

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


int main() {
    std::string plyFilePath = "../data/f16.ply";
    std::string USDFilePath = "../data/out.usda";

    std::vector<pxr::GfVec3f> vertices;
    pxr::VtArray<int> vtFaceVertexCounts;
    pxr::VtArray<int> vtFaceVertexIndices;

    readPlyFile(plyFilePath, vertices, vtFaceVertexCounts, vtFaceVertexIndices);

    try {
        // Create PixarUSDWriter instance
        PixarUSDWriter usdWriter(USDFilePath);

        // Add a mesh to the stage
        usdWriter.AddMesh("mesh", vertices, vtFaceVertexCounts, vtFaceVertexIndices);

        // Add a layer
        usdWriter.AddLayer("../data/cube.usda");

        // Save the stage to disk
        usdWriter.SaveStage();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}   