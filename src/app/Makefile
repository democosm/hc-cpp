# Include default make config
include $(PROJBASEDIR)/src/DefaultConfig

# Default target
default:
	@cd scratchsrv && $(MAKE)
	@cd hccli && $(MAKE)
	@cd hcquery && $(MAKE)
	@cd msgsrv && $(MAKE)
	@cd pisrv && $(MAKE)

# Install target (put all binaries in /tmp/hc for now)
install:
	@echo "Copying files to /tmp/hc/bin"
	@mkdir -p /tmp/hc/bin
	@cp $(BINDSTDIR)/* /tmp/hc/bin/
