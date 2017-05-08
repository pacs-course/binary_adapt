# Mode specific values are added after the mode agnostic values.
# The generated binary is stored as $(ObjDir)/$(EXENAME)_$(buildmode)
# except for the Opt binary. It is named $(ObjDir)/$(EXENAME).
# You can change the location by changing macro TargetFilename.
# The object files are stored in directory $(ObjDir)/$(buildmode)/$(EXENAME)
# You can change the location by changing ObjectFilename
# Variable "Modes" contains the list of build modes. Add your own mode with "Modes += mymode" if you like and add
# additional compiler and linker flags with "CXXFlags_mymode := " and "LFlags_mymode := ".

EXENAME	:= example
Modes		:= Opt Debug

#DIRECTORY TREE
BaseDir 		= $(THIS_DIR)/../..
CurrentDir	= $(PWD)
THIS_DIR		= .
INCLUDE_DIR = $(THIS_DIR)/include
SRC_DIR		= $(THIS_DIR)/src
BIN_DIR		= $(THIS_DIR)/bin
OBJ_DIR		= $(THIS_DIR)/obj

OBJ_DIRS	= $(OBJ_DIR)
BIN_DIRS	= $(BIN_DIR)
DIRS		= $(OBJ_DIRS) $(BIN_DIRS)

-include $(BaseDir)/Makefile.inc

RefineName		= refine_binary
RefineDir		= $(BaseDir)/refine_binary
RefineInclude	= $(RefineDir)/include
RefineLib		= $(RefineDir)/lib
RefineFile		= $(RefineLib)/lib$(RefineName).so
RefineDebug		= $(RefineLib)/lib$(RefineName)_Debug.so

BridgesDir			= $(BaseDir)/library_bridges
MeshBridgeName		= mesh_bridge
MeshBridgeDir		= $(BridgesDir)/libmesh_bridge
#MeshBridgeInclude	= $(MeshBridgeDir)/include
MeshBridgeLib		= $(MeshBridgeDir)/lib
#MeshBridgeFile		= $(wildcard $(MeshBridgeLib)/lib$(MeshBridgeName).so*)
#MeshBridgeDebug	= $(wildcard $(MeshBridgeLib)/lib$(MeshBridgeName)_Debug.so*)

QuadraturesDir				= $(BaseDir)/quadrature_rules
MeshQuadratureName		= mesh_quadrature
MeshQuadratureDir			= $(QuadraturesDir)/libmesh_quadrature
#MeshQuadratureInclude	= $(MeshQuadratureDir)/include
MeshQuadratureLib			= $(MeshQuadratureDir)/lib
#MeshQuadratureFile		= $(wildcard $(MeshQuadratureLib)/lib$(MeshQuadratureName).so*)
#MeshQuadratureDebug		= $(wildcard $(MeshQuadratureLib)/lib$(MeshQuadratureName)_Debug.so*)

FunctionsName		= my_functions
FunctionsDir 		= $(BaseDir)/interpolating_functions
#FunctionsInclude	= $(FunctionsDir)/include
FunctionsLib		= $(FunctionsDir)/lib
#FunctionsFile		= $(wildcard $(FunctionsLib)/lib$(FunctionsName).so*)
#FunctionsDebug		= $(wildcard $(FunctionsLib)/lib$(FunctionsName)_Debug.so*)

PluginLoaderName		= plugin_loader
PluginLoaderDir		= $(BaseDir)/plugin_loading
PluginLoaderInclude	= $(PluginLoaderDir)/include
PluginLoaderLib		= $(PluginLoaderDir)/lib
PluginLoaderFile		= $(wildcard $(PluginLoaderLib)/lib$(PluginLoaderName).so*)
PluginLoaderDebug		= $(wildcard $(PluginLoaderLib)/lib$(PluginLoaderName)_Debug.so*)

#StaticLibsFiles are linked and the executable depends on them
#DynamicLibsFiles are linked, but they are not in the dependencies
#Other plugins are not linked, since they are run at runtime
StaticLibsFiles_Opt	 := 
StaticLibsFiles_Debug := 

DynamicLibsFiles_Opt 	:= $(PluginLoaderFile) $(RefineFile)
DynamicLibsFiles_Debug	:= $(PluginLoaderDebug) $(RefineDebug)

