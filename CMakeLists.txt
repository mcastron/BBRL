
cmake_minimum_required(VERSION 2.6)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)


# ============================================================================
#	Project configuration
# ============================================================================
project(BBRL)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BUILD_TYPE})


# ============================================================================
#	Compilation command
# ============================================================================
# ----------------------------------------------------------------------------
#	DEBUG
# ----------------------------------------------------------------------------
#set(CMAKE_CXX_FLAGS "-O0 -g -Wall -W -Wextra -Wno-long-long -pedantic-errors")

# ----------------------------------------------------------------------------
#	VALGRIND
# ----------------------------------------------------------------------------
#set(CMAKE_CXX_FLAGS "-O0 -g")

# ----------------------------------------------------------------------------
#	GPROF
# ----------------------------------------------------------------------------
#add_definitions(-DNDEBUG)
#set(CMAKE_CXX_FLAGS "-O0 -pg")

# ----------------------------------------------------------------------------
#	RELEASE
# ----------------------------------------------------------------------------
add_definitions(-DNDEBUG)
set(CMAKE_CXX_FLAGS "-g -O3")


# ============================================================================
#	Custom ibraries
# ============================================================================
add_subdirectory(src/DDS)
add_subdirectory(src/IAgent)
add_subdirectory(src/IMDP)
add_subdirectory(src/IMDPDistribution)
add_subdirectory(src/IExperiment)
add_subdirectory(src/RLUtils)
add_subdirectory(src/Serializable)
add_subdirectory(src/Utils)
add_subdirectory(src/Xport)


# ============================================================================
#	Executables declaration
# ============================================================================
add_executable(BBRL-DDS src/main.cpp)
add_executable(BBRL-custom src/custom.cpp)
add_executable(BBRL-zlib	src/main-zlib.cpp)
add_executable(BBRL-export src/main-export.cpp)


# ============================================================================
#	Linkage
# ============================================================================
target_link_libraries(BBRL-DDS
					DDS
					IAgent
					IMDP
					IMDPDistribution
					IExperiment
					RLUtils
					Serializable
					Utils)		#	uses Boost 1.52 if possible.

target_link_libraries(BBRL-export
					DDS
					IAgent
					IMDP
					IMDPDistribution
					IExperiment
					RLUtils
					Serializable
					Utils          #	uses Boost 1.52 if possible.
					Xport)

target_link_libraries(BBRL-custom
					DDS
					IAgent
					IMDP
					IMDPDistribution
					IExperiment
					RLUtils
					Serializable
					Utils          #	uses Boost 1.52 if possible.
					Xport)

target_link_libraries(BBRL-zlib Utils)
