####### ANAGRAPHIC SECTION ######
ID = 864234
Name = SIMONE
Surname = CARRIERO
Whoami = $(ID)_$(Name)_$(Surname)
#####################################################


bridges_libs_path	 := ./library_bridges
quadrature_libs_path := ./quadrature_rules
examples_path		 := ./example

lib_interpolating_functions	:= ./interpolating_functions
lib_mesh_bridge				:= $(bridges_libs_path)/mesh_bridge
lib_plugin_loader			:= ./plugin_loader
lib_mesh_quadrature			:= $(quadrature_libs_path)/mesh_quadrature
lib_sandia_quadrature		:= $(quadrature_libs_path)/sandia_quadrature
lib_refine_binary			:= ./refine_binary

example_list := example1 \
				example2 \
				example3 \
				example4 \

examples := $(subst example,$(examples_path)/example,$(example_list))

test := ./test

lib_sub_dirs := $(bridges_libs_path) \
				$(quadrature_libs_path)

libraries := $(lib_interpolating_functions) \
			 $(lib_mesh_bridge) \
			 $(lib_plugin_loader) \
			 $(lib_mesh_quadrature) \
			 $(lib_sandia_quadrature) \
			 $(lib_refine_binary)

binaries :=	$(examples) \
			$(test)

docdir := ./doc

.PHONY: all $(binaries) $(libraries)
all: $(libraries)

.PHONY: navigate
navigate: $(libraries) $(binaries)

.PHONY: example_all
example_all : $(example_list)

define ExampleDep
$(1) : $(examples_path)/$(1)
endef

.PHONY: $(example_list)
$(foreach example,$(example_list),$(eval $(call ExampleDep,$(example))))

.PHONY: tests
tests: $(test)

$(binaries): $(libraries)
	$(MAKE) --directory=$@ init
	$(MAKE) -j3 --directory=$@ $(TARGET)

$(libraries):
	$(MAKE) --directory=$@ init
	$(MAKE) -j3 --directory=$@ $(TARGET)

#insert here any dependency between libraries

testdir := ./test

.PHONY: check
check: tests
	@if [ -f "$(testdir)/test.log" ] ; then \
		echo Removing previous "test.log"; \
		rm $(testdir)/test.log; \
	fi
	@cd $(testdir); \
	./bin/test;

.PHONY: install
install:
	$(MAKE) navigate TARGET=install

.PHONY: uninstall
uninstall:
	$(MAKE) navigate TARGET=uninstall

.PHONY: clean
clean:
	$(MAKE) navigate TARGET=clean
	rm -f -r $(docdir)/*

.PHONY: doc
doc:
	@mkdir -p $(docdir)
	doxygen Doxyfile

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
