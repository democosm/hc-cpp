# Remember current working directory
CURRENT_DIR = $(shell pwd)

# Determine package name and type
DIR_NAME = $(shell basename $(CURRENT_DIR))
DIR_TYPE = $(subst /$(DIR_NAME),,$(subst $(PROJBASEDIR)/src/,,$(CURRENT_DIR)))

# Add target OS library to link libraries if building an application
ifeq ($(DIR_TYPE), app)
  LIBRARY_NAMES += $(TGTOS)
endif

# Concatenated target architecture
TARGET_ARCH = $(TGTPROC)_$(TGTOS)_$(TGTBUILD)

# Source and destination directory
SOURCE_DIR = $(PROJBASEDIR)/src
TARGET_DIR = $(PROJBASEDIR)/dst/$(TARGET_ARCH)

# Determine list of source files
SOURCE_FILES = $(wildcard $(CURRENT_DIR)/*.cc)

# Determine list of object files
OBJECT_FILES = $(addsuffix .o,$(foreach sourcefile,$(SOURCE_FILES),$(subst $(SOURCE_DIR)/$(DIR_TYPE)/,$(TARGET_DIR)/$(DIR_TYPE)/,$(sourcefile))))

# Determine list of dependency files
DEPEND_FILES = $(addsuffix .dep,$(foreach sourcefile,$(SOURCE_FILES),$(subst $(SOURCE_DIR)/$(DIR_TYPE)/,$(TARGET_DIR)/$(DIR_TYPE)/,$(sourcefile))))

# Determine list of library files
LIBRARY_FILES = $(foreach libname,$(LIBRARY_NAMES),$(addprefix $(TARGET_DIR)/lib/lib,$(addsuffix .a,$(libname))))

# Determine list of library redirection targets
LIBRARY_REDIR = $(addprefix redir_,$(LIBRARY_NAMES))

# Variables used to build for different targets
ifeq ($(TGTPROC), $(shell uname -m))
  CC = gcc
  CXX = g++
  AR = ar
  CFLAGS = \
    -Wall \
    -I$(PROJBASEDIR)/src/lib/bus \
    -I$(PROJBASEDIR)/src/lib/common \
    -I$(PROJBASEDIR)/src/lib/drv \
    -I$(PROJBASEDIR)/src/lib/hc \
    -I$(PROJBASEDIR)/src/lib/$(TGTOS)
  STDLIBLIST = \
    -lpthread \
    -lssl \
    -lcrypto
  ifneq ($(shell uname -s), Darwin)
    CG_START = -Wl,--start-group
    CG_END = -Wl,--end-group
    STDLIBLIST += -lrt
  endif
endif

# Special options for special builds
ifeq ($(TGTBUILD), dbg)
  CFLAGS += -g -DDEBUG
else
  CFLAGS += -O3
endif
