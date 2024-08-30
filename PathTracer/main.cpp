#include "Utils/Logger.h"
#include "Maths.h"
#include "Utils/ProgressBar.h"
#include "Image.h"
#include "Ray.h"
#include "Camera.h"
#include "Scene.h"
#include <chrono> 
#include <filesystem>
#include <fstream>

constexpr uint16_t WIDTH = 1920;
constexpr uint16_t HEIGHT = 1080;
const char* OUTPUT_NAME = "Test";

const char* OUTPUT_DIRECTORY = "Output";
const char* SCENE_PATH = "Scenes/TestScenes/scene2.test";

#define ENABLE_DEBUG_SCENE 1

namespace EDX {
    struct RenderData {
        std::string outputName;
        Maths::Vector2<uint16_t> dimensions;
        Camera camera;
        Scene scene;
    };
}

bool LoadSceneFile(const char* filePath, EDX::RenderData& renderData) {
    EDX::Log::Status("Loading Scene \"%s\".\n", filePath);

    if (!std::filesystem::exists(filePath)) {
        EDX::Log::Failure("Failed to load Scene \"%s\": File does not Exist!\n", filePath);
        return false;
    }
    const uintmax_t fileSize = std::filesystem::file_size(filePath);
    if (fileSize == 0) {
        EDX::Log::Failure("Failed to load Scene \"%s\": File size was Invalid!\n", filePath);
        return false;
    }

    std::ifstream inScene;
    inScene.open(filePath);
    if (!inScene.is_open()) {
        EDX::Log::Failure("Failed to load Scene \"%s\": Unable to open Scene File!\n", filePath);
        return false;
    }

    std::vector<EDX::Maths::Vector3f> vertices;
    uint64_t currentVtx = 0;

    //Parse each line from the file. 
    for (std::string line; std::getline(inScene, line);) {
        //Ignore blank lines
        if (line.size() == 0) {
            continue;
        }
        //Ignore Comments
        if (line.at(0) == '#') {
            continue;
        }

        EDX::Log::Debug("%s\n", line.c_str());

        //Split the line into tokens, delimited by a space ' '.
        std::vector<std::string> tokens;
        {
            uint64_t offset = 0;
            while (offset != std::string::npos) {
                offset = line.find(' ');
                auto token = line.substr(0, offset);
                if (!token.empty()) {
                    tokens.push_back(token);
                }
                line.erase(0, offset + 1);
            }
        }

        //Parse Each Command
        std::string command = tokens[0];
        {
            //The 'size' command specifies a render's size:
            //size [x] [y]
            if (command == "size") {
                renderData.dimensions.x = static_cast<uint16_t>(std::stoi(tokens[1]));
                renderData.dimensions.y = static_cast<uint16_t>(std::stoi(tokens[2]));
            }
            //The 'camera' command defines a camera
            //camera [lookFrom xyz] [lookAt xyz] [up xyz] [FoVDegrees]
            else if (command == "camera") {
                EDX::Maths::Vector3f lookFrom = {};
                {
                    lookFrom.x = std::stof(tokens[1]);
                    lookFrom.y = std::stof(tokens[2]);
                    lookFrom.z = std::stof(tokens[3]);
                }

                EDX::Maths::Vector3f lookAt = {};
                {
                    lookAt.x = std::stof(tokens[4]);
                    lookAt.y = std::stof(tokens[5]);
                    lookAt.z = std::stof(tokens[6]);
                }

                EDX::Maths::Vector3f up = {};
                {
                    up.x = std::stof(tokens[7]);
                    up.y = std::stof(tokens[8]);
                    up.z = std::stof(tokens[9]);
                }

                float fovDegrees = std::stof(tokens[10]);

                renderData.camera = EDX::Camera(lookFrom, lookAt, up, EDX::Maths::DegToRad(fovDegrees));
            }
            //The 'output' command specifies an output name.
            //Extensions are stripped, and a timestamp is added during export.
            //output [file name]
            else if (command == "output") {
                renderData.outputName = tokens[1];
            }
            //The 'sphere' command defines a sphere
            //sphere [position xyz] [radius]
            else if (command == "sphere") {
                EDX::Maths::Vector3f position = {};
                {
                    position.x = std::stof(tokens[1]);
                    position.y = std::stof(tokens[2]);
                    position.z = std::stof(tokens[3]);
                }

                float radius = std::stof(tokens[4]);
                renderData.scene.Spheres().push_back({ position, radius });
            }
            //The 'maxverts' command specifies the maximum number of vertices in this scene. 
            //Used for array sizing. 
            //maxverts [count] 
            else if (command == "maxverts") {
                const uint64_t max_verts = std::stoi(tokens[1]);
                vertices.resize(max_verts);
            }
            //The 'vertex' command defines a vertex in the scene. 
            //Indexed by the 'tri' command to construct triangles. 
            //vertex [x] [y] [z]
            else if (command == "vertex") {
                EDX::Maths::Vector3f vtx = {};
                {
                    vtx.x = std::stof(tokens[1]);
                    vtx.y = std::stof(tokens[2]);
                    vtx.z = std::stof(tokens[3]);
                }

                vertices[currentVtx] = vtx;
                currentVtx++;
            }
            //The 'tri' command defines a triangle geometry.
            //Defined by 3 vertices, a, b and c - indexed into the vertices array. 
            else if (command == "tri") {
                uint32_t a = std::stoi(tokens[1]);
                uint32_t b = std::stoi(tokens[3]);  //Swap to enforce handedness!
                uint32_t c = std::stoi(tokens[2]);

                renderData.scene.Triangles().push_back({ vertices[a], vertices[b], vertices[c] });
            }
            else if (command == "directional") {
                EDX::Maths::Vector3f direction = {};
                {
                    direction.x = std::stof(tokens[1]);
                    direction.y = std::stof(tokens[2]);
                    direction.z = std::stof(tokens[3]);
                }

                EDX::Colour colour = {};
                {
                    colour.r = std::stof(tokens[4]);
                    colour.g = std::stof(tokens[5]);
                    colour.b = std::stof(tokens[6]);
                    colour.a = 1.0f;
                }

                renderData.scene.DirectionalLights().push_back({ direction, colour });
            }
            else {
                EDX::Log::Warning("Unknown Command \"%s\".\n", command.c_str());
            }
        }
    }

    return true;
}

