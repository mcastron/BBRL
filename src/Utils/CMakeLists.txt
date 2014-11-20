
#	(optional) Boost 1.52 (for 'RandomGen')
find_package(Boost 1.52)

if (Boost_FOUND)
	set(Boost_USE_MULTITHREADED ON)
	link_directories(${Boost_LIBRARY_DIRS})
	include_directories(${Boost_INCLUDE_DIRS})
	
	add_definitions(-DBOOST_FOUND)

else()
	message(WARNING "BOOST NOT FOUND! (RandomGen)")

endif()


#	pthreads (for 'Thread')
find_package(Threads REQUIRED)


#	Zlib (for 'compressStr' & 'decompressStr')
find_package(ZLIB)

if (ZLIB_FOUND)
	add_definitions(-DZLIB_FOUND)

else()
	message(WARNING "ZLIB NOT FOUND!")

endif()
	


#Génération de la liste des fichiers sources

if (ZLIB_FOUND)
	file(
		GLOB_RECURSE
		lib_files
		src/*.cpp
	)

else()
	file(
		GLOB_RECURSE
		lib_files
		src/*.cpp
		src/zlib/*.c
	)
	
endif()

add_library(Utils ${lib_files})

#	Link pthreads
target_link_libraries(Utils ${CMAKE_THREAD_LIBS_INIT})


#	Link Boost
if (Boost_FOUND)
	target_link_libraries(Utils
						boost_chrono
						boost_thread
						boost_system)

endif()


#	Link zlib
if (ZLIB_FOUND)
	target_link_libraries(Utils ${ZLIB_LIBRARIES})

endif()
