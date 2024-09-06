#include "Utils/Logger.h"
#include "Maths.h"
#include "Utils/ProgressBar.h"
#include "Primitives/Box.h"
#include "Image.h"
#include "Ray.h"
#include "Camera.h"
#include "Scene.h"
#include <chrono> 
#include <filesystem>
#include <fstream>
#include <math.h>
#include <stack>
#include <thread>
#include <vector> 
#include "RenderData.h"

constexpr uint16_t WIDTH = 1920;
constexpr uint16_t HEIGHT = 1080;
const char* OUTPUT_NAME = "Test";

const char* OUTPUT_DIRECTORY = "Output";
const char* SCENE_PATH = "Scenes/HW3/scene7.test";

#define ENABLE_DEBUG_SCENE 1

EDX::Acceleration::Grid g_AccelGrid;

EDX::Colour RenderPixel(const uint32_t x, const uint32_t y, EDX::RenderData& renderData);
EDX::Maths::Vector3f OrientRay(const uint32_t x, const uint32_t y, const EDX::RenderData& renderData);
void ExportImage(const EDX::Image& img, const std::string& outputName, const float gamma = 1.0f);
bool LoadSceneFile(const char* filePath, EDX::RenderData& renderData);

int main() {
    EDX::Log::Status("EDX UC San Diego CS-168 Rendering 2 Coursework\nEwan Burnett - 2024\n");

    //Load the scene 
    EDX::RenderData renderData = {};

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
            EDX::Maths::Vector3f lookAt = { 0.0f, 0.0f, 1.0f };
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

            EDX::Maths::Matrix4x4<float> transform = (EDX::Maths::Matrix4x4<float>::Scaling({ 1.0f, 1.0f, 1.0f }) * EDX::Maths::Matrix4x4<float>::ZRotationFromDegrees(15.0f)) * EDX::Maths::Matrix4x4<float>::Translation({ 0.0f, 0.0f, 10.0f });

            renderData.scene.PointLights().push_back({
                { 0.0f, 0.5f, 1.0f }, {1.0f, 0.2f, 0.0f},  { 1.0f, 1.0f, 1.0f,1.0f }
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

            EDX::Sphere s = { {0.0f, 0.0f, 0.0f}, 1.0f };
            s.SetMaterial(mat);
            s.SetWorldMatrix(transform);
            renderData.scene.Spheres().push_back(s);

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

    g_AccelGrid.BuildAccelerationStructure(renderData);


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

    auto render = [&](EDX::Maths::Vector2<uint32_t> dim_x, EDX::Maths::Vector2<uint32_t> dim_y)
    {
        for (uint32_t y = dim_y.x; y < dim_y.y; y++) {
            for (uint32_t x = dim_x.x; x < dim_x.y; x++) {

                const EDX::Colour pixelColour = RenderPixel(x, y, renderData);

                img.SetPixel(x, y, pixelColour);
            }

            //Only update the progress bar in the outer part of the loop, as it's SLOW. 
            float p = (float)(y * renderData.dimensions.x) / (float)(renderData.dimensions.x * renderData.dimensions.y);
            pb.Update(p);
        }
    };

    const uint32_t num_threads = 1; 
    std::vector<std::thread> threads(num_threads - 1); 
    for (int i = 1; i < num_threads; i++) {
        threads[i - 1] = std::thread([&](int idx) {

            thread_local uint32_t startX = (renderData.dimensions.x / num_threads) * idx;
            thread_local uint32_t countX = (renderData.dimensions.x / num_threads);
            thread_local uint32_t startY = (renderData.dimensions.y / num_threads) * idx;
            thread_local uint32_t countY = (renderData.dimensions.y / num_threads);

            render({ startX, startX + countX }, { startY, startY + countY });

            }, i);


    }
    render(
        { 0, renderData.dimensions.x - ((renderData.dimensions.x / num_threads) * (num_threads - 1)) }, 
        { 0, renderData.dimensions.y - ((renderData.dimensions.y / num_threads) * (num_threads - 1)) }
    ); 

    for (auto& t : threads) {
        t.join(); 
    }
    const double render_time_s = pb.GetProgressTimer().Duration();
    EDX::Log::Success("\nRender Complete in %.8fs.\n", render_time_s);

    ExportImage(img, renderData.outputName);

    return 0;
}


EDX::Colour RenderPixel(const uint32_t x, const uint32_t y, EDX::RenderData& renderData)
{

    //Compute a Direction for this ray
    EDX::Maths::Vector3f rayDirection = OrientRay(x, y, renderData);

    EDX::Ray r(renderData.camera.GetPosition(), rayDirection);

    uint32_t currentDepth = 0;

    EDX::Colour clr = { 0.0f, 0.0f, 0.0f, 1.0f };   //Output Pixel Colour - Black by default. 

    {
        EDX::Box box({ 100.0f, 100.0f, 100.0f }, { 300.0f, 300.0f, 300.0f });
        EDX::RayHit boxHit = {};
        bool rayBox = box.Intersects(r, boxHit);

        if (!rayBox) {
            int a = 0;
        }
    }

    auto rayColour = [&](EDX::Ray ray, uint32_t depth) {

        if (currentDepth >= renderData.maxDepth) {
            return;
        }

        currentDepth++;

        //Test Intersection in the scene
        EDX::RayHit result = {};
        if (renderData.scene.TraceRay(r, result, g_AccelGrid))
        {
            //Apply shading based on the Material
            if (result.pMat) {
                const EDX::BlinnPhong m = *result.pMat;
                clr = clr + m.ambient;
                clr = clr + m.emission;

                for (auto& light : renderData.scene.DirectionalLights()) {
                    const EDX::Maths::Vector3f lightDir = light.GetDirection().Normalize();

                    EDX::RayHit shadowHit = {};
                    bool isVisible = !renderData.scene.TraceRay({ result.point + (lightDir * (1.0f / 1000.0f)), lightDir }, shadowHit, g_AccelGrid);  //Visible if Nothing is hit in the light's direction!

                    if (!isVisible) {
                        clr = { 0.0f, 0.0f, 0.0f, 1.0f };
                        continue;
                    }

                    const float n_dot_l = EDX::Maths::Vector3f::Dot(result.normal, lightDir);

                    if (n_dot_l > 0.0f) {
                        const EDX::Colour& k_Light = light.GetColour();

                        const auto h = (lightDir + -rayDirection).Normalize();

                        const float n_dot_h = EDX::Maths::Vector3f::Dot(result.normal, h);
                        clr = clr + (k_Light * n_dot_l * k_Light.a) * (m.diffuse + (m.specular * std::pow(std::max(n_dot_h, 0.0f), m.shininess)));

                    }
                }

                for (auto& light : renderData.scene.PointLights()) {
                    EDX::Maths::Vector3f lightDir = (light.GetPosition() - result.point);
                    const float dist = lightDir.Length();
                    lightDir = lightDir.Normalize();

                    EDX::RayHit shadowHit = {};
                    bool isVisible = !renderData.scene.TraceRay({ result.point + (lightDir * (1.0f / 1000.0f)), lightDir }, shadowHit, g_AccelGrid); //Visible if Nothing is hit in the light's direction until the light's position. 

                    if (!isVisible && shadowHit.t > 0.0f && shadowHit.t < dist) {
                        clr = { 0.0f, 0.0f, 0.0f, 1.0f };
                        continue;
                    }


                    const float n_dot_l = EDX::Maths::Vector3f::Dot(result.normal, lightDir);

                    if (n_dot_l > 0.0f) {
                        const EDX::Colour& k_Light = light.GetColour();

                        const auto& att = light.GetAttenuation();

                        float attenuation = att.x + (att.y * dist) + (att.z * dist * dist);
                        const auto h = (lightDir + rayDirection).Normalize();

                        const float n_dot_h = EDX::Maths::Vector3f::Dot(result.normal, h);
                        clr = clr + ((k_Light * n_dot_l * k_Light.a) * (m.diffuse + (m.specular * std::pow(std::max(n_dot_h, 0.0f), m.shininess))) / attenuation);

                    }
                }


            }
            else {
                const EDX::Colour k_Ambient = { 0.10f, 0.10f, 0.10f, 0.0f };
                clr = clr + k_Ambient;

                for (auto& light : renderData.scene.DirectionalLights()) {
                    const EDX::Maths::Vector3f lightDir = light.GetDirection().Normalize();

                    const float n_dot_l = EDX::Maths::Vector3f::Dot(result.normal, lightDir);

                    if (n_dot_l > 0.0f) {
                        const EDX::Colour& k_Light = light.GetColour();

                        clr = clr + (k_Light * n_dot_l * k_Light.a);
                    }
                }
                for (auto& light : renderData.scene.PointLights()) {
                    EDX::Maths::Vector3f lightDir = (light.GetPosition() - result.point);
                    const float dist = lightDir.Length();
                    lightDir = lightDir.Normalize();

                    const float n_dot_l = EDX::Maths::Vector3f::Dot(result.normal, lightDir);

                    if (n_dot_l > 0.0f) {
                        const EDX::Colour& k_Light = light.GetColour();

                        const auto& att = light.GetAttenuation();

                        float attenuation = att.x + (att.y * dist) + (att.z * dist * dist);
                        const auto h = (lightDir + rayDirection).Normalize();

                        const float n_dot_h = EDX::Maths::Vector3f::Dot(result.normal, h);
                        clr = clr + ((k_Light * n_dot_l * k_Light.a) / attenuation);

                    }
                }

            }
            //clr = EDX::Colour(result.normal.x + 1, result.normal.y + 1, result.normal.z + 1, 1.0f) * 0.5f;    //Uncomment to view Normals
        }
    };

    rayColour(r, currentDepth);

    //Clamp the pixel colour to [0, 1]
    clr.r = EDX::Maths::Clamp(clr.r, 0.0f, 1.0f);
    clr.g = EDX::Maths::Clamp(clr.g, 0.0f, 1.0f);
    clr.b = EDX::Maths::Clamp(clr.b, 0.0f, 1.0f);
    clr.a = 1.0f;   //Ignore any transparency artifacts. 

    return clr;
}

EDX::Maths::Vector3f OrientRay(const uint32_t x, const uint32_t y, const EDX::RenderData& renderData)
{
    const float alpha = 4.0f * tan(renderData.FoV.x / 2.0f) * ((x - ((float)renderData.dimensions.x / 2.0f)) / (float)renderData.dimensions.x / 2.0f);
    const float beta = 4.0f * -tan(renderData.FoV.y / 2.0f) * ((y - ((float)renderData.dimensions.y / 2.0f)) / (float)renderData.dimensions.y / 2.0f);

    EDX::Maths::Vector3f dir = {};
    dir = (alpha * renderData.camera.GetRightVector()) + (beta * renderData.camera.GetUpVector()) + renderData.camera.GetForwardsVector();
    return dir.Normalize();
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

    std::stack<EDX::Maths::Matrix4x4<float>> transformStack;
    std::vector<EDX::Maths::Matrix4x4<float>> currentTransforms;

    EDX::Maths::Vector3f attenuation = { 1.0f, 0.0f, 0.0f };

    auto currentTransform = [&]() {
        EDX::Maths::Matrix4x4<float> acc = {};
        for (int i = currentTransforms.size(); i > 0; i--) {
            acc = acc * currentTransforms[i - 1];
        }

        return acc;
    };


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
                s.SetWorldMatrix(currentTransform());
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
                t.SetWorldMatrix(currentTransform());
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
            else if (command == "emission") {
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
            else if (command == "pushTransform") {
                transformStack.push(currentTransform());
                currentTransforms.clear();
            }
            else if (command == "popTransform") {
                currentTransforms.clear();
                currentTransforms.push_back(transformStack.top());
                transformStack.pop();
            }

            else if (command == "translate") {
                EDX::Maths::Vector3f t = {
                    std::stof(tokens[1]),
                    std::stof(tokens[2]),
                    std::stof(tokens[3])
                };
                auto translation = EDX::Maths::Matrix4x4<float>::Translation(t);
                currentTransforms.push_back(translation);
            }
            else if (command == "rotate") {
                const EDX::Maths::Vector3f axis = {
                    std::stof(tokens[1]),
                    std::stof(tokens[2]),
                    std::stof(tokens[3])
                };

                const float angle = EDX::Maths::DegToRad(std::stof(tokens[4]));

                EDX::Maths::Quaternion q;
                q = q.FromAxisAngle(axis, angle);

                auto m = q.ToMatrix4x4();
                currentTransforms.push_back(m);
            }
            else if (command == "scale") {
                EDX::Maths::Vector3f s = {
                std::stof(tokens[1]),
                std::stof(tokens[2]),
                std::stof(tokens[3])
                };

                currentTransforms.push_back(EDX::Maths::Matrix4x4<float>::Scaling(s));
            }
            else if (command == "point") {
                EDX::Maths::Vector3f position = {};
                {
                    position.x = std::stof(tokens[1]);
                    position.y = std::stof(tokens[2]);
                    position.z = std::stof(tokens[3]);
                }

                EDX::Colour colour = {};
                {
                    colour.r = std::stof(tokens[4]);
                    colour.g = std::stof(tokens[5]);
                    colour.b = std::stof(tokens[6]);
                    colour.a = 1.0f;
                }

                renderData.scene.PointLights().push_back({ position, attenuation, colour });
            }
            else if (command == "attenuation") {
                attenuation.x = std::stof(tokens[1]);
                attenuation.y = std::stof(tokens[2]);
                attenuation.z = std::stof(tokens[3]);
            }
            else if (command == "maxdepth") {
                renderData.maxDepth = std::stof(tokens[1]);
            }
            else {
                EDX::Log::Warning("Unknown Command \"%s\".\n", command.c_str());
            }
        }
    }

    return true;
}