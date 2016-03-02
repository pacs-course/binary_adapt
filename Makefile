#COMPILER FLAGS
CXX = g++ 
OPTIMIZE_OPS = -O3
CXXFLAGS = $(OPTIMIZE_OPS) -march=native -Wall -Wextra -std=c++11 $(INCLUDE_FLAGS)
INCLUDE_FLAGS = -I$(INCLUDE_DIR)

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

SRC = $(SRC_DIR)/main.cpp \
		$(SRC_DIR)/Poly.cpp

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))


EXE_TEST = $(BIN_DIR)/test

EXECUTABLES = $(EXE_TEST)
###################################
#  sezione generazione programmi  #
###################################
.PHONY:all
all: dir_tree $(EXECUTABLES)
#all:
#	echo $(OBJ)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXE_TEST): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

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

