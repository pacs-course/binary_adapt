####### ANAGRAPHIC SECTION ######
MATRICOLA = 774857
NAME = SIMONE
SURNAME = CARRIERO
WHOAMI = $(MATRICOLA)_$(NAME)_$(SURNAME)
#####################################################


lib_interpolating_functions	:= ./interpolating_functions
lib_libmesh_bridge				:= ./library_bridges/libmesh_bridge
lib_plugin_loading				:= ./plugin_loading
lib_libmesh_quadrature			:= ./quadrature_rules/libmesh_quadrature
lib_refine_binary					:= ./refine_binary
example								:= ./example
test									:= ./test

libraries	:= $(lib_interpolating_functions) $(lib_libmesh_bridge) $(lib_plugin_loading) $(lib_libmesh_quadrature) $(lib_refine_binary)
binaries	:= $(example) $(test)


.PHONY: all $(binaries) $(libraries)
all: $(libraries) $(binaries)

#$(binaries) $(libraries):
#	$(MAKE) --directory=$@ $(TARGET)

$(binaries): $(libraries)
	$(MAKE) --directory=$@ $(TARGET)

$(libraries):
	$(MAKE) --directory=$@ $(TARGET)

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

.PHONY:package
package: clean
	@if [ -e $(WHOAMI) ] ; then \
		echo $(WHOAMI) exists! cleaning... \
		; rm -rf $(WHOAMI)/* \
	; else \
		echo $(WHOAMI) does not exist! creating... \
		; mkdir $(WHOAMI) \
	; fi
	@rm -f $(WHOAMI).zip
	@echo ... preparing package inputs...
	@cp Makefile $(WHOAMI)
	@for d in $(binaries) $(libraries) \
	; do \
		cp -r $$d $(WHOAMI) \
	; done
	@echo ... creating zipfile...
	@zip -q -r $(WHOAMI).zip $(WHOAMI)
	@echo ... done!
