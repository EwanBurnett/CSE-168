# CSE-168 Path Tracer Application Project File
# Author: Ewan Burnett (EwanBurnettSK@Outlook.com)
# Last Revised: 2024-09-26

project("PathTracer")

find_package(Threads REQUIRED)

include(FetchContent)

# stb image library
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)

FetchContent_MakeAvailable(stb)

add_executable(${PROJECT_NAME} "main.cpp" "Utils/Logger.h" "Utils/Logger.cpp" "Utils/Timer.h" "Utils/Timer.cpp" "Maths.h" "Maths/Utils.h" "Maths/Vector2.h"  "Maths/Vector3.h" "Maths/Vector4.h" "Maths/Matrix.h" "Maths/Quaternion.h" "Maths/Quaternion.cpp" "Colour.h" "Utils/ProgressBar.h" "Image.h" "Image.cpp" "Ray.h" "Camera.h" "Camera.cpp" "RayHit.h" "Primitives/Sphere.h" "Primitives/Sphere.cpp" "Primitives/Plane.h" "Primitives/Plane.cpp" "Primitives/Triangle.h" "Primitives/Triangle.cpp" "Scene.h" "Scene.cpp" "Lights/DirectionalLight.h" "Lights/DirectionalLight.cpp" "Materials/BlinnPhong.h" "Primitives/Box.h" "Primitives/Box.cpp" "Primitives/Primitive.h" "Primitives/Primitive.cpp" "Lights/PointLight.h" "Lights/PointLight.cpp" "RenderData.h" "Acceleration/Grid.h" "Acceleration/Grid.cpp" "Containers/TS_Stack.h" "RayTracer.h" "RayTracer.cpp" "Acceleration/AccelStructure.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} Threads::Threads)


# Copy scenes to outdir
add_custom_command(
    TARGET ${PROJECT_NAME} 
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Scenes/" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Scenes/"
)