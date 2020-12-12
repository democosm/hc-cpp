# Include default make config
include $(PROJBASEDIR)/src/config.gmk

# Target to build all applications
default:
	$(MAKE) -C $(SOURCE_DIR)/app/hccli
	$(MAKE) -C $(SOURCE_DIR)/app/hcquery
	$(MAKE) -C $(SOURCE_DIR)/app/hcxml
	$(MAKE) -C $(SOURCE_DIR)/app/msgsrv
	$(MAKE) -C $(SOURCE_DIR)/app/pisrv
	$(MAKE) -C $(SOURCE_DIR)/app/scratchsrv

# Target to clean all libraries and applications
clean:
	$(MAKE) clean -C $(SOURCE_DIR)/lib/$(TGTOS)
	$(MAKE) clean -C $(SOURCE_DIR)/lib/bus
	$(MAKE) clean -C $(SOURCE_DIR)/lib/common
	$(MAKE) clean -C $(SOURCE_DIR)/lib/drv
	$(MAKE) clean -C $(SOURCE_DIR)/lib/hc
	$(MAKE) clean -C $(SOURCE_DIR)/app/hccli
	$(MAKE) clean -C $(SOURCE_DIR)/app/hcquery
	$(MAKE) clean -C $(SOURCE_DIR)/app/hcxml
	$(MAKE) clean -C $(SOURCE_DIR)/app/msgsrv
	$(MAKE) clean -C $(SOURCE_DIR)/app/pisrv
	$(MAKE) clean -C $(SOURCE_DIR)/app/scratchsrv

# Target to install headers, libraries and applications
install:
	@echo "Copying header files to /tmp/hc/include"
	@mkdir -p /tmp/hc/include
	@cp $(SOURCE_DIR)/lib/$(TGTOS)/*.hh /tmp/hc/include
	@cp $(SOURCE_DIR)/lib/bus/*.hh /tmp/hc/include
	@cp $(SOURCE_DIR)/lib/common/*.hh /tmp/hc/include
	@cp $(SOURCE_DIR)/lib/drv/*.hh /tmp/hc/include
	@cp $(SOURCE_DIR)/lib/hc/*.hh /tmp/hc/include
	@echo "Creating archive in /tmp/hc/lib"
	@mkdir -p /tmp/hc/lib
	@ar -r -c -s /tmp/hc/lib/libhc.a $(TARGET_DIR)/lib/bus/*.o $(TARGET_DIR)/lib/common/*.o $(TARGET_DIR)/lib/drv/*.o $(TARGET_DIR)/lib/hc/*.o $(TARGET_DIR)/lib/$(TGTOS)/*.o
	@echo "Copying applications to /tmp/hc/bin"
	@mkdir -p /tmp/hc/bin
	@cp $(TARGET_DIR)/bin/* /tmp/hc/bin
