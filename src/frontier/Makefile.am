LIBDIR = ../frontier_lib/

LIBHEADERS = $(LIBDIR)BigInteger.hpp $(LIBDIR)FrontierAlgorithm.hpp \
    $(LIBDIR)FrontierComp.hpp $(LIBDIR)FrontierManager.hpp $(LIBDIR)Global.hpp \
    $(LIBDIR)Graph.hpp $(LIBDIR)GraphInterface.hpp $(LIBDIR)HashTable.hpp \
    $(LIBDIR)HyperGraph.hpp $(LIBDIR)Mate.hpp $(LIBDIR)MatePrinter.hpp \
    $(LIBDIR)PseudoZDD.hpp \
    $(LIBDIR)RBuffer.hpp $(LIBDIR)RootManager.hpp $(LIBDIR)SolutionArray.hpp \
    $(LIBDIR)State.hpp $(LIBDIR)StateFrontier.hpp $(LIBDIR)StateFrontierComp.hpp \
    $(LIBDIR)StateFrontierCompHyper.hpp $(LIBDIR)StateFrontierHyper.hpp \
    $(LIBDIR)StateHyper.hpp $(LIBDIR)ZDDNode.hpp

MYSOURCES = StateCombination.cpp \
    StateDSTPath.cpp \
    StateKcut.cpp StateMTPath.cpp \
    StatePathMatching.cpp StateRcut.cpp StateRForest.cpp StateSetCover.cpp \
    StateSetPacking.cpp StateSetPartition.cpp StateSForest.cpp StateSTPath.cpp \
    StateSTPathDist.cpp StateSTree.cpp  

MYHEADERS = OptionParser.hpp \
    StateDSTPath.hpp \
    StateKcut.hpp StateMTPath.hpp \
    StatePathMatching.hpp StateRcut.hpp StateRForest.hpp StateSetCover.hpp \
    StateSetPacking.hpp StateSetPartition.hpp StateSForest.hpp StateSTPath.hpp \
    StateSTPathDist.hpp StateSTree.hpp $(LIBHEADERS)

bin_PROGRAMS = frontier testfrontier 
frontier_SOURCES = $(MYSOURCES) $(MYHEADERS) frontier.cpp
testfrontier_SOURCES = $(MYSOURCES) $(MYHEADERS) testfrontier.cpp

#readxdd_SOURCES = BigInteger.cpp Graph.cpp HyperGraph.cpp \
#    PseudoZDD.cpp SolutionArray.cpp StateFrontier.cpp \
#	readxdd.cpp

lib_LIBRARIES = libfrontier.a
libfrontier_a_SOURCES = $(MYSOURCES) $(MYHEADERS)

include_HEADERS = $(MYHEADERS)

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
