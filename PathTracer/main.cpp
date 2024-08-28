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
    EDX::Maths::Vector3f lookAt = { 0.0f, 0.0f, 10.0f };
    EDX::Maths::Vector3f up = { 0.0f, 1.0f, 0.0f };

    EDX::Camera camera(lookFrom, lookAt, up, EDX::Maths::DegToRad(60.0));

    //Render
    {
        EDX::Log::Status("Rendering...\n");
        EDX::ProgressBar pb;

        //Shoot rays through the center of each pixel, adding 0.5 offset to x and y with respect to the image aspect ratio. 
        const float aspectRatio = (float)WIDTH / (float)HEIGHT;

        //Compute the top-leftmost pixel coordinates
        const float tlX = -static_cast<float>(WIDTH) / 2.0f;
        const float tlY = -static_cast<float>(HEIGHT) / 2.0f;

        const float FoVX = (2.0f * atan(tan(camera.GetFoVRadians() * 0.5f) * aspectRatio)); //Compute Horizontal FoV wrt Vertical FoV and Aspect Ratio
        const float FoVY = camera.GetFoVRadians();

        //Render the Image
        for (uint32_t y = 0; y < HEIGHT; y++) {
            for (uint32_t x = 0; x < WIDTH; x++) {
                EDX::Colour clr = {};

                const float alpha = tan(FoVX / 2.0f) * ((x - ((float)WIDTH / 2.0f)) / (float)WIDTH / 2.0f);
                const float beta = tan(FoVY / 2.0f) * ((((float)HEIGHT / 2.0f) - y) / (float)HEIGHT / 2.0f);

                EDX::Maths::Vector3f rayDirection;
                rayDirection = (alpha * camera.GetRightVector()) + (beta * camera.GetUpVector()) + camera.GetForwardsVector();
                rayDirection = rayDirection.Normalize();


                EDX::Ray r(camera.GetPosition(), rayDirection);

                //Intersect with a debug sphere. 
                //Solve the quadratic to compute intersection
                //For now, just shade it as a solid colour. 
                {
                    const EDX::Maths::Vector3f sphere_pos = { 0.0, 0.0, 10.0 };
                    const float sphere_radius = 1.0f;

                    EDX::Maths::Vector3f toCenter = sphere_pos - r.Origin();
                    const float a = EDX::Maths::Vector3f::Dot(r.Direction(), r.Direction());
                    const float b = -2.0f * EDX::Maths::Vector3f::Dot(r.Direction(), toCenter);
                    const float c = EDX::Maths::Vector3f::Dot(toCenter, toCenter) - (sphere_radius * sphere_radius);

                    const float discriminant = b * b - 4 * a * c;
                    if (discriminant >= 0.0) {
                        clr = { 1.0f, 0.0f, 0.0f, 1.0f };
                    }
                }

                //Clamp the pixel colour to [0, 1]
                clr.r = EDX::Maths::Clamp(clr.r, 0.0f, 1.0f);
                clr.g = EDX::Maths::Clamp(clr.g, 0.0f, 1.0f);
                clr.b = EDX::Maths::Clamp(clr.b, 0.0f, 1.0f);
                clr.a = EDX::Maths::Clamp(clr.a, 0.0f, 1.0f);

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
        img.ExportToPNG(buffer);
    }
    EDX::Log::Status("Export Complete.\n");
}