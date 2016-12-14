#COMPILER FLAGS
CXX = g++ 
OPTIMIZE_OPS = -O3
CXXFLAGS = $(OPTIMIZE_OPS) -march=native -std=c++14 $(INCLUDE_FLAGS) -Wall -Wextra
INCLUDE_FLAGS = -I$(INCLUDE_DIR)

LIBS += -ldl
THIS_DIR = $(PWD)
BASE_DIR = $(THIS_DIR)/..

CXXFLAGS += -rdynamic

REFINER_DIR = $(BASE_DIR)/refine_binary
INCLUDE_FLAGS += -I$(REFINER_DIR)/include

LIBRARIES_DIR = $(BASE_DIR)/lib

LDLFLAGS += -L$(LIBRARIES_DIR)
#linking dynamic libraries
LIBS += $(patsubst $(LIBRARIES_DIR)/lib%.so,-l%,$(wildcard $(LIBRARIES_DIR)/*.so))
#linking static libraries
LIBS += $(patsubst $(LIBRARIES_DIR)/lib%.a,-l%,$(wildcard $(LIBRARIES_DIR)/*.a))

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

SRC = $(wildcard $(SRC_DIR)/*.cpp)

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))


EXE_EXAMPLE = $(BIN_DIR)/example

EXECUTABLES = $(EXE_EXAMPLE)
###################################
#  sezione generazione programmi  #
###################################

.PHONY:all
all: dir_tree $(EXECUTABLES)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXE_EXAMPLE): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDLFLAGS) $(OBJ) -o $@ $(LIBS)

#.PHONY:debug
#debug: OPTIMIZE_OPS = -g -DMYDEBUG -O0
#debug: CXXFLAGS += -DDESTRUCTOR_ALERT -DMYDEBUG
#debug:

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

#.PHONY:profile
#profile: PROFILE_OPS = -pg
#profile: $(SERIAL_EXE) #$(EXECUTABLES)
#	@if [ -e gmon.out ] ; then \
#		gprof $< gmon.out > $<.profile.txt \
#	; else \
#		echo ERROR! you must run $< in order to profile it! \
#	; fi

#.PHONY:package
#WHOAMI = PACS_project
#package: clean
#	@if [ -e $(WHOAMI) ] ; then \
#		echo $(WHOAMI) exists! cleaning... \
#		; rm -rf $(WHOAMI)/* \
#	; else \
#		echo $(WHOAMI) does not exist! creating... \
#		; mkdir $(WHOAMI) \
#	; fi
#	@rm -f $(WHOAMI).zip
#	@echo ... preparing package inputs...
#	@cp Makefile $(WHOAMI)
#	@cp -r $(INCLUDE_DIR) $(WHOAMI)
#	@cp -r $(SRC_DIR) $(WHOAMI)
#	@echo ... creating zipfile...
#	@zip -q -r $(WHOAMI).zip $(WHOAMI)
#	@echo ... done!
#	@rm -r $(WHOAMI)

