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
#include <math.h>
#include <thread>

constexpr uint16_t WIDTH = 1920;
constexpr uint16_t HEIGHT = 1080;
const char* OUTPUT_NAME = "Test";

const char* OUTPUT_DIRECTORY = "Output";
const char* SCENE_PATH = "";// "Scenes/HW3/scene7.test";

#define ENABLE_DEBUG_SCENE 1

namespace EDX {
    struct RenderData {
        std::string outputName;
        Maths::Vector2<uint16_t> dimensions;
        Maths::Vector2<float> FoV;
        Camera camera;
        Scene scene;
    };
}

bool LoadSceneFile(const char* filePath, EDX::RenderData& renderData);
EDX::Maths::Vector3f OrientRay(const uint32_t x, const uint32_t y, const EDX::RenderData& renderData);
EDX::Colour RenderPixel(const uint32_t x, const uint32_t y, EDX::RenderData& renderData);
void ExportImage(const EDX::Image& img, const std::string& outputName, const float gamma = 1.0f);

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
            EDX::BlinnPhong mat = {};
            mat.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
            mat.emission = { 0.0f, 0.0f, 0.0f, 1.0f };
            mat.diffuse = { 0.4f, 0.0f, 0.0f, 1.0f };
            mat.specular = { 1.0f, 1.0f, 1.0f, 1.0f };
            mat.shininess = 80.0f;

            /*
            */
            renderData.scene.Planes().push_back({
               {0.0f, 1.0f, 0.0f},{0.0f, -100.0f, 0.0f}
                });
            renderData.scene.Triangles().push_back({
                {-1.0f, -1.0f, 10.0f}, {-1.0f, 1.0f, 10.0f}, {1.0f, -1.0f, 10.0f}
                });
            /*
            renderData.scene.Spheres().push_back({
                {12.6f, 1.0f, 60.0f}, 1.4f
                });
            */
            renderData.scene.Spheres().push_back({
                {0.0f, 0.0f, 5.0f}, 1.0f
                });
            renderData.scene.Triangles().push_back({
                {4.0f, -1.0f, 10.0f}, {-1.0f, 1.0f, 10.0f}, {1.0f, -1.0f, 10.0f}
                });

            renderData.scene.Spheres().push_back({
                {10.0f, 0.0f, 60.0f}, 8.9f
                });

            renderData.scene.DirectionalLights().push_back({
                { -1.0f, 0.6f, -1.0f }, { 1.0f, 1.0f, 1.0f,1.0f }
                });
            renderData.scene.DirectionalLights().push_back({
                { 0.15f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f }
                });

            for (auto& s : renderData.scene.Spheres()) {
                s.SetMaterial(mat);
            }
        }
    }
#else 
    {
        EDX::Log::Failure("Failed to load scene!\n");
        return 1;
    }
#endif

    EDX::Log::Status("Rendering Image \"%s\" (%d x %d)\n", renderData.outputName.c_str(), renderData.dimensions.x, renderData.dimensions.y);
    EDX::ProgressBar pb;

    EDX::Image img(renderData.dimensions.x, renderData.dimensions.y);

    //Compute Horizontal FoV wrt Vertical FoV and Aspect Ratio
    {
        const float aspectRatio = (float)renderData.dimensions.x / (float)renderData.dimensions.y;

        renderData.FoV.x = (2.0f * atan(tan(renderData.camera.GetFoVRadians() * 0.5f) * aspectRatio));
        renderData.FoV.y = renderData.camera.GetFoVRadians();
    }

    //Render the Image
    for (uint32_t y = 0; y < renderData.dimensions.y; y++) {
        for (uint32_t x = 0; x < renderData.dimensions.x; x++) {

            const EDX::Colour pixelColour = RenderPixel(x, y, renderData);

            img.SetPixel(x, y, pixelColour);
        }

        //Only update the progress bar in the outer part of the loop, as it's SLOW. 
        float p = (float)(y * renderData.dimensions.x) / (float)(renderData.dimensions.x * renderData.dimensions.y);
        pb.Update(p);
    }

    const double render_time_s = pb.GetProgressTimer().Duration();
    EDX::Log::Success("\nRender Complete in %.8fs.\n", render_time_s);

    ExportImage(img, renderData.outputName);

    return 0;
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

    EDX::BlinnPhong material = {};
    material.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };


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
                EDX::Sphere s = { position, radius };
                s.SetMaterial(material);
                renderData.scene.Spheres().push_back(s);
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
            //tri [a] [b] [c]
            else if (command == "tri") {
                uint32_t a = std::stoi(tokens[1]);
                uint32_t b = std::stoi(tokens[2]);
                uint32_t c = std::stoi(tokens[3]);

                EDX::Triangle t = { vertices[a], vertices[b], vertices[c] };
                t.SetMaterial(material);

                renderData.scene.Triangles().push_back(t);
            }
            //The 'directional' command defines a Directional light
            //Defined by a Direction and a colour. 
            //direction [dir xyz] [colour rgb]
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
            else if (command == "diffuse") {
                EDX::Colour d = {};
                {
                    d.r = std::stof(tokens[1]);
                    d.g = std::stof(tokens[2]);
                    d.b = std::stof(tokens[3]);
                    d.a = 1.0f;
                }
                material.diffuse = d;
            }
            else if (command == "ambient") {
                EDX::Colour a = {};
                {
                    a.r = std::stof(tokens[1]);
                    a.g = std::stof(tokens[2]);
                    a.b = std::stof(tokens[3]);
                    a.a = 1.0f;
                }
                material.ambient = a;
            }
            else if (command == "emissive") {
                EDX::Colour e = {};
                {
                    e.r = std::stof(tokens[1]);
                    e.g = std::stof(tokens[2]);
                    e.b = std::stof(tokens[3]);
                    e.a = 1.0f;
                }
                material.emission = e;
            }
            else if (command == "specular") {
                EDX::Colour s = {};
                {
                    s.r = std::stof(tokens[1]);
                    s.g = std::stof(tokens[2]);
                    s.b = std::stof(tokens[3]);
                    s.a = 1.0f;
                }
                material.specular = s;
            }
            else if (command == "shininess") {
                material.shininess = std::stof(tokens[1]);
            }
            else {
                EDX::Log::Warning("Unknown Command \"%s\".\n", command.c_str());
            }
        }
    }

    return true;
}

