# Check for building from directory under app
ifeq ($(DIR_TYPE), app)
  # Target - Application
  $(TARGET_DIR)/bin/$(DIR_NAME): $(OBJECT_FILES) $(LIBRARY_FILES)
	@echo "APP: $(DIR_NAME)"
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(CG_START) $(OBJECT_FILES) $(LIBRARY_FILES) $(CG_END) $(STDLIBLIST) -o $@

  # Target - Library archive
  $(TARGET_DIR)/lib/lib%.a:
	@echo "LIB: $(subst $(TARGET_DIR)/lib/lib,,$(subst .a,,$@))"
	cd $(subst $(TARGET_DIR)/lib/lib,$(SOURCE_DIR)/lib/,$(subst .a,,$@)) && $(MAKE)
endif

# Check for building from directory under lib
ifeq ($(DIR_TYPE), lib)
  # Target - Library archive
  $(TARGET_DIR)/lib/lib$(DIR_NAME).a: $(OBJECT_FILES)
	@echo "LIB: $(DIR_NAME)"
	@mkdir -p $(dir $@)
	$(AR) -r -c -s $@ $^
endif

# Rule - Generate object files from C++ source
$(TARGET_DIR)/%.cc.o: $(SOURCE_DIR)/%.cc
	@echo "OBJ: $@"
	@mkdir -p "$(dir $@)"
	$(CXX) $(CFLAGS) -MM -MT $@ -MF $(subst .o,.d,$@) $<
	$(CXX) $(CFLAGS) -c $< -o $@

# Target - Show make system variables
vars:
	@echo "PROJBASEDIR = $(PROJBASEDIR)"
	@echo "TGTPROC = $(TGTPROC)"
	@echo "TGTOS = $(TGTOS)"
	@echo "TGTBUILD = $(TGTBUILD)"
	@echo "CURRENT_DIR = $(CURRENT_DIR)"
	@echo "DIR_NAME = $(DIR_NAME)"
	@echo "DIR_TYPE = $(DIR_TYPE)"
	@echo "SOURCE_DIR = $(SOURCE_DIR)"
	@echo "TARGET_ARCH = $(TARGET_ARCH)"
	@echo "TARGET_DIR = $(TARGET_DIR)"
	@echo "SOURCE_FILES = $(SOURCE_FILES)"
	@echo "OBJECT_FILES = $(OBJECT_FILES)"
	@echo "DEPEND_FILES = $(DEPEND_FILES)"
	@echo "LIBRARY_NAMES = $(LIBRARY_NAMES)"
	@echo "LIBRARY_FILES = $(LIBRARY_FILES)"
	@echo "STDLIBLIST = $(STDLIBLIST)"
	@echo "CC = $(CC)"
	@echo "CXX = $(CXX)"
	@echo "AR = $(AR)"
	@echo "CFLAGS = $(CFLAGS)"

# Include the dependencies files if appropriate
ifeq (,$(findstring vars,$(MAKECMDGOALS)))
  -include $(DEPEND_FILES)
endif
