#COMPILER FLAGS
CXX = g++ 
OPTIMIZE_OPS = -O3
CXXFLAGS = $(OPTIMIZE_OPS) -march=native -std=c++14 $(INCLUDE_FLAGS) -Wall -Wextra
INCLUDE_FLAGS = -I$(INCLUDE_DIR)

THIS_DIR = $(PWD)
BASE_DIR = $(THIS_DIR)/../
INSTALL_DIR = $(BASE_DIR)/lib

REFINER_DIR = $(BASE_DIR)/refine_binary
INCLUDE_FLAGS += -I$(REFINER_DIR)/include

#TODO : I want to generate two different libraries: one optimized, one for the debug
LIBNAME = my_functions
SO_NAME = lib$(LIBNAME).so
SO_FILE = $(SO_DIR)/$(SO_NAME).1.0

DYNAMIC_CXXFLAGS = -fPIC -rdynamic
CXXFLAGS += $(DYNAMIC_CXXFLAGS)
DYNAMIC_LDLFLAGS = -shared -Wl,-soname,$(SO_NAME) -Wl,-rpath,$(SO_DIR)
LDLFLAGS += $(DYNAMIC_LDLFLAGS)

#DIRECTORY TREE
OBJ_DIR = $(THIS_DIR)/obj
INCLUDE_DIR = $(THIS_DIR)/include
SRC_DIR = $(THIS_DIR)/src
SO_DIR = $(THIS_DIR)/lib

OBJ_DIRS = $(OBJ_DIR)
SO_DIRS = $(SO_DIR)
DIRS = $(OBJ_DIRS) $(SO_DIRS)

#INCLUDES
DEPS = $(INCLUDE_DIR)/*.h

SRC = $(wildcard $(SRC_DIR)/*.cpp)

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

DYNAMIC_LIBRARY = $(SO_FILE)
SO_LINK = $(INSTALL_DIR)/$(SO_NAME)
###################################
#  sezione generazione programmi  #
###################################

.PHONY:all
all: dir_tree dynamic

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY:dynamic
dynamic: $(DYNAMIC_LIBRARY)

$(DYNAMIC_LIBRARY): $(OBJ)
	$(CXX) $(LDLFLAGS) $(OBJ) -o $@ $(LIBS)

#TODO: copy the library, don't just link it
.PHONY:install
install: dynamic
	cd $(INSTALL_DIR)
	ln -f -s $(DYNAMIC_LIBRARY) $(INSTALL_DIR)/$(SO_NAME)
	cd $(THIS_DIR)
	rm $(OBJ)

.PHONY:uninstall
uninstall:
	@echo -n removing so_links...
	@echo -n .
	@rm -f $(SO_LINK)
	@echo  "done!"

#TODO: fix the debug
#.PHONY:debug
#debug: OPTIMIZE_OPS = -g -DMYDEBUG -O0
#debug: CXXFLAGS += -DDESTRUCTOR_ALERT -DMYDEBUG -DDEBUG
#debug: LIBS = -lmesh_dbg -lpthread
#debug: all

.PHONY:dir_tree
dir_tree: $(DIRS)

$(DIRS) :
	@if [ ! -e $@ ] ; then \
		echo creating $@ \
		; mkdir $@ \
	; fi

.PHONY:clean
clean:
	@echo -n removing so_files...
	@echo -n .
	@rm -f $(DYNAMIC_LIBRARY)
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


