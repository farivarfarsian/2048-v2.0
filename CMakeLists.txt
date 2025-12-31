# CMakeList.txt : CMake project for 2048 v2.0, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("2048 v2.0")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Add source to this project's executable.
add_executable (2048-v2.0) # WIN32 for Release Build should be applied

target_sources(2048-v2.0 PRIVATE
	"src/main.cpp"
)

# Link SDL3
find_package(SDL3)
target_link_libraries(2048-v2.0 PRIVATE SDL3::SDL3)
target_link_libraries(2048-v2.0 PRIVATE 
    mingw32 
    ${CMAKE_SOURCE_DIR}/SDL3/i686-w64-mingw32/bin/SDL3_image.dll)
target_link_libraries(2048-v2.0 PRIVATE 
    mingw32 
    ${CMAKE_SOURCE_DIR}/SDL3/i686-w64-mingw32/bin/SDL2_mixer.dll)
target_compile_definitions(2048-v2.0 PRIVATE SDL3_MAIN_HANDLED)
target_link_options(2048-v2.0 PRIVATE -static)



# Copy SDL3.dll to output directory after build
# for mingw32 compiler /SDL3/i686-w64-mingw32/bin/SDL3.dll (Doesn't work with 64bit version I don't know WHY)
# for msvc compiler /SDL3/x86_64-w64-mingw32/SDL3.dll
add_custom_command(TARGET 2048-v2.0 POST_BUILD  
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/SDL3/i686-w64-mingw32/bin/SDL3.dll"      
       $<TARGET_FILE_DIR:2048-v2.0>)
       
add_custom_command(TARGET 2048-v2.0 POST_BUILD  
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/SDL3/i686-w64-mingw32/bin/SDL3_image.dll"      
       $<TARGET_FILE_DIR:2048-v2.0>)

add_custom_command(TARGET 2048-v2.0 POST_BUILD  
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/SDL3/i686-w64-mingw32/bin/SDL2_mixer.dll"      
       $<TARGET_FILE_DIR:2048-v2.0>) 

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 2048-v2.0 PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.
