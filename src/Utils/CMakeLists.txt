
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
		src/fann*.c
	)

else()
	file(
		GLOB_RECURSE
		lib_files
		src/*.cpp
		src/zlib/*.c
		src/fann*.c
	)
	
endif()

add_library(Utils ${lib_files})

#	Link pthreads
target_link_libraries(Utils ${CMAKE_THREAD_LIBS_INIT})


#	Link zlib
if (ZLIB_FOUND)
	target_link_libraries(Utils ${ZLIB_LIBRARIES})

endif()

