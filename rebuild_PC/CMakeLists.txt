cmake_minimum_required(VERSION 3.20)
project(CTR-PC)
include(CMakeDependentOption)

# === OPTIONS ===
# ALTMODS is always on with REBUILD_PC

option(CTR_60FPS "60 frames per second" ON)
option(CTR_16BY9 "Widescreen" ON)
cmake_dependent_option(CTR_NEW2P "Side-By-Side 2P" ON "CTR_16BY9" OFF)
option(CTR_OXIDE "Unlock Oxide" ON)
option(CTR_PENTA "Penta Max Stats" ON)
option(CTR_HARDER "Difficulty Selector (Arcade)" ON)
option(CTR_NEWCUPS "Cup Randomizer" ON)
option(CTR_BOOSTBAR "Super's reserve bar" ON)
option(CTR_PROFILER "Debug profiler from PizzaHut" ON)
option(CTR_4PADTEST "1 controller for all 4 players" OFF)
option(CTR_FASTANIM "Decompress animations" OFF)
option(CTR_BIGQUEUE "Allow LOAD queue to be more than 8" ON)
cmake_dependent_option(CTR_DRIVERLOD "All high model drivers" ON "CTR_BIGQUEUE" OFF)
cmake_dependent_option(CTR_DRIVERRND "Character Randomizer" OFF "CTR_DRIVERLOD" OFF)

cmake_dependent_option(CTR_PRELOAD "Preloads more characters" OFF "CTR_BIGQUEUE" OFF)

option(CTR_RAMEX "8mb RAM expansion" OFF)
cmake_dependent_option(CTR_ONLINE "Online Multiplayer" OFF "CTR_RAMEX" OFF)
cmake_dependent_option(CTR_HIGHMP "Multiplayer Maxed mod" OFF "CTR_RAMEX" OFF)
cmake_dependent_option(CTR_LEVELDEV "Level with hot-reload" OFF "CTR_RAMEX" OFF)

option(CTR_VR "Virtual Reality" OFF)
# PCDRV is always on with REBUILD_PC

option(CTR_PRIM "PsyX USE_EXTENDED_PRIM_POINTERS" 0)

# === BASICS ===

set_source_files_properties("CrashTeamRacingPC.c" PROPERTIES LANGUAGE C)
add_executable(ctr_bin "CrashTeamRacingPC.c")
set_target_properties(ctr_bin PROPERTIES OUTPUT_NAME "CrashTeamRacingPC")
target_include_directories(ctr_bin PUBLIC "../include")

# === COMPILER OPTIONS ===

set_property(TARGET ctr_bin PROPERTY C_STANDARD 99)
target_compile_options(ctr_bin PUBLIC -DUSE_EXTENDED_PRIM_POINTERS=${CTR_PRIM})

if(CTR_60FPS)
  target_compile_options(ctr_bin PUBLIC -DUSE_60FPS)
endif()
if(CTR_16BY9)
  target_compile_options(ctr_bin PUBLIC -DUSE_16BY9)
endif()
if(CTR_NEW2P)
  target_compile_options(ctr_bin PUBLIC -DUSE_NEW2P)
endif()
if(CTR_OXIDE)
  target_compile_options(ctr_bin PUBLIC -DUSE_OXIDE)
endif()
if(CTR_PENTA)
  target_compile_options(ctr_bin PUBLIC -DUSE_PENTA)
endif()
if(CTR_HARDER)
  target_compile_options(ctr_bin PUBLIC -DUSE_HARDER)
endif()
if(CTR_NEWCUPS)
  target_compile_options(ctr_bin PUBLIC -DUSE_NEWCUPS)
endif()
if(CTR_BOOSTBAR)
  target_compile_options(ctr_bin PUBLIC -DUSE_BOOSTBAR)
endif()
if(CTR_PROFILER)
  target_compile_options(ctr_bin PUBLIC -DUSE_PROFILER)
endif()
if(CTR_4PADTEST)
  target_compile_options(ctr_bin PUBLIC -DUSE_4PADTEST)
endif()
if(CTR_FASTANIM)
  target_compile_options(ctr_bin PUBLIC -DUSE_FASTANIM)
endif()
if(CTR_BIGQUEUE)
  target_compile_options(ctr_bin PUBLIC -DUSE_BIGQUEUE)
