#include "Utils/Logger.h"
#include "Maths.h"
#include "Utils/ProgressBar.h"
#include "Image.h"
#include "Ray.h"
#include "Camera.h"
#include "Scene.h"
#include <chrono> 
#include <filesystem>

constexpr uint16_t WIDTH = 1920;
constexpr uint16_t HEIGHT = 1080;
const char* OUTPUT_NAME = "Test";
const char* OUTPUT_DIRECTORY = "Output";

int main() {
    EDX::Log::Status("EDX UC San Diego CS-168 Rendering 2 Coursework\nEwan Burnett - 2024\n");

    //Resource Allocation
    EDX::Image img(WIDTH, HEIGHT);

    EDX::Maths::Vector3f lookFrom = { 0.0f,0.0f, 0.0f };
    EDX::Maths::Vector3f lookAt = { 0.0f, 0.0f, 10.0f };
    EDX::Maths::Vector3f up = { 0.0f, 1.0f, 0.0f };

    EDX::Scene scene;
   
    scene.Planes().push_back({
       {0.0f, 0.0f,-1.0f},{0.0f, 0.0f,100.0f}
        });
    scene.Triangles().push_back({
        { -2.0f,0.0f, 17.0f },{ 1.0f, -12.0f, 10.0f },  { 0.0f, 1.0f, 17.0f }
        });
    scene.Spheres().push_back({ {12.6f, 1.0f, 60.0f}, 1.4f });
    scene.Spheres().push_back({{0.0f, 0.0f, 10.0f}, 1.0f});

    scene.DirectionalLights().push_back({ { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f,1.0f } });
    scene.DirectionalLights().push_back({ { 0.0f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } });

    EDX::Camera camera(lookFrom, lookAt, up, EDX::Maths::DegToRad(60.0));
    {
        EDX::Log::Status("Rendering...\n");
        EDX::ProgressBar pb;

        //Shoot rays through the center of each pixel, adding 0.5 offset to x and y with respect to the image aspect ratio. 
        const float aspectRatio = (float)WIDTH / (float)HEIGHT;

        const float FoVX = (2.0f * atan(tan(camera.GetFoVRadians() * 0.5f) * aspectRatio)); //Compute Horizontal FoV wrt Vertical FoV and Aspect Ratio
        const float FoVY = camera.GetFoVRadians();

        //Render the Image
        for (uint32_t y = 0; y < HEIGHT; y++) {
            for (uint32_t x = 0; x < WIDTH; x++) {
                EDX::Colour clr = {};

                const float alpha = tan(FoVX / 2.0f) * ((((float)WIDTH / 2.0f) - x) / (float)WIDTH / 2.0f);
                const float beta = tan(FoVY / 2.0f) * ((y - ((float)HEIGHT / 2.0f)) / (float)HEIGHT / 2.0f);

                EDX::Maths::Vector3f rayDirection;
                rayDirection = (alpha * camera.GetRightVector()) + (beta * camera.GetUpVector()) - camera.GetForwardsVector();
                rayDirection = rayDirection.Normalize();


                EDX::Ray r(camera.GetPosition(), rayDirection);

                EDX::RayHit result = {}; 
                //Test Intersection in the scene
                if(scene.TraceRay(r, result))
                {
                    const EDX::Colour k_Ambient = { 1.0f, 0.0f, 0.0f, 0.0f };
                    clr = clr + k_Ambient;
                    for (auto& light : scene.DirectionalLights()) {
                        //Apply constant shading
                        const EDX::Maths::Vector3f lightDir = light.GetDirection().Normalize();

                        const float n_dot_l = result.normal.Dot(lightDir);

                        if (n_dot_l > 0.0f) {
                            const EDX::Colour& k_Light = light.GetColour();

                            clr = clr + (k_Light * n_dot_l * k_Light.a);
                        }
                    }
                    //clr = { result.normal.x, result.normal.y,  result.normal.z, 1.0f };
                    //clr = { result.point.x, result.point.y,  result.point.z, 1.0f };
                }

                //Clamp the pixel colour to [0, 1]
                clr.r = EDX::Maths::Clamp(clr.r, 0.0f, 1.0f);
                clr.g = EDX::Maths::Clamp(clr.g, 0.0f, 1.0f);
                clr.b = EDX::Maths::Clamp(clr.b, 0.0f, 1.0f);
                clr.a = 1.0f; // EDX::Maths::Clamp(clr.a, 0.0f, 1.0f);

                img.SetPixel(x, y, clr);
            }

            //Only update the progress bar in the outer part of the loop, as it's SLOW. 
            float p = (float)(y * WIDTH) / (float)(WIDTH * HEIGHT);
            pb.Update(p);
        }

        const double render_time_s = pb.GetProgressTimer().Duration();
        EDX::Log::Success("\nRender Complete in %.8fs.\n", render_time_s);
    }

    EDX::Log::Status("Exporting Render to PNG...\n");
    {
        std::filesystem::create_directory(OUTPUT_DIRECTORY);

        //Format the filename in relation to the output path. 
        char buffer[0xff];
        strcpy(buffer, OUTPUT_DIRECTORY);
        strcat(buffer, "/");
        strcat(buffer, OUTPUT_NAME);

        //Format a timestamp for this render
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        size_t len = strlen(buffer);
        strftime(buffer + len, 0xff - len, "_%Y-%m-%d_%H-%M-%S.png", &tm);

        EDX::Log::Status("Output Path: %s/%s\n", std::filesystem::current_path().generic_string().c_str(), buffer);
        img.ExportToPNG(buffer, 2.2);
    }
    EDX::Log::Status("Export Complete.\n");
}