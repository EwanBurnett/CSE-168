#include "RayTracer.h"
#include "Maths.h"
#include "Utils/Logger.h"
#include "Utils/Timer.h"
#include <filesystem>
#include <fstream>
#include <stack>
#include <list>


constexpr bool g_ShowNormals = false;   //Displays surface normals. 
constexpr bool g_ShowShadows = false;   //Highlights shadows in Red.


EDX::Colour EDX::RayTracer::RenderPixel(const uint32_t x, const uint32_t y, EDX::RenderData& renderData)
{
    const Maths::Vector4i viewport = {
        0, renderData.dimensions.x,
        0, renderData.dimensions.y
    };

    Ray r = renderData.camera.GenRay(viewport, x, y);

    EDX::Colour clr = { 0.0f, 0.0f, 0.0f, 1.0f };   //Output Pixel Colour - Black by default. 

    clr = clr + RayColour(r, 0, renderData);

    //Clamp the pixel colour to [0, 1]
    clr.r = EDX::Maths::Clamp(clr.r, 0.0f, 1.0f);
    clr.g = EDX::Maths::Clamp(clr.g, 0.0f, 1.0f);
    clr.b = EDX::Maths::Clamp(clr.b, 0.0f, 1.0f);
    clr.a = 1.0f;   //Ignore any transparency artifacts. 

    return clr;
}


EDX::Colour EDX::RayTracer::RayColour(const EDX::Ray ray, uint32_t depth, EDX::RenderData& renderData) {

    //Bounce until Max Depth is reached
    if (depth > renderData.maxDepth) {
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }
    depth++;


    EDX::Colour c = { 0.0f, 0.0f, 0.0f, 0.0f };

    const float shadowBias = 0.0001f;
    const float reflectionBias = 0.0001f;

    auto computeVisibility = [&](const Maths::Vector3f point, const Maths::Vector3f normal, const Maths::Vector3f lightDirection, const float lightDistance) {

        bool isVisible = false;
        {
            EDX::RayHit shadowHit = {};
            //Add a small bias to prevent shadow acne. 
            bool shadowHitObject = renderData.scene.TraceRay({ point + (normal * shadowBias), lightDirection }, shadowHit, renderData.accelGrid); //Visible if Nothing is hit in the light's direction until the light's position. 

            //If we didn't hit anything in the light's direction, then the point is visible to the light. 
            if (!shadowHitObject) {
                isVisible = true;
            }
            else {
                //If we did hit something, check that the distance to the object is > the distance to the light. 
                if (shadowHit.t > 0.0f && (lightDistance + shadowBias) < shadowHit.t) {
                    isVisible = true;
                }
            }
        }
        return isVisible;
    };

    //Test Intersection in the scene
    EDX::RayHit result = {};
    if (renderData.scene.TraceRay(ray, result, renderData.accelGrid))
    {
        if constexpr (!g_ShowNormals) {
            //Apply shading based on the Material
            if (result.pMat) {
                const EDX::BlinnPhong m = *result.pMat;
                c = c + m.ambient;
                c = c + m.emission;

                for (auto& light : renderData.scene.DirectionalLights()) {
                    const EDX::Maths::Vector3f lightDir = light.GetDirection().Normalize();

                    EDX::RayHit shadowHit = {};
                    const bool isVisible = computeVisibility(result.point, result.normal, lightDir, Maths::Infinity);

                    //Shadow Debugging
                    if constexpr (g_ShowShadows) {
                        if (!isVisible) {
                            c = { 1.0f, 0.0f, 0.0f, 1.0f };
                            continue;
                        }
                    }

                    const float n_dot_l = EDX::Maths::Vector3f::Dot(result.normal, lightDir);


                    const Maths::Vector3f reflectDir = ray.Direction() - 2.0f * result.normal * (float)EDX::Vec3::Dot(ray.Direction(), result.normal);
                    if (n_dot_l > 0.0f) {

                        //If the ray is in shadow,don't reflect it. 
                        if (isVisible != false) {

                            const EDX::Colour& k_Light = light.GetColour();

                            auto toEye = (ray.Origin() - result.point);
                            toEye = toEye.Normalize();
                            const auto h = (lightDir + toEye).Normalize();

                            const float n_dot_h = EDX::Maths::Vector3f::Dot(result.normal, h);

                            c = c + (k_Light * k_Light.a) * ((m.diffuse * n_dot_l) + (m.specular * std::powf(std::max(n_dot_h, 0.0f), m.shininess)));


                            c = c + (RayColour({ result.point + (result.normal * reflectionBias) , reflectDir }, depth, renderData) * m.specular);
                        }
                    }
                }

                for (auto& light : renderData.scene.PointLights()) {
                    EDX::Maths::Vector3f lightDir = (light.GetPosition() - result.point);
                    const float dist = lightDir.LengthSquared();
                    lightDir = lightDir.Normalize();


                    const bool isVisible = computeVisibility(result.point, result.normal, lightDir, dist);

                    //Shadow Debugging
                    if constexpr (g_ShowShadows) {
                        if (!isVisible) {
                            c = { 1.0f, 0.0f, 0.0f, 1.0f };
                            continue;
                        }
                    }


                    const float n_dot_l = EDX::Maths::Vector3f::Dot(result.normal, lightDir);

                    const Maths::Vector3f reflectDir = ray.Direction() - 2.0f * result.normal * (float)EDX::Vec3::Dot(ray.Direction(), result.normal);

                    if (n_dot_l > 0.0f) {

                        //If the ray is in shadow, don't reflect it. 
                        if (isVisible != false) {

                            const EDX::Colour& k_Light = light.GetColour();

                            const auto& att = light.GetAttenuation();
                            float attenuation = att.x + (att.y * dist) + (att.z * dist * dist);

                            auto toEye = (ray.Origin() - result.point);
                            toEye = toEye.Normalize();
                            const auto h = (lightDir + toEye).Normalize();
                            const float n_dot_h = EDX::Maths::Vector3f::Dot(result.normal, h);

                            c = c + ((k_Light * k_Light.a / attenuation) * ((m.diffuse * n_dot_l) + (m.specular * std::powf(std::max(n_dot_h, 0.0f), m.shininess))));

                            c = c + (RayColour({ result.point + (result.normal * reflectionBias), reflectDir }, depth, renderData) * m.specular);
                        }
                    }
                }


            }
        }
        else {
            c = EDX::Colour(result.normal.x + 1, result.normal.y + 1, result.normal.z + 1, 1.0f) * 0.5f;    //view Normals
        }
    }

    return c;

}

