# Include default make config
include $(PROJBASEDIR)/src/config.mk

# Include default make rules
include $(PROJBASEDIR)/src/rules.mk

# Create a list of all apps
APPLIST = $(sort $(dir $(wildcard $(PROJBASEDIR)/src/app/*/)))
LIBLIST = $(sort $(dir $(wildcard $(PROJBASEDIR)/src/lib/*/)))

default:: $(APPLIST)
	@echo "Building app $<"
	@cd $< && $(MAKE)

clean::
	@$(foreach dir,$(APPLIST),$(MAKE) -C $(dir) clean;)
	@$(foreach dir,$(LIBLIST),$(MAKE) -C $(dir) clean;)
