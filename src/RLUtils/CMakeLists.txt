
#Génération de la liste des fichiers sources
file(
	GLOB_RECURSE
	lib_files
	src/*.cpp
)

add_library(RLUtils ${lib_files})
target_link_libraries(RLUtils Serializable)
