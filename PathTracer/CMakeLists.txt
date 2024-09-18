# CSE-168 Path Tracer Application Project File
# Author: Ewan Burnett (EwanBurnettSK@Outlook.com)
# Last Revised: 2024-09-18

project("PathTracer")

find_package(Threads REQUIRED)

include(FetchContent)

if(${ENABLE_VIEWER})
    find_package(Vulkan REQUIRED)

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
    )

    FetchContent_Declare(
        vma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    )

    FetchContent_Declare(
        tinyobj
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG release
    )

    FetchContent_Declare(
        imgui 
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG master
    )

    FetchContent_Declare(
        imguizmo 
        GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo
        GIT_TAG master
    )
    FetchContent_Declare(
        nfd 
        GIT_REPOSITORY https://github.com/mlabbe/nativefiledialog
        GIT_TAG master
    )

    FetchContent_MakeAvailable(glfw vma tinyobj imgui imguizmo nfd)
endif()

# stb image library
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)

FetchContent_MakeAvailable(stb)

set(PATHTRACER_SOURCES "main.cpp" "Utils/Logger.h" "Utils/Logger.cpp" "Utils/Timer.h" "Utils/Timer.cpp" "Maths.h" "Maths/Utils.h" "Maths/Vector2.h"  "Maths/Vector3.h" "Maths/Vector4.h" "Maths/Matrix.h" "Maths/Quaternion.h" "Maths/Quaternion.cpp" "Colour.h" "Utils/ProgressBar.h" "Image.h" "Image.cpp" "Ray.h" "Camera.h" "Camera.cpp" "RayHit.h" "Primitives/Sphere.h" "Primitives/Sphere.cpp" "Primitives/Plane.h" "Primitives/Plane.cpp" "Primitives/Triangle.h" "Primitives/Triangle.cpp" "Scene.h" "Scene.cpp" "Lights/DirectionalLight.h" "Lights/DirectionalLight.cpp" "Materials/BlinnPhong.h" "Primitives/Box.h" "Primitives/Box.cpp" "Primitives/Primitive.h" "Primitives/Primitive.cpp" "Lights/PointLight.h" "Lights/PointLight.cpp" "RenderData.h" "Acceleration/Grid.h" "Acceleration/Grid.cpp" "Containers/TS_Stack.h" "RayTracer.h" "RayTracer.cpp" "Acceleration/AccelStructure.h" "Viewer.h")
set(PATHTRACER_LIBRARIES Threads::Threads)
set(PATHTRACER_INCLUDE_DIRS ${stb_SOURCE_DIR})

if(${ENABLE_VIEWER})
    set(PATHTRACER_SOURCES ${PATHTRACER_SOURCES} "Viewer.cpp" "Viewer.h" "Viewer/Window.h" "Viewer/Window.cpp" "${imgui_SOURCE_DIR}/imgui.cpp" "${imgui_SOURCE_DIR}/imgui.h" "${imgui_SOURCE_DIR}/imconfig.h" "${imgui_SOURCE_DIR}/imgui_demo.cpp" "${imgui_SOURCE_DIR}/imgui_draw.cpp" "${imgui_SOURCE_DIR}/imgui_tables.cpp" "${imgui_SOURCE_DIR}/imgui_widgets.cpp" "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp" "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp" "${imguizmo_SOURCE_DIR}/imguizmo.cpp" "${imguizmo_SOURCE_DIR}/imguizmo.h" "${nfd_SOURCE_DIR}/src/nfd_common.c" )

    # Platform-specific sources
    if(${PLATFORM_WINDOWS})
        set(PATHTRACER_SOURCES ${PATHTRACER_SOURCES} "${nfd_SOURCE_DIR}/src/nfd_win.cpp")
    elseif(${PLATFORM_UNIX})
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GTK REQUIRED gtk+-3.0)

        set(PATHTRACER_SOURCES ${PATHTRACER_SOURCES} "${nfd_SOURCE_DIR}/src/nfd_gtk.c")
        set(PATHTRACER_LIBRARIES ${PATHTRACER_LIBRARIES} ${GTK_LIBRARIES})
        set(PATHTRACER_INCLUDE_DIRS ${PATHTRACER_INCLUDE_DIRS} ${GTK_INCLUDE_DIRS}
    else()
        message(WARNING "Platform Not Supported!")
    endif()

    set(PATHTRACER_LIBRARIES ${PATHTRACER_LIBRARIES} Vulkan::Vulkan glfw GPUOpen::VulkanMemoryAllocator)
    set(PATHTRACER_INCLUDE_DIRS ${PATHTRACER_INCLUDE_DIRS} ${tinyobj_SOURCE_DIR} ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR} "${nfd_SOURCE_DIR}/src/include")
endif()

add_executable(${PROJECT_NAME} ${PATHTRACER_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PATHTRACER_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PATHTRACER_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PUBLIC 
    ENABLE_VIEWER=${ENABLE_VIEWER}
    PLATFORM_WINDOWS=${PLATFORM_WINDOWS} 
    PLATFORM_LINUX=${PLATFORM_LINUX}
)

# Copy scenes to outdir
add_custom_command(
    TARGET ${PROJECT_NAME} 
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Scenes/" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Scenes/"
)