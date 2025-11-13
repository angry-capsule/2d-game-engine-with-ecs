cmake_minimum_required(VERSION 3.20)
project(2d-game-engine-with-ecs LANGUAGES C CXX)

# -------------------- Common --------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_DIR "${CMAKE_SOURCE_DIR}/Library")

# -------------------- SDL2 + addons --------------------
set(SDL2_DIR        "${LIBRARY_DIR}/SDL2/cmake")
set(SDL2_image_DIR  "${LIBRARY_DIR}/SDL2_image/cmake")
set(SDL2_ttf_DIR    "${LIBRARY_DIR}/SDL2_ttf/cmake")
set(SDL2_mixer_DIR  "${LIBRARY_DIR}/SDL2_mixer/cmake")

find_package(SDL2        CONFIG REQUIRED)
find_package(SDL2_image  CONFIG REQUIRED)
find_package(SDL2_ttf    CONFIG REQUIRED)
find_package(SDL2_mixer  CONFIG REQUIRED)

set(SDL_DLL_DIR "${CMAKE_SOURCE_DIR}/DLL/x64")
file(GLOB SDL_DLLS "${SDL_DLL_DIR}/*.dll")

# -------------------- GLM --------------------
add_subdirectory("${LIBRARY_DIR}/glm" EXCLUDE_FROM_ALL)
set(HAVE_GLM_TARGET OFF)
if (TARGET glm::glm)
  set(HAVE_GLM_TARGET ON)
endif()

# -------------------- ImGui --------------------
file(GLOB_RECURSE IMGUI_FILES
  "${LIBRARY_DIR}/imgui/*.cpp"
  "${LIBRARY_DIR}/imgui/*.c"
  "${LIBRARY_DIR}/imgui/*.hpp"
  "${LIBRARY_DIR}/imgui/*.h"
)
set_property(GLOBAL PROPERTY CMAKE_CONFIGURE_DEPENDS "${LIBRARY_DIR}/imgui")

add_library(imgui STATIC ${IMGUI_FILES})
target_include_directories(imgui PUBLIC "${LIBRARY_DIR}/imgui")

target_link_libraries(imgui PUBLIC SDL2::SDL2 SDL2::SDL2main)

# -------------------- Lua (STATIC) --------------------
file(GLOB_RECURSE LUA_SRC "${LIBRARY_DIR}/lua/*.c")
list(FILTER LUA_SRC EXCLUDE REGEX ".*/lua\\.c$")
list(FILTER LUA_SRC EXCLUDE REGEX ".*/luac\\.c$")

add_library(lua STATIC ${LUA_SRC})
target_include_directories(lua PUBLIC "${LIBRARY_DIR}/lua")
set_target_properties(lua PROPERTIES OUTPUT_NAME "lua") # lua.lib
if (MSVC)
  target_compile_definitions(lua PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# -------------------- sol2 (header-only) --------------------
add_library(sol INTERFACE)
target_include_directories(sol INTERFACE "${LIBRARY_DIR}")

# -------------------- Executable --------------------

file(GLOB_RECURSE PROJECT_SOURCE 
	"Source/*.cpp" 
	"Source/*.h")
add_executable(2d-game-engine-with-ecs ${PROJECT_SOURCE})

add_custom_command(TARGET 2d-game-engine-with-ecs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL_DLLS}
        $<TARGET_FILE_DIR:2d-game-engine-with-ecs>
)

target_link_libraries(2d-game-engine-with-ecs PRIVATE
  SDL2::SDL2
  SDL2::SDL2main
  SDL2_image::SDL2_image
  SDL2_ttf::SDL2_ttf
  SDL2_mixer::SDL2_mixer
  imgui
  lua
  sol
)

target_compile_definitions(2d-game-engine-with-ecs PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if (HAVE_GLM_TARGET)
  target_link_libraries(2d-game-engine-with-ecs PRIVATE glm::glm)
else()
  target_include_directories(2d-game-engine-with-ecs PRIVATE "${LIBRARY_DIR}/glm")
endif()
