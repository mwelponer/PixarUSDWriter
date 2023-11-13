#include <iostream>
#include <fstream>
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

void PixarUSDWriter::ImportPly(std::ifstream& ifs){
    std::string line;
    int numVertices, vertexCount = 0;
    bool readingVertices = false;
    bool readingFaces = false;

    while (std::getline(ifs, line)) {
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
}

void PixarUSDWriter::ImportObj(std::ifstream& ifs){
    std::string line;

    while (std::getline(ifs, line)) {
        //std::cout << "line: " << line << std::endl;
        std::istringstream iss(line);
        std::string element;
        iss >> element;

        if (element == "vn"){
            pxr::GfVec3f nVertex;
            iss >> nVertex[0] >> nVertex[1] >> nVertex[2];
            normals.push_back(nVertex);

        } else if (element == "v"){
            pxr::GfVec3f vertex;
            iss >> vertex[0] >> vertex[1] >> vertex[2];
            vertices.push_back(vertex);
        } else if (element == "f"){
            int index;
            int numVerticesInFace = 0;
            while (iss >> index) {
                //std::cout << "index: " << index << std::endl;
                vtFaceVertexIndices.push_back(index - 1);
                numVerticesInFace++;
            }
            vtFaceVertexCounts.push_back(numVerticesInFace);
        }
    }
}

// void PixarUSDWriter::ImportH(std::ifstream& ifs){}

void PixarUSDWriter::ImportMesh(const std::string& filePath){
    std::string fileExtension;

    // Check if a dot was found and it is not at the beginning or end of the filename
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos > 0 && dotPos < filePath.length() - 1){
        fileExtension = filePath.substr(dotPos + 1);
        //std::cout << "file extension: " << fileExtension << std::endl;
    }else return;

    // open the file
    std::ifstream ifs(filePath);
    if (!ifs.is_open()){
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    // check the file type by extension
    if (fileExtension == "ply"){
        ImportPly(ifs);
    }else if (fileExtension == "obj"){
        ImportObj(ifs);
    }
    // else if (fileExtension == "h"){
    //     ImportH(ifs);
    // }
    else{
        std::cerr << "File extension ."<< fileExtension 
            << " not supported. No mesh was imported." << std::endl;
    }

    // close the file 
    ifs.close();
}

// Add a mesh to the stage with vertices and faces
void PixarUSDWriter::AddMesh(const std::string& meshName) {

    // check if any mesh was imported 
    if (vertices.empty()) return;

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

    // clear the data vectors
    vertices.clear();
    vtFaceVertexCounts.clear();
    vtFaceVertexIndices.clear();

    // Set normals if available
    if (!normals.empty()){
        pxr::UsdAttribute normalsAttr = mesh.GetNormalsAttr();
        // Convert the vector of normals to VtArray
        pxr::VtArray<pxr::GfVec3f> vtNormals(normals.begin(), normals.end());
        normalsAttr.Set(vtNormals);

        normals.clear();
    }
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
