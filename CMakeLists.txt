cmake_minimum_required(VERSION 3.10)

project(gdextension)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB src_files
	"src/*.h"
	"src/*.cpp"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /vmg /vmv -Wno-c++98-compat  -Wno-c++98-compat-pedantic -Wno-double-promotion -Wno-old-style-cast -Wno-implicit-float-conversion -Wno-unsafe-buffer-usage -Wno-unused-command-line-argument -Wno-newline-eof -Wno-float-equal -Wno-unknown-argument -Wno-shorten-64-to-32 -Wno-sign-conversion -Wno-float-conversion -Wno-cast-align -Wno-cast-qual -Wno-extra-semi-stmt -Wno-cast-function-type-strict -Wno-implicit-int-conversion -Wno-missing-prototypes -Wno-switch-default -Wno-exit-time-destructors -Wno-covered-switch-default -Wno-global-constructors")
	endif()
endif()

add_subdirectory(godot-cpp)

add_library(libgdexample.windows.template_debug.x86_64 SHARED ${src_files})

target_link_libraries(libgdexample.windows.template_debug.x86_64 PUBLIC godot-cpp)
