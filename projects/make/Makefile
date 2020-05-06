## Copyright (c) 2015 Ahmad Fatoum
## Copyright (c) 2020 Jakub Vanek

############################
# Workspace-level makefile #
############################

ifeq ($(app),)
  $(error please specify app=...)
endif

_default:
	@$(MAKE) -C $(app)

$(MAKECMDGOALS):
	@$(MAKE) -C $(app) $@

.PHONY: $(MAKECMDGOALS)
