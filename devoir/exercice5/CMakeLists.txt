# Version de cmake demandée.
CMAKE_MINIMUM_REQUIRED( VERSION 3.0.0 )
 
# Chemin des répertoires contenant les fichiers entêtes.
INCLUDE_DIRECTORIES( src/include )

# Ajoute l'options OpenMP au compilateur.
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp" )

# Chemin du répertoire contenant les binaires.
SET ( EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE} )

# Packages requis.
FIND_PACKAGE( OpenMP REQUIRED )

# Création des exécutables.
ADD_EXECUTABLE( exercice5 
                src/Metrics.cpp
		src/testParallelStableMerge.cpp
)

# Faire parler le make.
set( CMAKE_VERBOSE_MAKEFILE off )

