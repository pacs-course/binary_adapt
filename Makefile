####### ANAGRAPHIC SECTION ######
ID = 864234
Name = SIMONE
Surname = CARRIERO
Whoami = $(ID)_$(Name)_$(Surname)
#####################################################


bridges_libs_path		:= ./library_bridges
quadrature_libs_path	:= ./quadrature_rules
examples_path			:= ./example

lib_interpolating_functions	:= ./interpolating_functions
lib_mesh_bridge					:= $(bridges_libs_path)/mesh_bridge
lib_plugin_loader					:= ./plugin_loader
lib_mesh_quadrature				:= $(quadrature_libs_path)/mesh_quadrature
lib_sandia_quadrature			:= $(quadrature_libs_path)/sandia_quadrature
lib_refine_binary					:= ./refine_binary
example	:= $(examples_path)/example1 $(examples_path)/example2
test		:= ./test

lib_sub_dirs := $(bridges_libs_path) \
					 $(quadrature_libs_path)

libraries	:= $(lib_interpolating_functions) \
					$(lib_mesh_bridge) \
					$(lib_plugin_loader) \
					$(lib_mesh_quadrature) \
					$(lib_sandia_quadrature) \
					$(lib_refine_binary)

binaries	:= $(example) \
				$(test)


.PHONY: all $(binaries) $(libraries)
all: $(libraries) $(binaries)

$(binaries): $(libraries)
	$(MAKE) --directory=$@ init
	$(MAKE) -j4 --directory=$@ $(TARGET)

$(libraries):
	$(MAKE) --directory=$@ init
	$(MAKE) -j4 --directory=$@ $(TARGET)

#inserire qui eventuali dipendenze fra le librerie

.PHONY: check
check:
	./Test_check.sh

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
tree: $(subst .,$(Whoami),$(libraries))

$(subst .,$(Whoami),$(libraries)):
	@mkdir -p $@

.PHONY:package
package:
	@if [ -e $(Whoami) ] ; then \
		echo $(Whoami) exists! cleaning... \
		; rm -rf $(Whoami)/* \
	; else \
		echo $(Whoami) does not exist! creating... \
		; mkdir $(Whoami) \
	; fi
	@echo ... creating directory tree ...
	@$(MAKE) tree
	@rm -f $(Whoami).zip
	@echo ... preparing package inputs...
	@echo adding . to package in $(Whoami)
	@./make_package.sh . $(Whoami)
	@for d in $(binaries) $(libraries) \
	; do \
		echo adding $$d to package in $(Whoami) \
		; ./make_package.sh $$d $(Whoami) \
	; done
	@echo ... creating zipfile...
	@zip -q -r $(Whoami).zip $(Whoami)
	@echo ... removing $(Whoami)...
	@rm -rf $(Whoami)
	@echo ... done!
