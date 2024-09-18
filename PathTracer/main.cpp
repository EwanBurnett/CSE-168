#define ENABLE_VIEWER 1
#define ENABLE_DEBUG_SCENE 0

#include "Utils/Logger.h"
#include "Utils/Timer.h"
#include "Utils/ProgressBar.h"
#include "RayTracer.h"
#include <filesystem>

#if ENABLE_VIEWER
#include "Viewer.h"
#endif

constexpr uint16_t WIDTH = 600;
constexpr uint16_t HEIGHT = 400;
const char* OUTPUT_NAME = "Test";
const char* OUTPUT_DIRECTORY = "Output";
const char* SCENE_PATH = "Scenes/HW1/scene5.test";
constexpr uint32_t MAX_DEPTH = 2;


void ExportImage(const EDX::Image& img, const std::string& outputName, const float gamma = 1.0f);

int main(int argc, char* argv[]) {
    EDX::Log::Status("EDX UC San Diego CS-168 Rendering 2 Coursework\nEwan Burnett - 2024\n");

#if ENABLE_VIEWER
    EDX::Log::Status("Viewer Enabled.\n");
    
    EDX::Viewer viewer;

    viewer.Init(); 
    while (viewer.PollEvents()) {
        viewer.Update(); 
    }

    viewer.Shutdown(); 

    
#else
    //Load the scene 
    EDX::RenderData renderData = {};

    std::string scenePath = SCENE_PATH;
    if (argc > 1) {
        scenePath = argv[1];
    }

    if (!EDX::RayTracer::LoadSceneFile(scenePath.c_str(), renderData))
    {
        EDX::Log::Failure("Failed to load scene!\n");
        return 1;
    }

    EDX::RayTracer rayTracer; 
    EDX::Image img(renderData.dimensions.x, renderData.dimensions.y);

    rayTracer.Settings().numThreads = 2; 
    rayTracer.Settings().gridDim = { 5, 5, 5 };
    rayTracer.Render(renderData, img); 

    ExportImage(img, renderData.outputName);
#endif
    return 0;
}



void ExportImage(const EDX::Image& img, const std::string& outputName, const float gamma)
{
    EDX::Log::Status("Exporting %s to PNG...\n", outputName.empty() ? "Render" : outputName.c_str());
    std::filesystem::create_directory(OUTPUT_DIRECTORY);

    //Format the filename in relation to the output path. 
    char buffer[0xff];
    strcpy(buffer, OUTPUT_DIRECTORY);
    strcat(buffer, "/");
    if (!outputName.empty()) {
        strcat(buffer, outputName.c_str());
        strcat(buffer, "_");
    }

    //Format a timestamp for this render
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    size_t len = strlen(buffer);
    strftime(buffer + len, 0xff - len, "%Y-%m-%d_%H-%M-%S.png", &tm);

    EDX::Log::Status("Output Path: %s/%s\n", std::filesystem::current_path().generic_string().c_str(), buffer);
    img.ExportToPNG(buffer, gamma);

    EDX::Log::Status("Export Complete.\n");
}
