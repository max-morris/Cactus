# /*@@
#   @file      Makefile
#   @date      Sun Jan 17 22:26:05 1999
#   @author    Tom Goodale
#   @desc 
#   gnu Makefile for the CCTK.
#
# WARNING: This makefile may not function with "make".  Errors like
# make: file `Makefile' line 36: Must be a separator (: or ::) for rules (bu39)
# mean you should have used gmake.  gmake is available free
# from ftp://prep.ai.mit.edu/ and should be installed on all production
# systems.
#
# For information on how to use this makefile, type
# gmake help.
#
#   
#   @enddesc 
#   @version $Id: Makefile,v 1.13 1999-02-14 20:07:59 allen Exp $
# @@*/

# Comment this out if you want to see what's going on.
.SILENT:

# Various auxilary programs
PERL = perl
SETUP    = lib/make/setup_configuration.pl
NEWTHORN = lib/make/new_thorn.pl

# Dividers to make the screen output slightly nicer
DIVEL   =  __________________
DIVIDER =  $(DIVEL)$(DIVEL)$(DIVEL)$(DIVEL)

# Work out where we are
export CCTK_HOME := $(shell pwd)

# Work out which configurations are available
CONFIGURATIONS = $(patsubst configs/%,%,$(wildcard configs/*))

# Default target does nothing. 
# In principle should set up a default based upon uname or something.
.PHONY:default 

default: new_setup
	@echo $(DIVIDER)
ifeq ($(words $(CONFIGURATIONS)), 1)
	@echo Please use $(MAKE) $(CONFIGURATIONS) 
else
	@echo Known configurations are: $(CONFIGURATIONS)
	@echo Please use $(MAKE) \<configuration\>
endif
	@echo $(DIVIDER)

# If there are no configurations, call the setup program.
.PHONY: new_setup

new_setup:
ifeq ($(strip $(CONFIGURATIONS)),)
	@echo $(DIVIDER)
	@echo Setting up cctk
	$(PERL) $(SETUP)
	@echo $(DIVIDER)
	@echo You are now ready to build the CCTK.
	@echo This is done by $(MAKE) \<configuration\>
	@echo
	@echo Please ignore the error below !
	@echo $(DIVIDER)
	exit 2
endif

# Target to build a configuration
.PHONY: $(CONFIGURATIONS)

$(CONFIGURATIONS):
	cd configs/$@ 
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CCTK_HOME)/configs/$@ CCTK_HOME=$(CCTK_HOME)

# Clean target
.PHONY: distclean

distclean:
	@echo $(DIVIDER)
	@echo Deleting all your configurations !
	rm -rf configs
	@echo $(DIVIDER)

# Targets to make tags files

# Mark these targets phony to force an update when gmake TAGS is done.
.PHONY: TAGS tags

TAGS:
	@echo $(DIVIDER)
	@echo Updating the Emacs TAGS file
	find src toolkits \( -name '*.[chF]' -o -name '*.F77' \) \
          -exec etags --append --regex '/[a-z A-Z \t]*FORTRAN_NAME[^)]*/' {} \;
	perl -pi -e 's/(subroutine\s*)([a-zA-Z0-9_]+)/\1\L\2/g;' TAGS
	@echo $(DIVIDER)

tags:
	@echo $(DIVIDER)
	@echo Updating the vi tags file
	find src toolkits \( -name '*.[chF]' -o -name '*.F77' \) \
          -exec ctags --append {} \;
	perl -pi -e 's/(subroutine\s*)([a-zA-Z0-9_]+)/\1\L\2/g;' tags
	@echo $(DIVIDER)

# Make a new configuration
.PHONY: config

config:
	@echo $(DIVIDER)
	@echo Running the configuration program
	$(PERL) $(SETUP)
	@echo $(DIVIDER)

# The help system.
.PHONY: help

help:
	@echo $(DIVIDER)
	@echo This is the main makefile for the Cactus Computational Toolkit
ifeq ($(strip $(CONFIGURATIONS)),)
	@echo There are no configurations currently specified.
	@echo $(MAKE) with no arguments will run a setup script.
else
	@echo The following configurations are currently specified
	@echo $(CONFIGURATIONS)
	@echo To build a configuration run $(MAKE) followed by the name of a configuration.
	@echo To clean a configuration run $(MAKE) followed by the name of a configuration suffixed by -clean e.g. Linux-clean.
	@echo To delete a configuration run $(MAKE) followed by the name of a configuration suffixed by -delete e.g. Linux-delete.
endif
	@echo $(DIVIDER)
	@echo $(MAKE) also knows the following targets
	@echo
	@echo       TAGS      - creates an Emacs TAGS file
	@echo       tags      - creates a Vi TAGS file
	@echo       config    - creates a new configuration
	@echo       newthorn  - creates a new thorn
	@echo       distclean - deletes all existing configurations
	@echo       \<anything else\> prompts to create such a configuration.
	@echo $(DIVIDER)

# Clean a configuration
$(addsuffix -clean,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-clean=%)
	cd configs/$(@:%-clean=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CCTK_HOME)/configs/$(@:%-clean=%) CCTK_HOME=$(CCTK_HOME) clean
	@echo $(DIVIDER)

%-clean:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-clean=%) does not exist.
	@echo Cleaning aborted.



# Clean a configuration
$(addsuffix -delete,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Deleting configuration $(@:%-delete=%)
	cd configs ; rm -rf $(@:%-delete=%)  
	@echo $(DIVIDER)

%-delete:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-delete=%) does not exist.
	@echo Deletion aborted.

# Make a new thorn

.PHONY: newthorn
newthorn:
	@echo $(DIVIDER)
	@echo Creating a new thorn
	$(PERL) $(NEWTHORN);
	@echo $(DIVIDER)

# Last resort rule.  Assume it is the name of a configuration

%::
	@echo $(DIVIDER)
	echo Setup configuration $@ \(no\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xyes" -o "x$$yesno" = "xy" -o "x$$yesno" = "xYES" -o "x$$yesno" = "xY" ] ;\
	then  \
	echo Setting up new configuration $@; \
	$(PERL) $(SETUP) $@; \
	echo $(DIVIDER)   ;  \
	echo Use $(MAKE) $@ to build the configuration.; \
	else \
	echo Setup cancelled ;     \
	fi 
	@echo $(DIVIDER)

