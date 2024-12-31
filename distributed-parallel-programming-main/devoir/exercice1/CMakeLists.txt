# Version de cmake demand�e.
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
 
# Chemin des r�pertoires contenant les fichiers ent�tes.
INCLUDE_DIRECTORIES( src/include )

# Chemin du r�pertoire contenant les binaires.
SET ( EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE} )

# Cr�ation des ex�cutables.
ADD_EXECUTABLE(exercice1 src/main.cpp)
# Faire parler le make.
set( CMAKE_VERBOSE_MAKEFILE off )