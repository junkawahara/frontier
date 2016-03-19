FRONTIER_DIR = ../frontier
FRONTIER_LIB_DIR = ../frontier_lib/

bin_PROGRAMS = makegrid distfilter
makegrid_SOURCES = makegrid.c
distfilter_SOURCES = $(FRONTIER_LIB_DIR)GraphInterface.hpp $(FRONTIER_LIB_DIR)Graph.hpp \
    distfilter.cpp

AUTOMAKE_OPTIONS = subdir-objects
