# Mode specific values are added after the mode agnostic values.
# The generated binary is stored as $(ObjDir)/$(EXENAME)_$(buildmode)
# except for the Opt binary. It is named $(ObjDir)/$(EXENAME).
# You can change the location by changing macro TargetFilename.
# The object files are stored in directory $(ObjDir)/$(buildmode)/$(EXENAME)
# You can change the location by changing ObjectFilename
# Variable "Modes" contains the list of build modes. Add your own mode with "Modes += mymode" if you like and add
# additional compiler and linker flags with "CXXFlags_mymode := " and "LFlags_mymode := ".

EXENAME	:= test
Modes		:= Opt Debug

BaseDir = ..
# I include the external libraries paths
-include $(BaseDir)/Makefile.inc

#DIRECTORY TREE
CurrentDir	= $(CURDIR)
RootDir		= $(CurrentDir)/$(BaseDir)
IncludeDir	= $(CurrentDir)/include
SrcDir		= $(CurrentDir)/src

# Directory where target binary is created and object files
ObjDir	= $(CurrentDir)/obj
BinDir	= $(CurrentDir)/bin

# Directory where results are stored
ResultsDir	= $(CurrentDir)/results

ObjDirs		= $(ObjDir)
BinDirs		= $(BinDir)
ResultsDirs	= $(ResultsDir)
Dirs			= $(ObjDirs) $(BinDirs) $(ResultsDirs)


DynamicLibraries =	\
							$(RootDir)/library_bridges/mesh_bridge \
							$(RootDir)/refine_binary \
							$(RootDir)/interpolating_functions \
							$(RootDir)/plugin_loader \

RunTimeLibraries =	\
							$(RootDir)/quadrature_rules/mesh_quadrature \
							$(RootDir)/quadrature_rules/sandia_quadrature \

StaticLibraries =	\


LinkedLibraries =	$(DynamicLibraries) $(StaticLibraries)


#StaticLibsFiles are linked and the executable depends on them
#DynamicLibsFiles are linked, but they are not in the dependencies
#Other plugins are not linked, since they are run at runtime


define LibFileName
$(2)/lib/lib$(notdir $(2))$(subst _Opt,,_$(1))$($(3)LibsExt)
endef

define AddLib
$(3)LibsFiles_$(1) += $(call LibFileName,$1,$2,$3)
endef

define DefineLibsFiles
$(foreach lib,$($(1)Libraries),$(eval $(call AddLib,$(2),$(lib),$(1))))
endef


#The paths where the loader can find dynamically linked libraries
DynamicLibsPath := $(patsubst %,%/lib,$(DynamicLibraries))

#The paths where the loader can find my plugins upon dlopen call
RunTimeLibsPath := $(patsubst %,%/lib,$(RunTimeLibraries))


Includes := $(IncludeDir) \
				$(patsubst %,%/include,$(LinkedLibraries))

# Search Path For Include Files (InclPaths is included before mode specific InclPaths_MODE)
InclPaths		:=	$(Includes) $(LibMeshInclude) $(EigenInclude) $(GTestInclude) $(GetPotInclude) $(MuParserInclude) $(SandiaRuleInclude)
InclPaths_Debug:=
InclPaths_Opt	:=
# Defined values used by CPP preprocessor (Defines is included before mode specific Defines_MODE)
Defines			:= $(GlobalDefines)
Defines_Debug	:= DEBUG _GLIBCXX_DEBUG
Defines_Opt		:= OPT
# Compiler specific flags (CXXFlags is included before mode specific CXXFlags_MODE)
CXXFlags			:= -march=native -Wall -Wextra
CXXFlags_Debug	:= -g -std=gnu++11 -felide-constructors -funroll-loops -fstrict-aliasing -Wdisabled-optimization -Wno-variadic-macros -DNDEBUG
CXXFlags_Opt	:= -std=c++14 -O3

# Linker specific flags (LFlags is included before mode specific LFlags_MODE)
LFlags			:= $(DynamicLibsPath:%=-Wl,-rpath,%) $(RunTimeLibsPath:%=-Wl,-rpath,%) -Wl,-rpath,$(LibMeshDir)
LFlags_Debug	:=
LFlags_Opt		:= 


LibMeshLib		:= mesh_opt
LibMeshLibDebug:= mesh_dbg

GTestLib	:= gtest

# List of Library Names (Libs is included before mode specific Libs_MODE)
Libs			:= $(GTestLib) pthread
Libs_Debug	:= $(LibMeshLibDebug)
Libs_Opt		:= $(LibMeshLib)

# Search Paths for Libraries (LibPaths is included before mode specific LibPaths_MODE)
LibPaths			:= $(LibMeshDir) $(GTestDir)
LibPaths_Debug	:=
LibPaths_Opt	:=

LD := g++


# Source files
SRC := $(notdir $(wildcard $(SrcDir)/*.cpp))


##########################
# Compiler Specific Part #
##########################
IncPathFlag := -I
DefineFlag  := -D
LibraryFlag := -l
LibPathFlag := -L
CXX         := g++

# Executables
EXECUTABLES = $(BinDir)/$(EXENAME)

# Libraries
DynamicLibsExt	:= .so
StaticLibsExt	:= .a


#########################
# Generic Template Part #
#########################
all: $(Modes)

.PHONY:init
init:$(Modes:%=init_%)

.PHONY:dir_tree
dir_tree: $(Dirs)

.PHONY:install
#it does nothing, needed only by the Makefile in the RootDir directory

.PHONY:uninstall
#it does nothing, needed only by the Makefile in the RootDir directory

$(Dirs) :
	@test -d $@ || (echo creating $@ ; mkdir -p $@)

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
$(call ObjectFilename,$(1),$(2)): $(SrcDir)/$(2)
	@echo compiling: '$$<'
	$$(CXX_$(1)) -c -o '$$@' '$$<'
	$$(CXX_$(1)) '$$<' -MM -MF '$$@'.temp.dep
	sed -e"s#$(subst .cpp,.o,$(2))#$$@#" '$$@'.temp.dep > $(call DependFilename,$(1),$(file))
	rm '$$@'.temp.dep
endef

define TargetTemplate
$(eval $(call DefineLibsFiles,Dynamic,$(1)))
$(eval $(call DefineLibsFiles,Static,$(1)))

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
