cmake_minimum_required(VERSION 3.22)

project(gdllama)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

file(GLOB src_files
  "src/*.h"
  "src/*.cpp"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    add_compile_options(
      /vmg
      /vmv
      -Wno-c++98-compat
      -Wno-c++98-compat-pedantic
      -Wno-double-promotion
      -Wno-old-style-cast
      -Wno-implicit-float-conversion
      -Wno-unsafe-buffer-usage
      -Wno-unused-command-line-argument
      -Wno-newline-eof
      -Wno-float-equal
      -Wno-unknown-argument
      -Wno-shorten-64-to-32
      -Wno-sign-conversion
      -Wno-float-conversion
      -Wno-cast-align
      -Wno-cast-qual
      -Wno-extra-semi-stmt
      -Wno-cast-function-type-strict
      -Wno-implicit-int-conversion
      -Wno-missing-prototypes
      -Wno-switch-default
      -Wno-exit-time-destructors
      -Wno-covered-switch-default
      -Wno-global-constructors
      "$<$<CONFIG:Debug>:-fstandalone-debug>"
    )
  endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fPIC)
endif()


add_subdirectory(godot-cpp)
add_subdirectory(llama.cpp)

set(LIB_NAME libgodot_llm.${CMAKE_SYSTEM_NAME}.${CMAKE_BUILD_TYPE}.${CMAKE_SYSTEM_PROCESSOR})

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

string(TOLOWER ${LIB_NAME} LIB_NAME)

add_library(${LIB_NAME} SHARED ${src_files})


target_link_libraries(${LIB_NAME} PRIVATE godot-cpp common llama)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

if (WIN32)
  install(TARGETS ${LIB_NAME}
    RUNTIME
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/addons/godot_llm/bin
  )
else()
  install(TARGETS ${LIB_NAME}
    LIBRARY
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/addons/godot_llm/bin
  )
endif()

install(FILES "godot_llm.gdextension" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/addons/godot_llm/)

file(GLOB test_files
  "test/src/*.h"
  "test/src/*.cpp"
)

add_executable(base_test ${test_files} ${src_files})

target_link_libraries(base_test PRIVATE godot-cpp common llama)
