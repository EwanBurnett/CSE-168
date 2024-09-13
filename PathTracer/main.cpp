#include "Utils/Logger.h"
#include "Utils/Timer.h"
#include "Utils/ProgressBar.h"
#include "RayTracer.h"
#include <thread>
#include <mutex> 
#include <filesystem>
constexpr uint16_t WIDTH = 1920;
constexpr uint16_t HEIGHT = 1080;
const char* OUTPUT_NAME = "Test";
uint32_t NUM_THREADS = std::thread::hardware_concurrency();
const char* OUTPUT_DIRECTORY = "Output";
const char* SCENE_PATH = "Scenes/HW3/scene5.test";
constexpr uint32_t MAX_DEPTH = 5;


#define ENABLE_DEBUG_SCENE 1

void ExportImage(const EDX::Image& img, const std::string& outputName, const float gamma = 1.0f);

int main(int argc, char* argv[]) {
    EDX::Log::Status("EDX UC San Diego CS-168 Rendering 2 Coursework\nEwan Burnett - 2024\n");

    //Load the scene 
    EDX::RenderData renderData = {};
    renderData.accelGrid = EDX::Acceleration::Grid({ 1, 1, 1 }); 

    std::string scenePath = SCENE_PATH; 
    if (argc > 1) { 
        scenePath = argv[1]; 
    }

    if (!EDX::RayTracer::LoadSceneFile(scenePath.c_str(), renderData))
#if ENABLE_DEBUG_SCENE
    {   //If we can't load a scene, load the debug scene instead. 
        //Context
        {
            renderData.outputName = OUTPUT_NAME;
            renderData.dimensions.x = WIDTH;
            renderData.dimensions.y = HEIGHT;
            renderData.maxDepth = MAX_DEPTH;
        }
        //Camera
        {
            EDX::Maths::Vector3f lookFrom = { 0.0f,-4.0f, -4.0f };
            EDX::Maths::Vector3f lookAt = { 0.0f, -1.0f, 0.0f };
            EDX::Maths::Vector3f up = { 0.0f, 1.0f, -1.0f };
            renderData.camera = EDX::Camera(lookFrom, lookAt, up, EDX::Maths::DegToRad(45.0));
        }
        //Scene
        {
            EDX::BlinnPhong mat = {};
            mat.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
            mat.emission = { 0.6f, 0.0f, 0.0f, 1.0f };
            mat.diffuse = { 0.4f, 0.0f, 0.0f, 1.0f };
            mat.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
            mat.shininess = 80.0f;

            EDX::Maths::Quaternion q; 
            q = EDX::Maths::Quaternion::FromAxisAngle(EDX::Maths::Vector3d{ 0.0, 0.0, 1.0 }, EDX::Maths::DegToRad(45.0)); 

            EDX::Maths::Matrix4x4<float> transform = (EDX::Maths::Matrix4x4<float>::Scaling({ 1.0f, .25f, .25f }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, 0.5f });

            renderData.scene.PointLights().push_back({
                { 1.0f, 1.0f, 3.0f  }, {1.0f, 1.0f, 1.0f},  { 1.0f, 1.0f, 1.0f,1.0f }
                });

            //Cube
            std::vector<EDX::Maths::Vector3f> verts = {
                {-1.0f, -1.0f, -1.0f},
                {1.0f, -1.0f, -1.0f},
                {1.0f, 1.0f, -1.0f},
                {-1.0f, 1.0f, -1.0f},
                {-1.0f, -1.0f, 1.0f},
                {1.0f, -1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f},
                {-1.0f, 1.0f, 1.0f}
            };

            EDX::Triangle t = { {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} ,{1.0f, -1.0f, 0.0f} };
            t.SetMaterial(mat);
            t.SetWorldMatrix(transform);
            //renderData.scene.Triangles().push_back(t);
/*
            renderData.scene.Triangles().push_back({ verts[0], verts[1], verts[5] });
            renderData.scene.Triangles().push_back({ verts[0], verts[5], verts[4] });
            renderData.scene.Triangles().push_back({ verts[3], verts[7], verts[6] });
            renderData.scene.Triangles().push_back({ verts[3], verts[6], verts[2] });
            renderData.scene.Triangles().push_back({ verts[1], verts[2], verts[6] });
            renderData.scene.Triangles().push_back({ verts[1], verts[6], verts[5] });
            renderData.scene.Triangles().push_back({ verts[0], verts[7], verts[3] });
            renderData.scene.Triangles().push_back({ verts[0], verts[4], verts[7] });
            renderData.scene.Triangles().push_back({ verts[0], verts[3], verts[2] });
            renderData.scene.Triangles().push_back({ verts[0], verts[2], verts[1] });
            renderData.scene.Triangles().push_back({ verts[4], verts[5], verts[6] });
            renderData.scene.Triangles().push_back({ verts[4], verts[6], verts[7] });

            for (auto& t : renderData.scene.Triangles()) {
                t.SetWorldMatrix(transform);
                t.SetMaterial(mat);
            }
*/


            EDX::Sphere s = { {0.0f, 0.0f, 0.0f}, 1.0f };
            s.SetMaterial(mat);
            s.SetWorldMatrix(transform);
            renderData.scene.Spheres().push_back(s);

            EDX::Plane p = { {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 100.0f} };
            p.SetMaterial(mat);
            //renderData.scene.Planes().push_back(p);

            /*
            EDX::Plane p = { {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -10.0f} };
            p.SetMaterial(mat);
            renderData.scene.Planes().push_back(p);
            */
        }
    }
#else 
    {
        EDX::Log::Failure("Failed to load scene!\n");
        return 1;
    }
#endif

    EDX::Log::Print("Image Size: (%d x %d)\nMax Depth: %d\nTriangles: %d\nSpheres: %d\nDirectional Lights: %d\nPoint Lights: %d\n", renderData.dimensions.x, renderData.dimensions.y, renderData.maxDepth, renderData.scene.Triangles().size(), renderData.scene.Spheres().size(), renderData.scene.DirectionalLights().size(), renderData.scene.PointLights().size());
    renderData.accelGrid.BuildAccelerationStructure(renderData);


    EDX::Log::Status("Rendering Image \"%s\" \n", renderData.outputName.c_str());
    EDX::ProgressBar pb;

    EDX::Image img(renderData.dimensions.x, renderData.dimensions.y);

    //Compute Horizontal FoV wrt Vertical FoV and Aspect Ratio
    {
        const float aspectRatio = (float)renderData.dimensions.x / (float)renderData.dimensions.y;

        renderData.FoV.x = (2.0f * atan(tan(renderData.camera.GetFoVRadians() * 0.5f) * aspectRatio));
        renderData.FoV.y = renderData.camera.GetFoVRadians();
    }

    //Render the Image
    std::atomic<uint32_t> pixelsProcessed(0);
    uint32_t totalPixels = renderData.dimensions.x * renderData.dimensions.y;   //TODO: Image::Size()

    std::mutex progressLock;
    auto render = [&](EDX::Maths::Vector2<uint32_t> dim_x, EDX::Maths::Vector2<uint32_t> dim_y)
    {
        for (uint32_t y = dim_y.x; y < dim_y.y; y++) {
            for (uint32_t x = dim_x.x; x < dim_x.y; x++) {

                const EDX::Colour pixelColour = EDX::RayTracer::RenderPixel(x, y, renderData);

                img.SetPixel(x, y, pixelColour);
                pixelsProcessed++;
            }

            //Only update the progress bar in the outer part of the loop, as it's SLOW. 
            //Exclusive access across threads via a mutex
            if (progressLock.try_lock())
            {
                float p = (float)(pixelsProcessed) / (float)(totalPixels);
                pb.Update(p);
                progressLock.unlock();
            }
        }
    };


    const uint32_t num_threads = NUM_THREADS;
    std::vector<std::thread> threads(num_threads - 1);

    //Kick off worker threads, each rendering a column of the image. 
    for (int i = 1; i < num_threads; i++) {
        threads[i - 1] = std::thread([&](int idx) {

            thread_local uint32_t startX = (renderData.dimensions.x / num_threads) * idx;
            thread_local uint32_t countX = (renderData.dimensions.x / num_threads);
            thread_local uint32_t startY = 0;
            thread_local uint32_t countY = renderData.dimensions.y;

            render({ startX, startX + countX }, { startY, startY + countY });

            }, i);
    }

    //Have the main thread render the remainder. 
    render(
        { 0, renderData.dimensions.x - ((renderData.dimensions.x / num_threads) * (num_threads - 1)) },
        { 0, renderData.dimensions.y }
    );

    //Wait for the worker threads to complete.
    uint32_t completeThreads = 0;
    while (completeThreads < (num_threads - 1)) {
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
                completeThreads++;
            }

        }
    }

    //Report 
    const double render_time_s = pb.GetProgressTimer().Duration();
    EDX::Log::Success("\nRender Complete in %.8fs.\n", render_time_s);

    ExportImage(img, renderData.outputName);

    return 0;
}



void ExportImage(const EDX::Image& img, const std::string& outputName, const float gamma)
{
    EDX::Log::Status("Exporting Render to PNG...\n");
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
