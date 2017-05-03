####### ANAGRAPHIC SECTION ######
MATRICOLA = 774857
NAME = SIMONE
SURNAME = CARRIERO
WHOAMI = $(MATRICOLA)_$(NAME)_$(SURNAME)
#####################################################


bridges_libs_path		:= ./library_bridges
quadrature_libs_path	:= ./quadrature_rules

lib_interpolating_functions	:= ./interpolating_functions
lib_libmesh_bridge				:= $(bridges_libs_path)/libmesh_bridge
lib_plugin_loading				:= ./plugin_loading
lib_libmesh_quadrature			:= $(quadrature_libs_path)/libmesh_quadrature
lib_sandia_quadrature			:= $(quadrature_libs_path)/sandia_quadrature
lib_refine_binary					:= ./refine_binary
example	:= ./example
test		:= ./test

lib_sub_dirs := $(bridges_libs_path) \
					 $(quadrature_libs_path)

libraries	:= $(lib_interpolating_functions) \
					$(lib_libmesh_bridge) \
					$(lib_plugin_loading) \
					$(lib_libmesh_quadrature) \
					$(lib_sandia_quadrature) \
					$(lib_refine_binary)

binaries	:= $(example) \
				$(test)


.PHONY: all $(binaries) $(libraries)
all: $(libraries) $(binaries)

$(binaries): $(libraries)
	$(MAKE) -j --directory=$@ $(TARGET)

$(libraries):
	$(MAKE) -j --directory=$@ $(TARGET)

#inserire qui eventuali dipendenze fra le librerie

.PHONY: install
install:
	$(MAKE) TARGET=install

.PHONY: uninstall
uninstall:
	$(MAKE) TARGET=uninstall

.PHONY: clean
clean:
	$(MAKE) TARGET=clean

.PHONY: tree
tree: $(subst .,$(WHOAMI),$(libraries))

$(subst .,$(WHOAMI),$(libraries)):
	mkdir -p $(dir $@)

.PHONY:package
package: clean
	@if [ -e $(WHOAMI) ] ; then \
		echo $(WHOAMI) exists! cleaning... \
		; rm -rf $(WHOAMI)/* \
	; else \
		echo $(WHOAMI) does not exist! creating... \
		; mkdir $(WHOAMI) \
	; fi
	@$(MAKE) tree
	@rm -f $(WHOAMI).zip
	@echo ... preparing package inputs...
	@cp Makefile $(WHOAMI)
	@cp Makefile.inc $(WHOAMI)
	@cp *.sh $(WHOAMI)
	@for d in $(binaries) $(libraries) \
	; do \
		cp -r $$d $(WHOAMI)/`dirname $$d` \
	; done
	@echo ... creating zipfile...
	@zip -q -r $(WHOAMI).zip $(WHOAMI)
	@echo ... removing $(WHOAMI)...
	@rm -rf $(WHOAMI)
	@echo ... done!