EDX::Maths::Vector3f OrientRay(const uint32_t x, const uint32_t y, const EDX::RenderData& renderData)
{
    const float alpha = 4.0f * tan(renderData.FoV.x / 2.0f) * ((x - ((float)renderData.dimensions.x / 2.0f)) / (float)renderData.dimensions.x / 2.0f);
    const float beta = 4.0f * -tan(renderData.FoV.y / 2.0f) * ((y - ((float)renderData.dimensions.y / 2.0f)) / (float)renderData.dimensions.y / 2.0f);

    EDX::Maths::Vector3f dir = {};
    dir = (alpha * renderData.camera.GetRightVector()) + (beta * renderData.camera.GetUpVector()) + renderData.camera.GetForwardsVector();
    return dir.Normalize();
}

EDX::Colour RenderPixel(const uint32_t x, const uint32_t y, EDX::RenderData& renderData)
{
    EDX::Colour clr = { 0.0f, 0.0f, 0.0f, 1.0f };   //Output Pixel Colour - Black by default. 

    //Compute a Direction for this ray
    EDX::Maths::Vector3f rayDirection = OrientRay(x, y, renderData);

    EDX::Ray r(renderData.camera.GetPosition(), rayDirection);


    //Test Intersection in the scene
    EDX::RayHit result = {};
    if (renderData.scene.TraceRay(r, result))
    {
        if (result.pMat) {
            EDX::BlinnPhong m = *result.pMat;
            clr = clr + m.ambient;
            clr = clr + m.emission;

            for (auto& light : renderData.scene.DirectionalLights()) {
                //Apply constant shading
                const EDX::Maths::Vector3f lightDir = light.GetDirection().Normalize();

                const float n_dot_l = EDX::Maths::Vector3f::Dot(result.normal, lightDir);

                if (n_dot_l > 0.0f) {
                    const EDX::Colour& k_Light = light.GetColour();

                    const auto h = (lightDir + -rayDirection).Normalize();

                    const float n_dot_h = EDX::Maths::Vector3f::Dot(result.normal, h);
                    clr = clr + (k_Light * n_dot_l * k_Light.a) * (m.diffuse + (m.specular * std::pow(std::max(n_dot_h, 0.0f), m.shininess)));

                }
            }

        }
        else {
            const EDX::Colour k_Ambient = { 0.10f, 0.10f, 0.10f, 0.0f };
            clr = clr + k_Ambient;

            for (auto& light : renderData.scene.DirectionalLights()) {
                //Apply constant shading
                const EDX::Maths::Vector3f lightDir = light.GetDirection().Normalize();

                const float n_dot_l = EDX::Maths::Vector3f::Dot(result.normal, lightDir);

                if (n_dot_l > 0.0f) {
                    const EDX::Colour& k_Light = light.GetColour();

                    clr = clr + (k_Light * n_dot_l * k_Light.a);
                }
            }
        }
        //clr = EDX::Colour(result.normal.x + 1, result.normal.y + 1, result.normal.z + 1, 1.0f) * 0.5f;    //Uncomment to view Normals
    }

    //Clamp the pixel colour to [0, 1]
    clr.r = EDX::Maths::Clamp(clr.r, 0.0f, 1.0f);
    clr.g = EDX::Maths::Clamp(clr.g, 0.0f, 1.0f);
    clr.b = EDX::Maths::Clamp(clr.b, 0.0f, 1.0f);
    clr.a = 1.0f;   //Ignore any transparency artifacts. 

    return clr;
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
