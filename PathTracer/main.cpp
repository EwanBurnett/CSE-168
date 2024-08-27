#include "Utils/Logger.h"
#include "Maths.h"
#include "Utils/ProgressBar.h"
#include "Image.h"
#include "Ray.h"
#include "Camera.h"

constexpr uint16_t WIDTH = 2160; 
constexpr uint16_t HEIGHT = 1440; 

int main() {
    EDX::Log::Status("EDX UC San Diego CS-168 Rendering 2 Coursework\nEwan Burnett - 2024\n");

    //Resource Allocation
    EDX::Image img(WIDTH, HEIGHT); 
   
    EDX::Maths::Vector3f lookFrom = { 0.0f, 0.0f, 0.0f };
    EDX::Maths::Vector3f lookAt = { 0.0f, 0.0f, 1000.0f };
    EDX::Maths::Vector3f up = { 0.0f, 1.0f, 0.0f };

    EDX::Camera camera(lookFrom, lookAt, up, EDX::Maths::DegToRad(100.0)); 

    //Render
    {
        EDX::Log::Status("Rendering...\n");
        EDX::ProgressBar pb;
        
        for (uint32_t y = 0; y < HEIGHT; y++) {
            for (uint32_t x = 0; x < WIDTH; x++) {
                EDX::Ray r; //TODO: Trace Rays into a Scene...

                //for now, Simulate rendering with UVs. 
                //NOTE: By convention, coord [0, 0] is the top-left corner of the image!! 
                EDX::Colour c = { (float)(x) / (float)(WIDTH), (float)(y) / (float)(HEIGHT), 0.0f, 1.0f }; 
                img.SetPixel(x, y, c); 
            }

            //Only update the progress bar in the outer part of the loop, as it's SLOW. 
            float p = (float)(y * WIDTH ) / (float)(WIDTH * HEIGHT);
            pb.Update(p);
        }

        const double render_time_s = pb.GetProgressTimer().Duration();
        EDX::Log::Success("\nRender Complete in %.8fs.\n", render_time_s); 
    }

    EDX::Log::Status("Exporting Render to PNG...\n");
    img.ExportToPNG("Test.png"); 
    EDX::Log::Status("Export Complete.\n");
}