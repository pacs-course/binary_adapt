# Mode specific values are added after the mode agnostic values.
# The generated binary is stored as $(ObjDir)/$(EXENAME)_$(buildmode)
# except for the Opt binary. It is named $(ObjDir)/$(EXENAME).
# You can change the location by changing macro TargetFilename.
# The object files are stored in directory $(ObjDir)/$(buildmode)/$(EXENAME)
# You can change the location by changing ObjectFilename
# Variable "Modes" contains the list of build modes. Add your own mode with "Modes += mymode" if you like and add
# additional compiler and linker flags with "CXXFlags_mymode := " and "LFlags_mymode := ".

EXENAME	:= test
#TODO: FIX THE DEBUG VERSION
Modes		:= Opt #Debug

#DIRECTORY TREE
BaseDir 		= $(THIS_DIR)/..
CurrentDir	= $(PWD)
THIS_DIR		= .
INCLUDE_DIR = $(THIS_DIR)/include
SRC_DIR		= $(THIS_DIR)/src
BIN_DIR		= $(THIS_DIR)/bin
OBJ_DIR		= $(THIS_DIR)/obj

OBJ_DIRS	= $(OBJ_DIR)
BIN_DIRS	= $(BIN_DIR)
DIRS		= $(OBJ_DIRS) $(BIN_DIRS)

RefineDir		= $(BaseDir)/refine_binary
RefineInclude	= $(RefineDir)/include

BridgesDir			= $(BaseDir)/library_bridges
MeshBridgeDir		= $(BridgesDir)/libmesh_bridge
MeshBridgeInclude	= $(MeshBridgeDir)/include

FunctionsDir 		= $(BaseDir)/interpolating_functions
FunctionsInclude	= $(FunctionsDir)/include

LibMeshInclude = /usr/local/include/libmesh

# Directory where target binary is created and object files
ObjDir := $(OBJ_DIR)
BinDir := $(BIN_DIR)
# Search Path For Include Files (InclPaths is included before mode specific InclPaths_MODE)
InclPaths			:= $(INCLUDE_DIR) $(RefineInclude) $(MeshBridgeInclude) $(LibMeshInclude) $(FunctionsInclude)
InclPaths_Debug	:=
InclPaths_Opt		:=
# Defined values used by CPP preprocessor (Defines is included before mode specific Defines_MODE)
Defines			:= 
Defines_Debug	:= DEBUG
Defines_Opt		:= OPT
# Compiler specific flags (CXXFlags is included before mode specific CXXFlags_MODE)
CXXFlags			:= -march=native -std=c++14 $(INCLUDE_FLAGS) -Wall -Wextra
CXXFlags_Debug	:= -g
CXXFlags_Opt	:= -O3


StaticLibsNames	:= refine_binary 
DynamicLibsNames	:= my_functions mesh_bridge
LibsNames			:= $(StaticLibsNames) $(DynamicLibsNames)
LibMeshLib			:= mesh_opt
LibMeshLibDebug	:= mesh_dbg

# List of Library Names (Libs is included before mode specific Libs_MODE)
Libs			:= pthread
Libs_Debug	:= $(patsubst %, %_Debug, $(LibsNames)) $(LibMeshLibDebug)
Libs_Opt		:= $(LibsNames) $(LibMeshLib)

LibMeshLibPath := /usr/local/lib
MyLibsPath		:= $(BaseDir)/lib
# Search Paths for Libraries (LibPaths is included before mode specific LibPaths_MODE)
LibPaths			:= $(MyLibsPath) $(LibMeshLibPath)
LibPaths_Debug	:=
LibPaths_Opt	:=

#TODO: separate the debug files from the opt ones
StaticLibsFiles := $(patsubst %, $(MyLibsPath)/lib%.a, $(StaticLibsNames))

LD := g++

# Includes
DEPS = $(INCLUDE_DIR)/*.h

# Source files
SRC := $(notdir $(wildcard $(SRC_DIR)/*.cpp))


##########################
# Compiler Specific Part #
##########################
IncPathFlag := -I
DefineFlag  := -D
LibraryFlag := -l
LibPathFlag := -L
CXX         := g++

# Executables
EXECUTABLES = $(BIN_DIR)/$(EXENAME)

#########################
# Generic Template Part #
#########################
all: $(Modes)

.PHONY:dir_tree
dir_tree: $(DIRS)

$(DIRS) :
	@test -d $@ || (echo creating $@ ; mkdir -p $@)

#TODO: fix it, it does not clean anything
clean: $(EXENAME)_clean $(Modes:%=clean_%)
	@echo Done!

$(EXENAME)_clean:
	@echo cleaning $(EXENAME)
	@rm -rf $(BinDir)/*

$(Modes:%=clean_%):
	@echo cleaning $(ObjDir)/$(subst clean_,,$@)
	@rm -rf $(ObjDir)/$(subst clean_,,$@)

$(Modes:%=init_%): dir_tree
	@mkdir -p $(ObjDir)/$(subst init_,,$@)

define TargetFilename
$(BinDir)/$(EXENAME)$(subst _Opt,,_$(1))
endef

define ObjectFilename
$(ObjDir)/$(1)/$(subst .cpp,.o,$(2))
endef

define CompileObject
$(call ObjectFilename,$(1),$(2)): $(SRC_DIR)/$(2) $(DEPS)
	@echo compiling: '$$<'
	$$(CXX_$(1)) -c -o '$$@' '$$<'
endef

define TargetTemplate
CXX_$(1) := $(CXX) $(Defines:%=$(DefineFlag)%) $$(Defines_$(1):%=$(DefineFlag)%) $(InclPaths:%=$(IncPathFlag)%) $$(InclPaths_$(1):%=$(IncPathFlag)%) $(CXXFlags) $$(CXXFlags_$(1))

Objects_$(1) := $(foreach file,$(SRC),$(call ObjectFilename,$(1),$(file)))

$(1):	init_$(1) $(call TargetFilename,$(1))

$(call TargetFilename,$(1)): $$(Objects_$(1)) $(StaticLibsFiles)
	@echo $(LD): generating '$$(notdir $$@)'
	$(LD) $(LFlags) $$(LFlags_$(1)) -o $(BinDir)/'$$(notdir $$@)' $$(^:%='%') $(LibPaths:%=$(LibPathFlag)%) $$(LibPaths_$(1):%=$(LibPathFlag)%) $(Libs:%=$(LibraryFlag)%) $$(Libs_$(1):%=$(LibraryFlag)%)

$(foreach file,$(SRC),$(eval $(call CompileObject,$(1),$(file))))

# Include compiler generated dependency files
-include $$(Objects_$(1):.o=.d)

endef

$(foreach mode,$(Modes),$(eval $(call TargetTemplate,$(mode))))