int main() {
    EDX::Log::Status("EDX UC San Diego CS-168 Rendering 2 Coursework\nEwan Burnett - 2024\n");

    //Resource Allocation
    EDX::RenderData renderData;
    if (!LoadSceneFile(SCENE_PATH, renderData))
#if ENABLE_DEBUG_SCENE
    {   //If we can't load a scene, load the debug scene instead. 
        //Context
        {
            renderData.outputName = OUTPUT_NAME;
            renderData.dimensions.x = WIDTH;
            renderData.dimensions.y = HEIGHT;
        }
        //Camera
        {
            EDX::Maths::Vector3f lookFrom = { 0.0f,0.0f, 0.0f };
            EDX::Maths::Vector3f lookAt = { 0.0f, 0.0f, 10.0f };
            EDX::Maths::Vector3f up = { 0.0f, 1.0f, 0.0f };
            renderData.camera = EDX::Camera(lookFrom, lookAt, up, EDX::Maths::DegToRad(60.0));
        }
        //Scene
        {
            renderData.scene.Planes().push_back({
               {0.0f, 0.0f,-1.0f},{0.0f, 0.0f,100.0f}
                });
            renderData.scene.Triangles().push_back({
                { -2.0f,0.0f, 17.0f },{ 1.0f, -12.0f, 10.0f },  { 0.0f, 1.0f, 17.0f }
                });
            renderData.scene.Spheres().push_back({
                {12.6f, 1.0f, 60.0f}, 1.4f
                });
            renderData.scene.Spheres().push_back({
                {0.0f, 0.0f, 10.0f}, 1.0f
                });
            renderData.scene.DirectionalLights().push_back({
                { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f,1.0f }
                });
            renderData.scene.DirectionalLights().push_back({
                { 0.0f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f }
                });
        }
    }
#else 
    {
        EDX::Log::Failure("Failed to load scene!\n");
        return 1;
    }
#endif

    EDX::Image img(renderData.dimensions.x, renderData.dimensions.y);
    {
        EDX::Log::Status("Rendering...\n");
        EDX::ProgressBar pb;

        //Shoot rays through the center of each pixel, adding 0.5 offset to x and y with respect to the image aspect ratio. 
        const float aspectRatio = (float)renderData.dimensions.x / (float)renderData.dimensions.y;

        const float FoVX = (2.0f * atan(tan(renderData.camera.GetFoVRadians() * 0.5f) * aspectRatio)); //Compute Horizontal FoV wrt Vertical FoV and Aspect Ratio
        const float FoVY = renderData.camera.GetFoVRadians();

        //Render the Image
        for (uint32_t y = 0; y < renderData.dimensions.y; y++) {
            for (uint32_t x = 0; x < renderData.dimensions.x; x++) {
                EDX::Colour clr = {};

                const float alpha = tan(FoVX / 2.0f) * ((((float)renderData.dimensions.x / 2.0f) - x) / (float)renderData.dimensions.x / 2.0f);
                const float beta = tan(FoVY / 2.0f) * ((y - ((float)renderData.dimensions.y / 2.0f)) / (float)renderData.dimensions.y / 2.0f);

                EDX::Maths::Vector3f rayDirection;
                rayDirection = (alpha * renderData.camera.GetRightVector()) + (beta * renderData.camera.GetUpVector()) - renderData.camera.GetForwardsVector();
                rayDirection = rayDirection.Normalize();


                EDX::Ray r(renderData.camera.GetPosition(), rayDirection);

                EDX::RayHit result = {};
                //Test Intersection in the scene
                if (renderData.scene.TraceRay(r, result))
                {
                    const EDX::Colour k_Ambient = { 0.50f, 0.50f, 0.50f, 0.0f };
                    clr = clr + k_Ambient;
                    for (auto& light : renderData.scene.DirectionalLights()) {
                        //Apply constant shading
                        const EDX::Maths::Vector3f lightDir = light.GetDirection().Normalize();

                        const float n_dot_l = result.normal.Dot(lightDir);

                        if (n_dot_l > 0.0f) {
                            const EDX::Colour& k_Light = light.GetColour();

                            clr = clr + (k_Light * n_dot_l * k_Light.a);
                        }
                    }
                    clr = { result.normal.x, result.normal.y,  result.normal.z, 1.0f };
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
            float p = (float)(y * renderData.dimensions.x) / (float)(renderData.dimensions.x * renderData.dimensions.y);
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
        if (!renderData.outputName.empty()) {
            strcat(buffer, renderData.outputName.c_str());
            strcat(buffer, "_");
        }

        //Format a timestamp for this render
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        size_t len = strlen(buffer);
        strftime(buffer + len, 0xff - len, "%Y-%m-%d_%H-%M-%S.png", &tm);

        EDX::Log::Status("Output Path: %s/%s\n", std::filesystem::current_path().generic_string().c_str(), buffer);
        img.ExportToPNG(buffer, 2.2);
    }
    EDX::Log::Status("Export Complete.\n");

    return 0;
}