# Include default make config
include $(PROJBASEDIR)/src/DefaultConfig

# Include default make rules
include $(PROJBASEDIR)/src/DefaultRules

# Create a list of all apps
APPLIST = $(sort $(dir $(wildcard $(PROJBASEDIR)/src/app/*/)))
LIBLIST = $(sort $(dir $(wildcard $(PROJBASEDIR)/src/lib/*/)))

default:: $(APPLIST)
	@echo "Building app $<"
	@cd $< && $(MAKE)
	@cd $(PROJBASEDIR)/src/lib && $(MAKE) install

clean::
	@$(foreach dir,$(APPLIST),$(MAKE) -C $(dir) clean;)
	@$(foreach dir,$(LIBLIST),$(MAKE) -C $(dir) clean;)