MYSOURCES = BigInteger.cpp Graph.cpp HyperGraph.cpp MateComponent.cpp \
    MateKcut.cpp MateMTPath.cpp MatePathMatching.cpp MateRForest.cpp \
    MateRcut.cpp MateSTPath.cpp MateSTPathDist.cpp MateSTree.cpp \
    MateSetCover.cpp MateSetPacking.cpp MateSetPartition.cpp PseudoZDD.cpp \
    SolutionArray.cpp StateFrontier.cpp StateFrontierHyper.cpp

MYHEADERS = BigInteger.hpp Global.hpp Graph.hpp \
    GraphInterface.hpp HyperGraph.hpp MateComponent.hpp \
    MateForestBase.hpp MateKcut.hpp MateMTPath.hpp MatePathMatching.hpp \
    MateRForest.hpp MateRcut.hpp MateSForest.hpp MateSTPath.hpp \
    MateSTPathDist.hpp MateSTree.hpp MateSetCover.hpp MateSetPacking.hpp \
    MateSetPartition.hpp Packer.hpp PseudoZDD.hpp RootManager.hpp SolutionArray.hpp \
    State.hpp StateFrontier.hpp StateFrontierFixed.hpp StateFrontierHyper.hpp \
    ZDDNode.hpp

MYSOURCESNEW = BigInteger.cpp Graph.cpp HyperGraph.cpp MatePartition.cpp \
    MateSTPath.cpp MateSTPathDist.cpp \
    PseudoZDD.cpp MateFGeneral.cpp MateHyperSForest.cpp MateHyperReli.cpp \
    SolutionArray.cpp StateFrontier.cpp

MYHEADERSNEW = BigInteger.hpp Global.hpp Graph.hpp \
    GraphInterface.hpp HyperGraph.hpp MatePartition.hpp \
    MateSTPath.hpp \
    MateSTPathDist.hpp MateFGeneral.hpp MateHyperSForest.hpp \
    Packer.hpp PseudoZDD.hpp RootManager.hpp SolutionArray.hpp \
    State.hpp StateFrontier.hpp StateFrontierAux.hpp StateFrontierHyper.hpp \
	StateFrontierHyperAux.hpp \
    ZDDNode.hpp CompManager.hpp MateHyperReli.hpp

bin_PROGRAMS = frontier enumpath readxdd
frontier_SOURCES = $(MYSOURCESNEW) $(MYHEADERSNEW) frontier.cpp
enumpath_SOURCES = Graph.cpp BigInteger.cpp PseudoZDD.cpp SolutionArray.cpp StateFrontier.cpp \
    MateSTPath.cpp \
    Global.hpp GraphInterface.hpp Graph.hpp State.hpp StateFrontier.hpp \
    MateSTPath.hpp ZDDNode.hpp PseudoZDD.hpp SolutionArray.hpp \
	MateFGeneral.cpp \
    enumpath.cpp
readxdd_SOURCES = BigInteger.cpp Graph.cpp HyperGraph.cpp \
    PseudoZDD.cpp SolutionArray.cpp StateFrontier.cpp \
	readxdd.cpp

lib_LIBRARIES = libfrontier.a
libfrontier_a_SOURCES = $(MYSOURCESNEW) $(MYHEADERSNEW)

include_HEADERS = $(MYHEADERSNEW)

AUTOMAKE_OPTIONS = subdir-objects

AM_CXXFLAGS = -Wall -Wextra -Wformat=2

if ENABLE_32BIT
  AM_CXXFLAGS += -DBIT32
endif

if ENABLE_DEBUG
  AM_CXXFLAGS += -g
  AM_CXXFLAGS += -DDEBUG
else
  AM_CXXFLAGS += -O3
endif
