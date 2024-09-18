#include "Utils/Logger.h"
#include "Utils/Timer.h"
#include "Utils/ProgressBar.h"
#include "RayTracer.h"
#include "Containers/TS_Stack.h"
#include <thread>
#include <atomic>
#include <mutex> 
#include <filesystem>

constexpr uint16_t WIDTH = 600;
constexpr uint16_t HEIGHT = 400;
const char* OUTPUT_NAME = "Test";
uint32_t NUM_THREADS = std::thread::hardware_concurrency();
const char* OUTPUT_DIRECTORY = "Output";
const char* SCENE_PATH = "Scenes/HW1/scene5.test";
constexpr uint32_t MAX_DEPTH = 2;


#define ENABLE_DEBUG_SCENE 0


void ExportImage(const EDX::Image& img, const std::string& outputName, const float gamma = 1.0f);

int main(int argc, char* argv[]) {
    EDX::Log::Status("EDX UC San Diego CS-168 Rendering 2 Coursework\nEwan Burnett - 2024\n");

    //Load the scene 
    EDX::RenderData renderData = {};
    renderData.accelGrid = EDX::Acceleration::Grid({ 10, 10, 10 });

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
            EDX::Maths::Vector3f lookFrom = { .0f, 0.0f, -3.0f };

            //EDX::Maths::Vector3f lookFrom = { 0.0f, -4.0f, 4.0f };  //default
            //EDX::Maths::Vector3f lookFrom = { 5.65f, 0.0f, 0.0f };    //90 deg
            //EDX::Maths::Vector3f lookFrom = { -5.65f, 0.0f, 0.0f };    //-90 deg
            //EDX::Maths::Vector3f lookFrom = { 0.0f, 4.0f, -4.0f };    //180 deg
            //EDX::Maths::Vector3f lookFrom = { 4.0f,-2.82843f,2.82843f };  //45 deg
            //EDX::Maths::Vector3f lookFrom = { -4.0f,-2.82843f,2.82843f };  //45 deg
            //EDX::Maths::Vector3f lookFrom = { 4.89898f,-2.0f,2.0f };  //60 deg
            //EDX::Maths::Vector3f lookFrom = { 2.82843,-3.4641,3.4641 };  //30 deg
            //EDX::Maths::Vector3f lookFrom = { 0.49303,-3.98478,3.98478 };  //5 deg
            //EDX::Maths::Vector3f lookFrom = { 1.4641,-3.8637,3.8637 };  //15 deg

            EDX::Maths::Vector3f lookAt = { 0.0f, 0.0f, 0.0f };
            EDX::Maths::Vector3f up = { 0.0f, 1.0f, 0.0f };
            renderData.camera = EDX::Camera(lookFrom, lookAt, up, EDX::Maths::DegToRad(95.0));
        }
        //Scene
        {
            {
                {
                    EDX::BlinnPhong mat = {};
                    mat.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
                    mat.emission = { 0.0f, 0.0f, 0.0f, 1.0f };
                    mat.diffuse = { 0.0f, 0.0f, 0.6f, 1.0f };
                    mat.specular = { 0.1f, 0.1f, 0.1f, 1.0f };
                    mat.shininess = 0.9f;

                    EDX::Maths::Quaternion q;
                    q = EDX::Maths::Quaternion::FromAxisAngle(EDX::Maths::Vector3d{ 0.0, 0.0, 1.0 }, EDX::Maths::DegToRad(0.0));

                    EDX::Maths::Matrix4x4<float> transform = EDX::Maths::Matrix4x4<float>::Scaling({ 2.0f, 1.0f, 0.25f }) * (q.ToMatrix4x4() * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, 0.0f }));

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
                }
                {
                    EDX::BlinnPhong mat = {};
                    mat.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
                    mat.emission = { 0.3f, 0.0f, 0.0f, 1.0f };
                    mat.diffuse = { 0.5f, 0.0f, 0.0f, 1.0f };
                    mat.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
                    mat.shininess = 0.6f;

                    EDX::BlinnPhong mat2 = {};
                    mat2.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
                    mat2.emission = { 0.0f, 0.3f, 0.0f, 1.0f };
                    mat2.diffuse = { 0.0f, 0.5f, 0.0f, 1.0f };
                    mat2.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
                    mat2.shininess = 0.6f;

                    EDX::Maths::Quaternion q;
                    q = EDX::Maths::Quaternion::FromAxisAngle(EDX::Maths::Vector3d{ 0.0, 0.0, 1.0 }, EDX::Maths::DegToRad(45.0));


                    const float scl = 0.25f;

                    EDX::Maths::Matrix4x4<float> transform = (
                        EDX::Maths::Matrix4x4<float>::Scaling({ 1.0f, scl, scl }) *
                        //EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, -0.5f }) *
                        q.ToMatrix4x4()) *
                        EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, 0.5f });

                    EDX::Sphere s({ 0.0f, 0.0f, 0.0f }, 1.0f);
                    s.SetMaterial(mat);
                    s.SetWorldMatrix(transform);
                    renderData.scene.Spheres().push_back(s);


                    q = EDX::Maths::Quaternion::FromAxisAngle(EDX::Maths::Vector3d{ 0.0, 0.0, 1.0 }, EDX::Maths::DegToRad(-45.0));
                    //EDX::Maths::Matrix4x4<float> transform2 = (EDX::Maths::Matrix4x4<float>::Scaling({ 1.0f, scl, scl }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, 0.5f });
                    EDX::Maths::Matrix4x4<float> transform2 = (EDX::Maths::Matrix4x4<float>::Scaling({ 1.0f, scl, scl }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, 0.5f });

                    EDX::Sphere s2({ 0.0f, 0.0f, 0.0f }, 1.0f);
                    s2.SetWorldMatrix(transform2);
                    s2.SetMaterial(mat2);
                    renderData.scene.Spheres().push_back(s2);

                }
            }
        }
    }
    /*
    EDX::Sphere s = { {0.0f, 0.0f, 0.0f}, 1.0f };
    s.SetMaterial(mat);
    s.SetWorldMatrix(transform);
    renderData.scene.Spheres().push_back(s);

    EDX::Plane p = { {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 100.0f} };
    p.SetMaterial(mat);
    //renderData.scene.Planes().push_back(p);

    EDX::Plane p = { {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -10.0f} };
    p.SetMaterial(mat);
    renderData.scene.Planes().push_back(p);
    {
        {
            EDX::BlinnPhong mat = {};
            mat.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
            mat.emission = { 0.0f, 0.0f, 0.0f, 1.0f };
            mat.diffuse = { 0.0f, 0.0f, 0.6f, 1.0f };
            mat.specular = { 0.1f, 0.1f, 0.1f, 1.0f };
            mat.shininess = 0.9f;

            EDX::BlinnPhong mat2 = {};
            mat2.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
            mat2.emission = { 0.0f, 0.3f, 0.0f, 1.0f };
            mat2.diffuse = { 0.0f, 0.5f, 0.0f, 1.0f };
            mat2.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
            mat2.shininess = 0.6f;



            EDX::Maths::Quaternion q;
            EDX::Maths::Quaternion q2;
            q = EDX::Maths::Quaternion::FromAxisAngle(EDX::Maths::Vector3d{ 0.0, 0.0, 1.0 }, EDX::Maths::DegToRad(0.0));

            // EDX::Maths::Matrix4x4<float> transform = (EDX::Maths::Matrix4x4<float>::Scaling({ 1.0f, 1.0f, 1.0f }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 1.0f, 1.0f });

            renderData.scene.PointLights().push_back({
                { 0.0f, 5.0f, 0.0f }, {3.0f, 1.0f, 1.0f},  { 1.0f, 1.0f, 1.0f,1.0f }
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

            auto addCube = [&](EDX::Maths::Matrix4x4<float> tfm) {

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
                    t.SetWorldMatrix(tfm);
                    t.SetMaterial(mat);
                }
            };


            EDX::Maths::Matrix4x4<float> transform1 =
(EDX::Maths::Matrix4x4<float>::Scaling({ 0.5f, 0.5f, 0.5f}) *
                      q.ToMatrix4x4()) *
                      EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, 0.0f });
                  addCube(transform1);

                  EDX::Maths::Matrix4x4<float> transform2 =
                   (EDX::Maths::Matrix4x4<float>::Scaling({ 0.5f, 0.5f, 0.5f }) *
                      q.ToMatrix4x4()) *
                      EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, .0f, 0.0f });
                  addCube(transform2);



                  {
                      EDX::BlinnPhong mat = {};
                      mat.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
                      mat.emission = { 0.3f, 0.0f, 0.0f, 1.0f };
                      mat.diffuse = { 0.5f, 0.0f, 0.0f, 1.0f };
                      mat.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
                      mat.shininess = 0.6f;


                      EDX::Maths::Quaternion q;
                      EDX::Maths::Quaternion q2;
                      q = EDX::Maths::Quaternion::FromAxisAngle(EDX::Maths::Vector3d{ 0.0, 0.0, 1.0 }, EDX::Maths::DegToRad(0.0));
                      // q2 = EDX::Maths::Quaternion::FromAxisAngle(EDX::Maths::Vector3d{ 1.0, 0.0, 0.0 }, EDX::Maths::DegToRad(90.0));

                      const float scl = 0.1;
                      const float scl2 = 0.5f;

                      EDX::Maths::Matrix4x4<float> transform;

                      EDX::Sphere s({ 0.0f, 0.0f, 0.0f }, 1.0f);
                      s.SetMaterial(mat);
                      transform = (EDX::Maths::Matrix4x4<float>::Scaling({ scl2, scl, scl }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 2.0f, 0.0f });
                      s.SetWorldMatrix(transform);
                      renderData.scene.Spheres().push_back(s);
                      s.SetMaterial(mat2);
                      transform = (EDX::Maths::Matrix4x4<float>::Scaling({ scl, scl2, scl }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, -2.0f, 0.0f });
                      s.SetWorldMatrix(transform);
                      renderData.scene.Spheres().push_back(s);
                      s.SetMaterial(mat);
                      transform = (EDX::Maths::Matrix4x4<float>::Scaling({ scl2, scl, scl }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 2.0f, 0.0f, 0.0f });
                      s.SetWorldMatrix(transform);
                      renderData.scene.Spheres().push_back(s);
                      s.SetMaterial(mat2);
                      transform = (EDX::Maths::Matrix4x4<float>::Scaling({ scl, scl2, scl }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ -2.0f, 0.0f, 0.0f });
                      s.SetWorldMatrix(transform);
                      renderData.scene.Spheres().push_back(s);
                      s.SetMaterial(mat);
                      transform = (EDX::Maths::Matrix4x4<float>::Scaling({ scl2, scl, scl }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, 2.0f });
                      s.SetWorldMatrix(transform);
                      renderData.scene.Spheres().push_back(s);
                      s.SetMaterial(mat2);
                      transform = (EDX::Maths::Matrix4x4<float>::Scaling({ scl, scl2, scl }) * q.ToMatrix4x4()) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, -2.0f });
                      s.SetWorldMatrix(transform);
                      renderData.scene.Spheres().push_back(s);

                  }

              }
          }
      }
  }
      */
