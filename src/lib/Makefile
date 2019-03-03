# Include default make config
include $(PROJBASEDIR)/src/config.mk

# Default target
default:
	@cd $(TGTOS) && $(MAKE)
	@cd common && $(MAKE)
	@cd hc && $(MAKE)

# Install target (put all headers and master archive in /tmp/hc for now)
install:
	@echo "Copying header files to /tmp/hc/include"
	@mkdir -p /tmp/hc/include
	@cd $(TGTOS) && cp *.hh /tmp/hc/include
	@cd common && cp *.hh /tmp/hc/include
	@cd hc && cp *.hh /tmp/hc/include
	@echo "Creating archive in /tmp/hc/lib"
	@mkdir -p /tmp/hc/lib
	@ar -r -c -s /tmp/hc/lib/libhc.a $(LIBDSTDIR)/common/obj/*.o $(LIBDSTDIR)/hc/obj/*.o $(LIBDSTDIR)/$(TGTOS)/obj/*.o
