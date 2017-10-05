# Mode specific values are added after the mode agnostic values.
# The generated binary is stored as $(ObjDir)/$(LIBNAME)_$(buildmode)
# except for the Release binary. It is named $(ObjDir)/$(LIBNAME).
# You can change the location by changing macro TargetFilename.
# The object files are stored in directory $(ObjDir)/$(buildmode)/$(LIBNAME)
# You can change the location by changing ObjectFilename
# Variable "Modes" contains the list of build modes. Add your own mode with "Modes += mymode" if you like and add
# additional compiler and linker flags with "CXXFlags_mymode := " and "LFlags_mymode := ".

#########################
# LIBNAME Specific Part #
#########################
LIBNAME		:= mesh_quadrature
Modes			:= Debug Release
#LibType		:= static
LibType		:= dynamic
MajLibVers	:= 1
MinLibVers	:= 0
LibVers		:= $(MajLibVers).$(MinLibVers)

BaseDir 		= ../..
# I include the external libraries paths
-include $(BaseDir)/Makefile.inc

#DIRECTORY TREE
InstallDir	= $(BaseDir)/lib
CurrentDir	= $(CURDIR)
IncludeDir	= $(CurrentDir)/include
SrcDir		= $(CurrentDir)/src

# Directory where target binary is created and object files
LibDir		= $(CurrentDir)/lib
ObjDir		= $(CurrentDir)/obj

ObjDirs	= $(ObjDir)
LibDirs	= $(LibDir)
Dirs		= $(ObjDirs) $(LibDirs)


RefineDir		:= $(BaseDir)/refine_binary
RefineInclude	:= $(RefineDir)/include

# Directory where target binary is created and object files
ObjDir				:= $(ObjDir)
LibDir				:= $(LibDir)
# Search Path For Include Files (InclPaths is included before mode specific InclPaths_MODE)
InclPaths			:= $(IncludeDir) $(RefineInclude) $(LibMeshInclude) $(EigenInclude) $(GetPotInclude)
InclPaths_Debug	:=
InclPaths_Release	:=
# Defined values used by CPP preprocessor (Defines is included before mode specific Defines_MODE)
Defines				:= $(GlobalDefines)
Defines_Debug		:= DEBUG _GLIBCXX_DEBUG
Defines_Release	:= RELEASE
# Compiler specific flags (CXXFlags is included before mode specific CXXFlags_MODE)
CXXFlags				:= -march=native -Wall -Wextra
CXXFlags_Debug		:= -g -std=gnu++11 -O2 -felide-constructors -funroll-loops -fstrict-aliasing -Wdisabled-optimization -Wno-variadic-macros -fopenmp -DNDEBUG
CXXFlags_Release	:= -O3 -std=c++14


ifeq ($(LibType), static)
CXXFlags		+= -fPIC
LibSuffix	:= .a
LD				:= ar
# Linker specific flags (LFlags is included before mode specific LFlags_MODE)
LFlags			:= -r
LFlags_Debug	:=
LFlags_Release	:=
else
CXXFlags		+= -fPIC -rdynamic
LibSuffix	:= .so.$(LibVers)
LD				:= g++
# List of Library Names (Libs is included before mode specific Libs_MODE)
Libs				:= pthread
Libs_Debug		:= mesh_dbg
Libs_Release	:= mesh_opt
# Search Paths for Libraries (LibPaths is included before mode specific LibPaths_MODE)
LibPaths				:= $(LibMeshDir)
LibPaths_Debug		:=
LibPaths_Release	:=
# Linker specific flags (LFlags is included before mode specific LFlags_MODE)
LFlags			:= -shared -Wl,-rpath,$(LibMeshDir)
LFlags_Debug	:= -g -Wl,-soname,lib$(LIBNAME)_Debug.so.$(MajLibVers)
LFlags_Release	:= -O3 -Wl,-soname,lib$(LIBNAME).so.$(MajLibVers)
endif


