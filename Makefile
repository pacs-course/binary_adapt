#COMPILER FLAGS
CXX = g++ 
OPTIMIZE_OPS = -O3
CXXFLAGS = $(OPTIMIZE_OPS) -march=native -std=c++14 $(INCLUDE_FLAGS) #-Wall -Wextra
INCLUDE_FLAGS = -I$(INCLUDE_DIR)

EIGEN_DIR = /usr/local/include/Eigen
LIBMESH_DIR = /usr/local/include/libmesh
CXXFLAGS += -I$(EIGEN_DIR)
CXXFLAGS += -I$(LIBMESH_DIR)
#CXXFLAGS += -DLIBMESH_HAVE_CXX11_UNIQUE_PTR
#CXXFLAGS += -DMYDEBUG
#CXXFLAGS += -DSINGLETON_ENABLED

LIBS = -lmesh_opt -lmesh_devel -lmesh_dbg -lpthread
LDLFLAGS = -L/usr/local/lib

#DIRECTORY TREE
OBJ_DIR = obj
INCLUDE_DIR = include
SRC_DIR = src
BIN_DIR = bin


OBJ_DIRS = $(OBJ_DIR)
BIN_DIRS = $(BIN_DIR)
DIRS = $(OBJ_DIRS) $(BIN_DIRS)


#INCLUDES
DEPS = $(INCLUDE_DIR)/*.h

#SRC = $(SRC_DIR)/main.cpp \
#		$(SRC_DIR)/Polinomial.cpp \
#		$(SRC_DIR)/BaseBuilderRule.cpp
SRC = $(wildcard $(SRC_DIR)/*.cpp)

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))


EXE_TEST = $(BIN_DIR)/test

EXECUTABLES = $(EXE_TEST)
###################################
#  sezione generazione programmi  #
###################################
.PHONY:all
all: dir_tree $(EXECUTABLES)
#all:
#	echo $(SRC)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXE_TEST): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDLFLAGS) $(OBJ) -o $@ $(LIBS)

.PHONY:debug
debug: OPTIMIZE_OPS = -g -DDEBUG
debug: all

.PHONY:dir_tree
dir_tree: $(DIRS)

$(DIRS) :
	@if [ ! -e $@ ] ; then \
		echo creating $@ \
		; mkdir $@ \
	; fi


.PHONY:clean
clean:
	@echo -n removing executables...
	@echo -n .
	@rm -f $(EXECUTABLES) 
	@echo  "done!"
	@echo -n removing object files...
	@echo -n .
	@find . -name "*.o" -delete
	@echo  "done!"
	@echo -n removing temporary files...
	@echo -n .
	@find . -name "*~" -delete
	@echo -n .
	@find . -name "*stcz*" -delete
	@echo  "done!"

.PHONY:profile
profile: PROFILE_OPS = -pg
profile: $(SERIAL_EXE) #$(EXECUTABLES)
	@if [ -e gmon.out ] ; then \
		gprof $< gmon.out > $<.profile.txt \
	; else \
		echo ERROR! you must run $< in order to profile it! \
	; fi

.PHONY:package
WHOAMI = PACS_project
package: clean
	@if [ -e $(WHOAMI) ] ; then \
		echo $(WHOAMI) exists! cleaning... \
		; rm -rf $(WHOAMI)/* \
	; else \
		echo $(WHOAMI) does not exist! creating... \
		; mkdir $(WHOAMI) \
	; fi
	@rm -f $(WHOAMI).zip
	@echo ... preparing package inputs...
	@cp Makefile $(WHOAMI)
	@cp -r $(INCLUDE_DIR) $(WHOAMI)
	@cp -r $(SRC_DIR) $(WHOAMI)
	@echo ... creating zipfile...
	@zip -q -r $(WHOAMI).zip $(WHOAMI)
	@echo ... done!
	@rm -r $(WHOAMI)

