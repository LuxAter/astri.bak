# ##############################################################################
# PROJECT
# ##############################################################################
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(
  Astri
  VERSION 0.0.1
  LANGUAGES C CXX)

option(STATIC_ANALYSIS "Enable static analysis" FALSE)

# ##############################################################################
# SOURCE
# ##############################################################################
set(SOURCE src/astri.cpp src/glad.c src/stb_image_write.c src/log.cpp src/icosasphere.cpp src/shader.cpp
  src/imgui/imgui.cpp src/imgui/imgui_draw.cpp src/imgui/imgui_impl_glfw.cpp src/imgui/imgui_impl_opengl3.cpp src/imgui/imgui_widgets.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ##############################################################################
# BUILD TYPE
# ##############################################################################
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
    "Debug"
    CACHE STRING "Choose the type of build" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
  if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
      ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)
  endif()
endif()

# ##############################################################################
# STATIC ANALYSIS
# ##############################################################################
if(STATIC_ANALYSIS OR CMAKE_BUILD_TYPE STREQUAL "Release")
  find_program(CLANG_TIDY "clang-tidy")
  if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
  endif()
  find_program(CPPCHECK "cppcheck")
  if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK ${CPPCHECK})
    list(APPEND CMAKE_CXX_CPPCHECK "--force")
  endif()
  find_program(CPPLINT "cpplint")
  if(CPPLINT)
    set(CMAKE_CXX_CPPLINT ${CPPLINT})
  endif()
endif()

# ##############################################################################
# TARGETS
# ##############################################################################
find_package(Git REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external)

add_executable(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(
  ${PROJECT_NAME} PRIVATE -Wall -Wextra
  $<$<CONFIG:Debug>:-O0>$<$<CONFIG:Release>:-O3>)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:DEBUG>$<$<CONFIG:Release>:RELEASE>)
set_target_properties(
  ${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED TRUE
  CXX_EXTENSIONS TRUE)

target_link_libraries(${PROJECT_NAME} libcurl)
target_link_libraries(${PROJECT_NAME} CLI11::CLI11)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

install(TARGETS ${PROJECT_NAME}_static)

# ##############################################################################
# PACKAGING
# ##############################################################################
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CPACK_GENERATOR "STGZ;TGZ;ZIP")
  set(CPACK_PACKAGE_VENDOR "LuxAter")
  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
  set(CPACK_MONOLITHIC_INSTALL TRUE)
  include(CPack)
endif()