# Source files
SRC := $(notdir $(wildcard $(SrcDir)/*.cpp))

# Configuration files
conf_files := $(IncludeDir)/$(LIBNAME).conf


##########################
# Compiler Specific Part #
##########################
IncPathFlag := -I
DefineFlag  := -D
LibraryFlag := -l
LibPathFlag := -L
CXX         := g++


#########################
# Generic Template Part #
#########################
all: $(Modes)

.PHONY:init
init: $(conf_files) $(Modes:%=init_%)

$(conf_files):
	@test -f $@ || (echo creating $@ ; cp $(BaseDir)/default_$(notdir $@) $(IncludeDir)/$(notdir $@))

.PHONY:install
install:all $(Modes:%=install_%)
	@echo Done!

$(Modes:%=install_%):
	@echo installing $(subst install_,,$@)
	cp $(LibDir)/lib$(LIBNAME)$(subst _install,,$(subst _Release,,_$@))$(LibSuffix) $(InstallDir)
ifeq ($(LibType), dynamic)
	cd $(InstallDir); \
	pwd; \
	ln -s -f ./lib$(LIBNAME)$(subst _install,,$(subst _Release,,_$@))$(LibSuffix) \
				./lib$(LIBNAME)$(subst _install,,$(subst _Release,,_$@)).so.$(MajLibVers); \
	ln -s -f ./lib$(LIBNAME)$(subst _install,,$(subst _Release,,_$@))$(LibSuffix) \
				./lib$(LIBNAME)$(subst _install,,$(subst _Release,,_$@)).so; \
	cd $(CurrentDir)
endif

.PHONY:uninstall
uninstall:$(Modes:%=uninstall_%)
	@echo Done!

$(Modes:%=uninstall_%):
	@echo uninstalling $(subst uninstall_,,$@)
ifeq ($(LibType), dynamic)
	rm -f $(InstallDir)/lib$(LIBNAME)$(subst _uninstall,,$(subst _Release,,_$@)).so
	rm -f $(InstallDir)/lib$(LIBNAME)$(subst _uninstall,,$(subst _Release,,_$@)).so.$(MajLibVers)
endif
	rm -f $(InstallDir)/lib$(LIBNAME)$(subst _uninstall,,$(subst _Release,,_$@))$(LibSuffix)

.PHONY:dir_tree
dir_tree: $(Dirs)

$(Dirs):
	@test -d $@ || (echo creating $@ ; mkdir -p $@)


clean: $(LIBNAME)_clean $(Modes:%=clean_%)
	@echo Done!

$(LIBNAME)_clean:
	@echo cleaning $(LIBNAME)
	@rm -rf $(LibDir)/*

$(Modes:%=clean_%):
	@echo cleaning $(ObjDir)/$(subst clean_,,$@)
	@rm -rf $(ObjDir)/$(subst clean_,,$@)

$(Modes:%=init_%): dir_tree
	@mkdir -p $(ObjDir)/$(subst init_,,$@)

define TargetFilename
$(LibDir)/lib$(LIBNAME)$(subst _Release,,_$(1))$(LibSuffix)
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
CXX_$(1) := $(CXX) $(Defines:%=$(DefineFlag)%) $$(Defines_$(1):%=$(DefineFlag)%) $(InclPaths:%=$(IncPathFlag)%) $$(InclPaths_$(1):%=$(IncPathFlag)%) $(CXXFlags) $$(CXXFlags_$(1))

Objects_$(1)  := $(foreach file,$(SRC),$(call ObjectFilename,$(1),$(file)))

$(1):	init_$(1) $(call TargetFilename,$(1))

$(call TargetFilename,$(1)): $$(Objects_$(1))
	@echo $(LD): generating '$$(notdir $$@)'
	$(LD) $(LFlags) $$(LFlags_$(1)) -o $(LibDir)/'$$(notdir $$@)' $$(^:%='%') $(LibPaths:%=$(LibPathFlag)%) $$(LibPaths_$(1):%=$(LibPathFlag)%) $(Libs:%=$(LibraryFlag)%) $$(Libs_$(1):%=$(LibraryFlag)%)
ifeq ($(LibType), dynamic)
	cd $(LibDir); \
	pwd; \
	ln -s -f $$(notdir $$@) $$(subst $(LibSuffix),.so.$(MajLibVers),$$(notdir $$@)); \
	ln -s -f $$(notdir $$@) $$(subst $(LibSuffix),.so,$$(notdir $$@)); \
	cd $(CurrentDir)
endif

$(foreach file,$(SRC),$(eval $(call CompileObject,$(1),$(file))))

# Include compiler generated dependency files
-include $$(Objects_$(1):.o=.d)

endef

$(foreach mode,$(Modes),$(eval $(call TargetTemplate,$(mode))))
