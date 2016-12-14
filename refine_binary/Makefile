#COMPILER FLAGS
CXX = g++ 
OPTIMIZE_OPS = -O3
CXXFLAGS = $(OPTIMIZE_OPS) -march=native -std=c++14 $(INCLUDE_FLAGS) -Wall -Wextra
INCLUDE_FLAGS = -I$(INCLUDE_DIR)

EIGEN_DIR = /usr/local/include/Eigen
CXXFLAGS += -I$(EIGEN_DIR)

CXXFLAGS += -DSINGLETON_ENABLED

THIS_DIR = $(PWD)
BASE_DIR = $(THIS_DIR)/..

LIBNAME = refine_binary

#static version
LIBRARY_NAME = lib$(LIBNAME).a
STATIC_CXXFLAGS = -fPIC
CXXFLAGS += $(STATIC_CXXFLAGS)

#dynamic version
#TODO: verify if this library can be made dynamic (e' rientrante?)
#LIBRARY_NAME = lib$(LIBNAME).so
#DYNAMIC_CXXFLAGS = -fPIC -rdynamic
#CXXFLAGS += $(DYNAMIC_CXXFLAGS)
#DYNAMIC_LDLFLAGS = -shared -Wl,-soname,$(LIBRARY_NAME) -Wl,-rpath,$(LIBRARY_DIR)
#LDLFLAGS += $(DYNAMIC_LDLFLAGS)


#DIRECTORY TREE
OBJ_DIR = $(THIS_DIR)/obj
INCLUDE_DIR = $(THIS_DIR)/include
SRC_DIR = $(THIS_DIR)/src
LIBRARY_DIR = $(THIS_DIR)/lib

OBJ_DIRS = $(OBJ_DIR)
LIBRARY_DIRS = $(LIBRARY_DIR)
DIRS = $(OBJ_DIRS) $(LIBRARY_DIRS)

#INCLUDES
DEPS = $(INCLUDE_DIR)/*.h

SRC = $(wildcard $(SRC_DIR)/*.cpp)

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))


INSTALL_DIR = $(BASE_DIR)/lib
LIBRARY_FILE = $(LIBRARY_DIR)/$(LIBRARY_NAME)
STATIC_LIBRARY = $(LIBRARY_FILE)
LIBRARY_LINK = $(INSTALL_DIR)/$(LIBRARY_NAME)

###################################
#  sezione generazione programmi  #
###################################
.PHONY:all
all: dir_tree static

.PHONY:static
static:$(STATIC_LIBRARY)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(STATIC_LIBRARY): $(OBJ)
	ar -r $@  $(OBJ)

#TODO: copy the library, don't just link it
.PHONY:install
install: static
	cd $(INSTALL_DIR)
	ln -f -s $(STATIC_LIBRARY) $(INSTALL_DIR)/$(LIBRARY_NAME)
	cd $(THIS_DIR)
	rm $(OBJ)

.PHONY:uninstall
uninstall:
	@echo -n removing library link...
	@echo -n .
	$(RM) -f $(LIBRARY_LINK) 
	@echo  "done!"

.PHONY:dir_tree
dir_tree: $(DIRS)

$(DIRS) :
	@if [ ! -e $@ ] ; then \
		echo creating $@ \
		; mkdir $@ \
	; fi

.PHONY:clean
clean:
	@echo -n removing library file...
	@echo -n .
	$(RM) -f $(LIBRARY_FILE) 
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

#TODO
#.PHONY:debug
#debug: OPTIMIZE_OPS = -g -DMYDEBUG -O0
#debug: CXXFLAGS += -DDESTRUCTOR_ALERT -DMYDEBUG

#debug: all

#.PHONY:profile
#profile: PROFILE_OPS = -pg
#profile: $(SERIAL_EXE) #$(EXECUTABLES)
#	@if [ -e gmon.out ] ; then \
#		gprof $< gmon.out > $<.profile.txt \
#	; else \
#		echo ERROR! you must run $< in order to profile it! \
#	; fi

