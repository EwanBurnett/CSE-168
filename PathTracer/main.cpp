#include "Utils/Logger.h"
#include "Maths.h"
#include "Utils/ProgressBar.h"
#include "Image.h"
#include "Ray.h"
#include "Camera.h"
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

    EDX::Maths::Vector3f lookFrom = { 0.0f, 0.0f, 0.0f };
    EDX::Maths::Vector3f lookAt = { 0.0f, 0.0f, 1.0f };
    EDX::Maths::Vector3f up = { 0.0f, 1.0f, 0.0f };

    EDX::Camera camera(lookFrom, lookAt, up, EDX::Maths::DegToRad(90.0));

    //Render
    {
        EDX::Log::Status("Rendering...\n");
        EDX::ProgressBar pb;

        //Shoot rays through the center of each pixel, adding 0.5 offset to x and y with respect to the image aspect ratio. 
        const float aspectRatio = (float)WIDTH / (float)HEIGHT;
        const EDX::Maths::Vector2f pixelDelta = EDX::Maths::Vector2f{ 1.0f, 1.0f };    //UV Pixel Delta = {AR / 2, 0.5}

        //Compute the top-leftmost pixel coordinates
        const float tlX = -static_cast<float>(WIDTH) / 2.0f;
        const float tlY = -static_cast<float>(HEIGHT) / 2.0f;

        EDX::Maths::Matrix4x4<float> view = camera.GetViewMatrix();

        //Render the Image
        for (uint32_t y = 0; y < HEIGHT; y++) {
            for (uint32_t x = 0; x < WIDTH; x++) {
                EDX::Maths::Vector4f rayDirection = {
                    tlX + (pixelDelta.x * x),  
                    tlY + (pixelDelta.y * y),
                    0.0f,
                    0.0f
                };

                //Orient the ray to the camera's view.
                rayDirection = rayDirection.Normalize();
                rayDirection = rayDirection * view;

                auto dir = camera.GetForwardsVector() + EDX::Maths::Vector3f{ rayDirection.x, rayDirection.y, rayDirection.z };
                dir = dir.Normalize();

                EDX::Ray r(camera.GetPosition(), dir); //TODO: Trace Rays into a Scene...

                auto f = r.At(600.0f);
                EDX::Colour c = {};

                //for now, Simulate rendering with UVs. 
                //NOTE: By convention, coord [0, 0] is the top-left corner of the image!! 
                //c = { (float)(x) / (float)(WIDTH), (float)(y) / (float)(HEIGHT), 0.0f, 1.0f }; // UV Output

                //Output ray direction, for debugging
                //c = { r.Direction().x, r.Direction().y, r.Direction().z, 1.0f};
                c = { r.Direction().x, r.Direction().y, 0.0f, 1.0f };
                //c = { r.Direction().x, 0.0f, 0.0f, 1.0f};
                //c = { 0.0f, r.Direction().y, 0.0f, 1.0f};
                //c = { 0.0f, 0.0f, r.Direction().z, 1.0f};

                img.SetPixel(x, y, c);
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
        img.ExportToPNG(buffer);
    }
    EDX::Log::Status("Export Complete.\n");
}