bool EDX::RayTracer::LoadSceneFile(const char* filePath, RenderData& renderData)
{
    EDX::Log::Status("Loading Scene \"%s\".\n", filePath);
    EDX::Timer timer;
    timer.Start();
    {

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
        std::list<EDX::Maths::Matrix4x4<float>> currentTransforms;

        EDX::Maths::Vector3f attenuation = { 1.0f, 0.0f, 0.0f };

        auto currentTransform = [&]() {
            EDX::Maths::Matrix4x4<float> acc = {};
            for (auto& t = currentTransforms.rbegin(); t != currentTransforms.rend(); t++) {
                acc = acc * *t;
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

            //EDX::Log::Debug("%s\n", line.c_str());

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
                    //transformStack.push(renderData.camera.GetViewMatrix());
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
                    //currentTransforms.clear();
                }
                else if (command == "popTransform") {
                    currentTransforms.clear();
                    currentTransforms.push_front(transformStack.top());
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
                    const EDX::Maths::Vector3f axis = EDX::Maths::Vector3f::Normalize({
                        std::stof(tokens[1]),
                        std::stof(tokens[2]),
                        std::stof(tokens[3])
                        });


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

    }

    timer.Tick();
    double dtms = timer.DeltaTime();
    EDX::Log::Success("Finished loading scene in %fs.\n", dtms);

    return true;
}
