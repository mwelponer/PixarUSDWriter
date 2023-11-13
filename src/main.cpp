#include <iostream>
#include <fstream>
#include <vector>
#include "PixarUSDWriter.h"

int main() {
    std::string ply1_FilePath = "../data/cube.ply";
    std::string ply2_FilePath = "../data/f16.ply";
    std::string obj_FilePath = "../data/teapot.obj";
    std::string usda_FilePath = "../data/box.usda";

    try {
        // Create PixarUSDWriter instance
        PixarUSDWriter usdWriter("../data/out.usda");

        // import ply1 from an external file
        usdWriter.ImportMesh(obj_FilePath);
        // Add the mesh to the stage
        usdWriter.AddMesh("mesh1");

        // import ply2 from an external file
        usdWriter.ImportMesh(ply2_FilePath);
        // Add the mesh to the stage
        usdWriter.AddMesh("mesh2");

        // Add a layer
        // usdWriter.AddLayer(usda_FilePath);

        // Save the stage to disk
        usdWriter.SaveStage();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}   