
#Génération de la liste des fichiers sources
file(
	GLOB_RECURSE
	lib_files
	src/*.cpp
)

add_library(Serializable ${lib_files})
#target_link_libraries(Serializable)
