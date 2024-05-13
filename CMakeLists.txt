cmake_minimum_required(VERSION 3.22)

project(gdllama)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(src_files
  "src/sqlite3.c"
  "src/sqlite3.h"
  "src/sqlite3ext.h"
  "src/sqlite-vec.c"
  "src/sqlite-vec.h"
  "src/conversion.h"
  "src/conversion.cpp"
  "src/gdembedding.h"
  "src/gdembedding.cpp"
  "src/gdllama.h"
  "src/gdllama.cpp"
  "src/gdllava.h"
  "src/gdllava.cpp"
  "src/embedding_runner.h"
  "src/embedding_runner.cpp"
  "src/llama_runner.h"
  "src/llama_runner.cpp"
  "src/llava_runner.h"
  "src/llava_runner.cpp"
  "src/register_types.h"
  "src/register_types.cpp"
)


file(GLOB sqlite_src_files
     "sqlite/src/*.h"
     "sqlite/src/*.c"
)

add_compile_options(-DSQLITE_CORE)


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
add_subdirectory(llama.cpp/examples/llava)

set(LIB_NAME libgodot_llm.${CMAKE_SYSTEM_NAME}.${CMAKE_BUILD_TYPE}.${CMAKE_SYSTEM_PROCESSOR})

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

string(TOLOWER ${LIB_NAME} LIB_NAME)

add_library(${LIB_NAME} SHARED ${src_files})

target_link_libraries(${LIB_NAME} PRIVATE godot-cpp common llama llava_static llava)

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

set(llama_runnner_test_files
  "test/src/llama_runner_test.cpp"
)

add_executable(llama_runner_test ${llama_runnner_test_files} ${src_files})

target_link_libraries(llama_runner_test PRIVATE godot-cpp common llama llava_static llava)

add_test(
  NAME llama_runner_test
  COMMAND llama_runner_test
)

set(embedding_runnner_test_files
  "test/src/embedding_runner_test.cpp"
)

add_executable(embedding_runner_test ${embedding_runnner_test_files} ${src_files})

target_link_libraries(embedding_runner_test PRIVATE godot-cpp common llama llava_static llava)

add_test(
  NAME embedding_runner_test
  COMMAND embedding_runner_test
)

set(llava_runnner_test_files
  "test/src/llava_runner_test.cpp"
)

add_executable(llava_runner_test ${llava_runnner_test_files} ${src_files})

target_link_libraries(llava_runner_test PRIVATE godot-cpp common llama llava_static llava)

add_test(
  NAME llava_runner_test
  COMMAND llava_runner_test
)
