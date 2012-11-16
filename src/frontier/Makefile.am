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

bin_PROGRAMS = frontier enumpath
frontier_SOURCES = $(MYSOURCES) $(MYHEADERS) frontier.cpp
enumpath_SOURCES = Graph.cpp PseudoZDD.cpp SolutionArray.cpp StateFrontier.cpp \
    MateSTPath.cpp \
    Global.hpp GraphInterface.hpp Graph.hpp State.hpp StateFrontier.hpp \
    MateSTPath.hpp ZDDNode.hpp PseudoZDD.hpp SolutionArray.hpp \
    enumpath.cpp

lib_LIBRARIES = libfrontier.a
libfrontier_a_SOURCES = $(MYSOURCES) $(MYHEADERS)

include_HEADERS = $(MYHEADERS)

CXXFLAGS = -O3 -Wall -Wextra -Wformat=2

if ENABLE_32BIT
  CXXFLAGS += -DBIT32
endif