#TODO : find a cleaner way to do this job

# Directory where target binary is created and object files
ObjDir := $(OBJ_DIR)
BinDir := $(BIN_DIR)

Includes := $(INCLUDE_DIR) \
				$(RefineInclude) \
				$(PluginLoaderInclude)

# Search Path For Include Files (InclPaths is included before mode specific InclPaths_MODE)
InclPaths			:=	$(Includes)
InclPaths_Debug	:=
InclPaths_Opt		:=
# Defined values used by CPP preprocessor (Defines is included before mode specific Defines_MODE)
Defines			:= $(GlobalDefines)
Defines_Debug	:= DEBUG _GLIBCXX_DEBUG
Defines_Opt		:= OPT
# Compiler specific flags (CXXFlags is included before mode specific CXXFlags_MODE)
CXXFlags			:= -march=native $(INCLUDE_FLAGS) -Wall -Wextra
CXXFlags_Debug	:= -g -std=gnu++11 -O2 -felide-constructors -funroll-loops -fstrict-aliasing -Wdisabled-optimization -Wno-variadic-macros -fopenmp -DNDEBUG
CXXFlags_Opt	:= -std=c++14 -O3

# Linker specific flags (LFlags is included before mode specific LFlags_MODE)
LFlags			:= 
LFlags_Debug	:=
LFlags_Opt		:= 

# List of Library Names (Libs is included before mode specific Libs_MODE)
Libs			:=
Libs_Debug	:=
Libs_Opt		:=

# Search Paths for Libraries (LibPaths is included before mode specific LibPaths_MODE)
LibPaths			:= #$(FunctionsLib) $(MeshQuadratureLib) $(MeshBridgeLib)
LibPaths_Debug	:=
LibPaths_Opt	:=

LD := g++


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

.PHONY:init
init:$(Modes:%=init_%)

.PHONY:dir_tree
dir_tree: $(DIRS)

.PHONY:install
#it does nothing, needed only by the Makefile in the BaseDir directory

.PHONY:uninstall
#it does nothing, needed only by the Makefile in the BaseDir directory

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

define DependFilename
$(ObjDir)/$(1)/$(subst .cpp,.d,$(2))
endef

define CompileObject
$(call ObjectFilename,$(1),$(2)): $(SRC_DIR)/$(2)
	@echo compiling: '$$<'
	$$(CXX_$(1)) -c -o '$$@' '$$<'
	$$(CXX_$(1)) '$$<' -MM -MF '$$@'.temp.dep
	sed -e"s#$(subst .cpp,.o,$(2))#$$@#" '$$@'.temp.dep > $(call DependFilename,$(1),$(file))
	rm '$$@'.temp.dep
endef

define TargetTemplate
CXX_$(1) := $(CXX) $(Defines:%=$(DefineFlag)%) $$(Defines_$(1):%=$(DefineFlag)%) $(InclPaths:%=$(IncPathFlag)%) $$(InclPaths_$(1):%=$(IncPathFlag)%) $(CXXFlags) $$(CXXFlags_$(1))

Objects_$(1) := $(foreach file,$(SRC),$(call ObjectFilename,$(1),$(file)))

$(1):	init_$(1) $(call TargetFilename,$(1))

$(call TargetFilename,$(1)): $$(Objects_$(1)) $(StaticLibsFiles_$(1))
	@echo $(LD): generating '$$(notdir $$@)'
	$(LD) $(LFlags) $$(LFlags_$(1)) -o $(BinDir)/'$$(notdir $$@)' $$(^:%='%') $$(DynamicLibsFiles_$(1)) $(LibPaths:%=$(LibPathFlag)%) $$(LibPaths_$(1):%=$(LibPathFlag)%) $(Libs:%=$(LibraryFlag)%) $$(Libs_$(1):%=$(LibraryFlag)%)

$(foreach file,$(SRC),$(eval $(call CompileObject,$(1),$(file))))

# Include compiler generated dependency files
-include $$(Objects_$(1):.o=.d)

endef

$(foreach mode,$(Modes),$(eval $(call TargetTemplate,$(mode))))