#else 
    {
        EDX::Log::Failure("Failed to load scene!\n");
        return 1;
    }
#endif

    EDX::Log::Print("Image Size: (%d x %d)\nMax Depth: %d\nTriangles: %d\nSpheres: %d\nDirectional Lights: %d\nPoint Lights: %d\n", renderData.dimensions.x, renderData.dimensions.y, renderData.maxDepth, renderData.scene.Triangles().size(), renderData.scene.Spheres().size(), renderData.scene.DirectionalLights().size(), renderData.scene.PointLights().size());
    renderData.accelGrid.Build(renderData);


    EDX::Log::Status("Rendering Image \"%s\" \n", renderData.outputName.c_str());
    EDX::ProgressBar pb;

    EDX::Image img(renderData.dimensions.x, renderData.dimensions.y);

    //Render the Image
    std::atomic<uint32_t> pixelsProcessed(0);
    uint32_t totalPixels = img.Size();

    auto render = [&](EDX::Maths::Vector2<int> dim_x, EDX::Maths::Vector2<int> dim_y)
    {
        for (uint32_t y = dim_y.x; y < dim_y.y; y++) {
            for (uint32_t x = dim_x.x; x < dim_x.y; x++) {

                const EDX::Colour pixelColour = EDX::RayTracer::RenderPixel(x, y, renderData);

                img.SetPixel(x, y, pixelColour);
                pixelsProcessed++;
            }

            //Only update the progress bar in the outer part of the loop, as it's SLOW. 
            const float p = (float)(pixelsProcessed) / (float)(totalPixels);
            pb.Update(p);
        }
    };


    //Split the image into Blocks to process. 
    const EDX::Maths::Vector2i blockDim = { 64u, 64u };

    EDX::TS_Stack<EDX::Maths::Vector4i> imageBlocks;

    const uint32_t blocks_y = (renderData.dimensions.y / blockDim.y);
    const uint32_t remainder_x = renderData.dimensions.x % blockDim.x;

    const uint32_t blocks_x = (renderData.dimensions.x / blockDim.x);
    const uint32_t remainder_y = renderData.dimensions.y % blockDim.y;

    for (uint32_t y = 0; y < blocks_y + 1; y++) {
        for (uint32_t x = 0; x < blocks_x + 1; x++) {
            //Compute each block size
            const int x_min = EDX::Maths::Clamp((int)blockDim.x * (int)x, 0, (int)renderData.dimensions.x);
            const int x_max = EDX::Maths::Clamp(x_min + blockDim.x, 0, (int)renderData.dimensions.x);
            if (x_min == x_max) {
                continue;
            }

            const int y_min = EDX::Maths::Clamp((int)blockDim.y * (int)y, 0, (int)renderData.dimensions.y);
            const int y_max = EDX::Maths::Clamp(y_min + blockDim.y, 0, (int)renderData.dimensions.y);

            if (y_min == y_max) {
                continue;
            }
            EDX::Maths::Vector4i block = {  //TODO: convert Vec4i to Vec4<uint32_t>
                x_min, x_max,   //xmin, xmax
                y_min, y_max    //ymin, ymax
            };


            imageBlocks.Wait_And_Push(block);
        }
    }

    EDX::Log::Print("Num Blocks: %d\nBlock Dimensions: %d x %d\n", imageBlocks.Size(), blockDim.x, blockDim.y);

    //Kick off worker threads, each rendering sections of the image. 
    const uint32_t num_threads = NUM_THREADS;
    std::vector<std::thread> threads(num_threads - 1);  //Account for the main thread + (NUM_THREADS - 1) workers.

    EDX::Log::Print("Processing on %d Threads.\n", num_threads);

    for (int i = 1; i < num_threads; i++) {
        threads[i - 1] = std::thread([&](int idx) {
            while (imageBlocks.Empty() == false) {
                //Process a block of the image. 
                const EDX::Maths::Vector4i block = imageBlocks.Wait_And_Pop();
                render({ block.x, block.y }, { block.z, block.w });
            }

            }, i);
    }

    //Have the main thread render too. 
    while (imageBlocks.Empty() == false) {
        //Process a block of the image. 
        const EDX::Maths::Vector4i block = imageBlocks.Wait_And_Pop();
        render({ block.x, block.y }, { block.z, block.w });
    }


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

    //Report how long it took to render to the console. 
    const double render_time_s = pb.GetProgressTimer().Duration();
    EDX::Log::Success("\nRender Complete in %.8fs.\nProcessed %d / %d pixels\n", render_time_s, pixelsProcessed.load(), img.Size());

    ExportImage(img, renderData.outputName);

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
