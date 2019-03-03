# Target - Default targets depending on package type
ifeq ($(DIR_TYPE), lib)
  default: $(TARGET_DIR)/lib/lib$(DIR_NAME).a
else
  ifeq ($(DIR_TYPE), app)
    default: $(TARGET_DIR)/bin/$(DIR_NAME)
  else
    default::
	@echo "Unrecognized package type (must be 'lib' or 'app')"
  endif
endif

# Target - Application
$(TARGET_DIR)/bin/$(DIR_NAME): $(OBJECT_FILES) $(LIBRARY_REDIR)
	@echo "APP: $@"
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) $(CG_START) $(OBJECT_FILES) $(LIBRARY_FILES) $(CG_END) $(STDLIBLIST) -o $@

# Target - Library archive
$(TARGET_DIR)/lib/lib$(DIR_NAME).a: $(OBJECT_FILES)
	@echo "LIB: $@"
	@mkdir -p $(dir $@)
	@$(AR) -r -c -s $@ $^

# Rule - Generate object files from C++ source
$(TARGET_DIR)/%.cc.o: $(SOURCE_DIR)/%.cc
	@echo "OBJ: $@"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CFLAGS) -c $< -o $@

# Rule - Generate dependencies files from C++ source
$(TARGET_DIR)/%.cc.dep: $(SOURCE_DIR)/%.cc
	@echo "DEP: $@"
	@mkdir -p "$(dir $@)"
	@touch $@
	@$(CC) -M $(CFLAGS) $< | sed -e 's;^\([^ ]\);$(dir $@)\1;' | sed -e 's;\.o:;\.cc\.o:;' > $@

# Target - Library redirect (need this so we can move to the lib directory and build when invoking make from an app directory)
redir_%:
	@echo "LIB (REDIRECT): $@"
	@cd $(subst redir_,$(SOURCE_DIR)/lib/,$@) && $(MAKE)

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
