
#	(optional) Boost 1.52 (for 'Guez/' lib)
find_package(Boost 1.52)

if (Boost_FOUND)
	set(Boost_USE_MULTITHREADED ON)
	link_directories(${Boost_LIBRARY_DIRS})
	include_directories(${Boost_INCLUDE_DIRS})
	
	add_definitions(-DBOOST_FOUND)

else()
	message(WARNING "BOOST NOT FOUND! (Guez lib)")

endif()


#Génération de la liste des fichiers sources
file(
	GLOB_RECURSE
	lib_files
	src/*.cpp
)

add_library(DDS ${lib_files})
target_link_libraries(DDS
					IAgent
					IMDP
					IMDPDistribution
					IExperiment)


#	Link Boost
if (Boost_FOUND)
	target_link_libraries(DDS
						boost_chrono
						boost_thread
						boost_system)

endif()