endif()
if(CTR_DRIVERLOD)
  target_compile_options(ctr_bin PUBLIC -DUSE_DRIVERLOD)
endif()
if(CTR_DRIVERRND)
  target_compile_options(ctr_bin PUBLIC -DUSE_DRIVERRND)
endif()

if(CTR_PRELOAD)
  target_compile_options(ctr_bin PUBLIC -DUSE_PRELOAD)
endif()


if(CTR_RAMEX)
  target_compile_options(ctr_bin PUBLIC -DUSE_RAMEX)
endif()
if(CTR_ONLINE)
  target_compile_options(ctr_bin PUBLIC -DUSE_ONLINE)
endif()
if(CTR_HIGHMP)
  target_compile_options(ctr_bin PUBLIC -DUSE_HIGHMP)
endif()
if(CTR_LEVELDEV)
  target_compile_options(ctr_bin PUBLIC -DUSE_LEVELDEV)
endif()

if(CTR_VR)
  target_compile_options(ctr_bin PUBLIC -DUSE_VR)
endif()

# Compiler options
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  target_compile_options(ctr_bin PUBLIC -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-implicit-function-declaration -Wno-return-type)
  if(MINGW OR CYGWIN)
    if(NOT CMAKE_VERSION VERSION_LESS "3.13")
      target_link_options(ctr_bin PUBLIC -static-libgcc)
    endif()
  endif()
else()
  # Assume GCC
  target_compile_options(ctr_bin PRIVATE -Wno-int-conversion -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-implicit-int)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  target_compile_options(ctr_bin PUBLIC -g -gdwarf-2 -O0)
else()
  target_compile_options(ctr_bin PUBLIC -O2)
endif()

# === LINKER OPTIONS ===

# Clang always needs "no-pie", but some distros might add PIE to GCC too.
target_link_options(ctr_bin PUBLIC -fno-pie -no-pie -Wl,-Ttext,0x00500000)

# === DEPENDENCIES ===

find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
  add_subdirectory("../externals/SDL")
  set(SDL2_LIBRARIES SDL2 SDL2main)
  set(SDL2_INCLUDE_DIR "../externals/SDL/include")
endif()

target_link_libraries(ctr_bin ${SDL2_LIBRARIES})
target_include_directories(ctr_bin PRIVATE ${SDL2_INCLUDE_DIRS})

find_package(OpenAL QUIET)
if(NOT OpenAL_FOUND)
  add_subdirectory("../externals/openal-soft")
  set(OPENAL_LIBRARY OpenAL)
  set(OPENAL_INCLUDE_DIR "../externals/openal-soft/include")
endif()

# === PsyCross ===

file(GLOB_RECURSE PSYCROSS_SRCS_C
  "../externals/PsyCross/*.c" "../externals/PsyCross/*.C"
)

file(GLOB_RECURSE PSYCROSS_SRCS_CPP
  "../externals/PsyCross/*.cpp"
)

set_source_files_properties(${PSYCROSS_SRCS_C} PROPERTIES LANGUAGE C)
set_source_files_properties(${PSYCROSS_SRCS_CPP} PROPERTIES LANGUAGE CXX)

add_library(psycross_static STATIC ${PSYCROSS_SRCS_C} ${PSYCROSS_SRCS_CPP})
set_target_properties(psycross_static PROPERTIES OUTPUT_NAME "psycross")
target_include_directories(psycross_static PUBLIC "../externals/PsyCross/include")

target_compile_options(psycross_static PRIVATE -Wno-narrowing -Wno-incompatible-pointer-types -Wno-implicit-function-declaration)

target_link_libraries(psycross_static ${SDL2_LIBRARIES})
target_include_directories(psycross_static PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(psycross_static ${OPENAL_LIBRARY})
target_include_directories(psycross_static PRIVATE ${OPENAL_INCLUDE_DIR})

# === OUR PsyCross USAGE ===

target_compile_options(psycross_static PRIVATE -DUSE_EXTENDED_PRIM_POINTERS=${CTR_PRIM} -O2)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  target_compile_options(psycross_static PUBLIC -g -gdwarf-2)
endif()

# Clang is rigorous
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(psycross_static PRIVATE -Wno-int-conversion -Wno-implicit-function-declaration)
endif()

target_link_libraries(ctr_bin psycross_static)
target_include_directories(ctr_bin PRIVATE ${PsyCross_SOURCE_DIR}/include)
