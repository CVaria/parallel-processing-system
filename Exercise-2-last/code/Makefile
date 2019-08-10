include ../Makefile.include

INCLUDE_DIR = $(TBB_DIR)/include/
LIBRARY_DIR = $(TBB_DIR)/build/$(LIBRARY_VER)

CXX = g++
CXXFLAGS=-O3 -Wall -std=c++11 -DUSE_LAMBDAS
LDFLAGS =
LIBS = -ltbb -lrt

CXXFLAGS += -I$(INCLUDE_DIR)

# CXXFLAGS += -DDEBUG -g

OBJS = util.o

all : fw_v2 fw_sr_v1 fw_tiled_v1 fw_tiled_for_auto fw_tiled_for_aff

fw_v2: util.o  fw_v2.o
	$(CXX) $(LDFLAGS) util.o fw_v2.o -o fw_v2 -L$(LIBRARY_DIR) $(LIBS)

fw_sr_v1: util.o  fw_sr_v1.o
	$(CXX) $(LDFLAGS) util.o fw_sr_v1.o -o fw_sr_v1 -L$(LIBRARY_DIR) $(LIBS)

fw_tiled_v1: util.o  fw_tiled_v1.o
	$(CXX) $(LDFLAGS) util.o fw_tiled_v1.o -o fw_tiled_v1 -L$(LIBRARY_DIR) $(LIBS)

fw_tiled_for_auto: util.o  fw_tiled_for_auto.o
	$(CXX) $(LDFLAGS) util.o fw_tiled_for_auto.o -o fw_tiled_for_auto -L$(LIBRARY_DIR) $(LIBS)

fw_tiled_for_aff: util.o  fw_tiled_for_aff.o
	$(CXX) $(LDFLAGS) util.o fw_tiled_for_aff.o -o fw_tiled_for_aff -L$(LIBRARY_DIR) $(LIBS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean :
	rm -f *.o fw_v1 fw_v2 fw_sr_v1 fw_tiled_v1 fw_tiled_for_auto fw_tiled_for_aff
