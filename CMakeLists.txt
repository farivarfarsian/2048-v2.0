# CMakeList.txt : CMake project for 2048 v2.0, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("2048 v2.0" LANGUAGES CXX)

#Setting C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Add source to this project's executable.
add_executable (2048-v2.0) # WIN32 for Release Build should be applied

target_sources(2048-v2.0 PRIVATE
	"src/main.cpp"
    "src/tinyxml2.cpp"
)

# Link SDL3
find_package(SDL3)
find_package(SDL3/cmake/SDL3_ttf)
find_package(SDL3/cmake/SDL3_image)
target_link_libraries(2048-v2.0 PRIVATE SDL3::SDL3)
target_link_libraries(2048-v2.0 PRIVATE SDL3_image::SDL3_image SDL3::SDL3)
target_link_libraries(2048-v2.0 PRIVATE SDL3_ttf::SDL3_ttf SDL3::SDL3)
target_compile_definitions(2048-v2.0 PRIVATE SDL3_MAIN_HANDLED)
target_link_options(2048-v2.0 PRIVATE -static)

# Making sure GNU Compiler uses latest C++23 + Optimization Level 3 flag for the compiler
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(2048-v2.0 PRIVATE "-std=gnu++23" "-O3")
endif()

# Include folder
target_include_directories(2048-v2.0 PRIVATE "include")

# Copy SDL3.dll to output directory after build
add_custom_command(TARGET 2048-v2.0 POST_BUILD  
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/SDL3/x86_64-w64-mingw32/bin/SDL3.dll"      
       $<TARGET_FILE_DIR:2048-v2.0>)
       
add_custom_command(TARGET 2048-v2.0 POST_BUILD  
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/SDL3/x86_64-w64-mingw32/bin/SDL3_image.dll"      
       $<TARGET_FILE_DIR:2048-v2.0>)

add_custom_command(TARGET 2048-v2.0 POST_BUILD  
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/SDL3/x86_64-w64-mingw32/bin/SDL3_ttf.dll"      
       $<TARGET_FILE_DIR:2048-v2.0>)

#Copying assets folder to the main exe file
add_custom_command(TARGET 2048-v2.0 POST_BUILD  
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${PROJECT_SOURCE_DIR}/assets"      
       $<TARGET_FILE_DIR:2048-v2.0>/assets)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 2048-v2.0 PROPERTY CXX_STANDARD 20)
endif()