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
#   @version $Id: Makefile,v 1.38 1999-07-05 15:39:17 goodale Exp $
# @@*/

# Make quietly unless told not to
ifneq ($(strip $(SILENT)),no)
.SILENT:
endif

# Stuff for parallel makes
# TJOBS is the number of thorns to compile in parallel
ifeq ($(strip $(TJOBS)), )
TJOBS = 1
endif

# FJOBS is the number of files within a thorn to compile in parallel
ifeq ($(strip $(FJOBS)), )
FJOBS = 1
endif

export TJOBS FJOBS

# End of parallel make stuff


# Set the options to pass to the setup script
ifneq ($(strip $(options)),)
SETUP_OPTIONS = -config_file=$(options)
else
SETUP_OPTIONS = 
endif

# Allow various options to be passed to the configure script

SETUP_ENV = 

ifdef CC
ifneq ($(strip $(origin CC)), default)
SETUP_ENV += CC=$(CC) ; export CC ;
endif
endif

ifdef F90
ifneq ($(strip $(origin F90)), default)
SETUP_ENV += F90=$(F90) ; export F90 ;
endif
endif

ifdef F77
ifneq ($(strip $(origin F77)), default)
SETUP_ENV += F77=$(F77) ; export F77 ;
endif
endif

ifdef LD
ifneq ($(strip $(origin LD)), default)
SETUP_ENV += LD=$(LD) ; export LD ;
endif
endif

ifdef CFLAGS
ifneq ($(strip $(origin CFLAGS)), default)
SETUP_ENV += CFLAGS=$(CFLAGS) ; export CFLAGS;
endif
endif

ifdef F90FLAGS
ifneq ($(strip $(origin F90FLAGS)), default)
SETUP_ENV += F90FLAGS=$(F90FLAGS) ; export F90FLAGS ;
endif
endif

ifdef F77FLAGS
ifneq ($(strip $(origin F77FLAGS)), default)
SETUP_ENV += F77FLAGS=$(F77FLAGS) ; export F90FLAGS ;
endif
endif

ifdef LDFLAGS
ifneq ($(strip $(origin LDFLAGS)), default)
SETUP_ENV += LDFLAGS=$(LDFLAGS) ; export LDFLAGS ;
endif
endif

ifdef REAL_PRECISION
ifneq ($(strip $(origin REAL_PRECISION)), default)
SETUP_ENV += REAL_PRECISION=$(REAL_PRECISION) ; export REAL_PRECISION ;
endif
endif

ifdef INTEGER_PRECISION
ifneq ($(strip $(origin INTEGER_PRECISION)), default)
SETUP_ENV += INTEGER_PRECISION=$(INTEGER_PRECISION) ; export INTEGER_PRECISION ;
endif
endif

# Various auxilary programs
PERL = perl
SETUP    = lib/make/setup_configuration.pl
NEWTHORN = lib/make/new_thorn.pl

# Dividers to make the screen output slightly nicer
DIVEL   =  __________________
DIVIDER =  $(DIVEL)$(DIVEL)$(DIVEL)$(DIVEL)

# Work out where we are
export CCTK_HOME := $(shell pwd | sed 's,^//\(.\)/,\1:/,' )


# Work out which configurations are available
CONFIGURATIONS = $(patsubst configs/%,%,$(wildcard configs/*))

# Default target does nothing. 
# Used to set up a default based upon uname or something.
.PHONY:default-target

default-target:
ifeq ($(strip $(CONFIGURATIONS)),)
	@echo $(DIVIDER)
	@echo No configurations defined. 
	@echo Please use \'$(MAKE) \<name\>\' to setup a configuration called \<name\>.
	@echo $(DIVIDER)
	@echo \'$(MAKE) help\' lists all $(MAKE) options.
else
ifeq ($(words $(CONFIGURATIONS)), 1)
	@echo Please use $(MAKE) $(CONFIGURATIONS) 
else
	@echo Known configurations are: $(CONFIGURATIONS)
	@echo Please use $(MAKE) \<configuration\>
endif
endif
	@echo $(DIVIDER)

# Target to build a configuration
.PHONY: $(CONFIGURATIONS)

$(CONFIGURATIONS):
	cd configs/$@ 
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CCTK_HOME)/configs/$@ CCTK_HOME=$(CCTK_HOME) -j $(TJOBS)

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
	rm -f TAGS ; touch TAGS 
	find src packages \( -name '*.[chCF]' -o -name '*.F77' -o -name '*.cc' \) \
          -exec etags -a {} \;
#	find src packages \( -name '*.[cChF]' -o -name '*.F77' -o -name '*.cc'\) \
#          -exec etags --append --regex '/[a-z A-Z \t]*FORTRAN_NAME[^)]*/' {} \;
	perl -pi -e 's/(subroutine\s*)([a-zA-Z0-9_]+)/\1\L\2/g;' TAGS
	@echo $(DIVIDER)

tags:
	@echo $(DIVIDER)
	@echo Updating the vi tags file
	rm -f tags ; touch tags 
	find src packages \( -name '*.[cChF]' -o -name '*.F77' -o -name '*.cc' \) \
          -exec ctags -a {} \;
	perl -pi -e 's/(subroutine\s*)([a-zA-Z0-9_]+)/\1\L\2/g;' tags
	sort tags > sortedtags ; mv sortedtags tags
	@echo $(DIVIDER)

# Make a new configuration with a default name
.PHONY: default

default:
	@echo $(DIVIDER)
	@echo Running the configuration program
	$(SETUP_ENV) $(PERL) -s $(SETUP) $(SETUP_OPTIONS)
	@echo $(DIVIDER)
	@echo You are now ready to build the CCTK.
	@echo This is done by $(MAKE) \<configuration\>
	@echo $(DIVIDER)

# The help system.
.PHONY: help

help:
	@echo $(DIVIDER)
	@echo This is the main makefile for the Cactus Computational Toolkit
ifeq ($(strip $(CONFIGURATIONS)),)
	@echo There are no configurations currently specified.
	@echo \'$(MAKE) \<name\>\' will run a setup script to setup a configuration called \'\<name\>\'.
else
	@echo The following configurations are currently specified
	@echo 
	@echo "  $(CONFIGURATIONS)"
	@echo $(DIVIDER)
	@echo "To build a configuration: "
	@echo "  run $(MAKE) followed by the name of a configuration."
	@echo $(DIVIDER)
	@echo There is a range of options available to act on a configuration.
	@echo These are activated by $(MAKE) \<conf-name\>-\<option\>
	@echo Valid options are
	@echo "  -clean       : to clean a configuration."
	@echo "                 (deletes all object and dependency files in "
	@echo "                  the configuration)."
	@echo "  -cleandeps   : to clean a configuration\'s dependency files."
	@echo "  -cleanobjs   : to clean a configuration\'s object files."
	@echo "  -realclean   : to restore a configuration to almost a new state. "
	@echo "                 (deletes all but the config-data directory "
	@echo "                  and the ThornList file)."
	@echo "  -delete      : to delete a configuration." 
	@echo "  -rebuild     : to rebuild a configuration." 
	@echo "                 (forces the CST to be rerun)."
	@echo "  -config      : to (re)configure a configuration. "
	@echo "                 (runs or reruns the configuration scripts)."
endif
	@echo $(DIVIDER)
	@echo $(MAKE) also knows the following targets
	@echo
	@echo "  TAGS      - creates an Emacs TAGS file."
	@echo "  tags      - creates a Vi TAGS file."
	@echo "  checkout  - checkout public packages/thorns."
	@echo "  default   - creates a new configuration with a default name."
	@echo "  newthorn  - creates a new thorn."
	@echo "  distclean - deletes all existing configurations."
	@echo "  testsuite - run the test program."
	@echo "  downsize  - remove non-essential files."
	@echo "  doc       - creates UserGuide.ps"
	@echo "  <anything else> prompts to create such a configuration."
	@echo $(DIVIDER)

# Clean a configuration

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -clean,$(CONFIGURATIONS)):

$(addsuffix -clean,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-clean=%)
	cd configs/$(@:%-clean=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CCTK_HOME)/configs/$(@:%-clean=%) CCTK_HOME=$(CCTK_HOME) clean
	@echo $(DIVIDER)

endif

%-clean:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-clean=%) does not exist.
	@echo Cleaning aborted.


# Clean just dependency files

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -cleandeps,$(CONFIGURATIONS)):

$(addsuffix -cleandeps,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-cleandeps=%)
	cd configs/$(@:%-cleandeps=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CCTK_HOME)/configs/$(@:%-cleandeps=%) CCTK_HOME=$(CCTK_HOME) cleandeps
	@echo $(DIVIDER)

endif

%-cleandeps:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-cleandeps=%) does not exist.
	@echo Cleaning dependencies aborted.

# Clean just object files

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -cleanobjs,$(CONFIGURATIONS)):

$(addsuffix -cleanobjs,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-cleanobjs=%)
	cd configs/$(@:%-cleanobjs=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CCTK_HOME)/configs/$(@:%-cleanobjs=%) CCTK_HOME=$(CCTK_HOME) cleanobjs
	@echo $(DIVIDER)

endif

%-cleanobjs:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-cleanobjs=%) does not exist.
	@echo Cleaning object files aborted.


# Clean away all produced files (doesn't delete ThornList)

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -realclean,$(CONFIGURATIONS)):

$(addsuffix -realclean,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-realclean=%)
	cd configs/$(@:%-realclean=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CCTK_HOME)/configs/$(@:%-realclean=%) CCTK_HOME=$(CCTK_HOME) realclean
	@echo $(DIVIDER)

endif

%-realclean:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-cleanreal=%) does not exist.
	@echo Cleaning aborted.

# Delete a configuration

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -delete,$(CONFIGURATIONS)):

$(addsuffix -delete,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Deleting configuration $(@:%-delete=%)
	cd configs ; rm -rf $(@:%-delete=%)  
	@echo $(DIVIDER)

endif

%-delete:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-delete=%) does not exist.
	@echo Deletion aborted.

# Rebuild a configuration

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -rebuild,$(CONFIGURATIONS)):

$(addsuffix -rebuild,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Rebuilding $(@:%-rebuild=%)
	if [ -r configs/$(@:%-rebuild=%)/config-data/make.thornlist ] ; then rm  configs/$(@:%-rebuild=%)/config-data/make.thornlist ; fi
	$(MAKE) $(@:%-rebuild=%)
endif

%-rebuild:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-rebuild=%) does not exist.
	@echo Rebuild aborted.

# Regenerate the compiled thorns list

ifneq ($strip($(CONFIGURATIONS)),) 
.PHONY $(addsuffix -thornlist,$(CONFIGURATIONS)):

$(addsuffix -thornlist,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Regenerating compiled ThornList $(@:%-thornlist=%)
	if [ -r configs/$(@:%-thornlist=%)/ThornList ] ; then rm configs/$(@:%-thornlist=%)/ThornList ; fi
	$(MAKE) $(@:%-thornlist=%)
endif

%-thornlist:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-thornlist=%) does not exist.
	@echo Regeneration of compiled ThornList aborted.

# Rerun the configuration script

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -config,$(CONFIGURATIONS)):

$(addsuffix -config,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	$(SETUP_ENV) $(PERL) -s $(SETUP) -reconfig=1 $(SETUP_OPTIONS) $(@:%-config=%); 
endif

%-config:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-config=%) does not exist.
	echo Setup configuration $(@:%-config=%) \(no\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xyes" -o "x$$yesno" = "xy" -o "x$$yesno" = "xYES" -o "x$$yesno" = "xY" ] ;\
	then  \
	echo Setting up new configuration $(@:%-config=%); \
	$(SETUP_ENV) $(PERL) -s $(SETUP) $(SETUP_OPTIONS) $(@:%-config=%); \
	echo $(DIVIDER)   ;  \
	echo Use $(MAKE) $(@:%-config=%) to build the configuration.; \
	else \
	echo Setup cancelled ;     \
	fi 
	@echo $(DIVIDER)

# Make a new thorn

.PHONY: newthorn
newthorn:
	@echo $(DIVIDER)
	@echo Creating a new thorn
	$(PERL) -s $(NEWTHORN);
	@echo $(DIVIDER)


# Run the testsuite

ifneq ($strip($(CONFIGURATIONS)),) 
.PHONY $(addsuffix -testsuite,$(CONFIGURATIONS)):

$(addsuffix -testsuite,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Running test suite $(@:%-thornlist=%)
	if [ -r configs/$(@:%-testsuite=%)/ThornList ] ; then $(PERL) lib/sbin/Runtest.pl $(@:%-testsuite=%) ; fi
endif

%-testsuite:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-testsuite=%) does not exist.
	@echo Test suite aborted.


# Checkout public thorns and packages

.PHONY: checkout
checkout:
	@echo $(DIVIDER)
	@echo Running package/thorn checkout script
	$(PERL) ./lib/sbin/checkout.pl


# Make the users manuals

.PHONY: doc
doc:
	@echo $(DIVIDER)
	@echo Creating user documentation UsersGuide.ps
	(cd doc/UsersGuide; latex UsersGuide.tex; cd $(CCTK_HOME); dvips doc/UsersGuide/UsersGuide.dvi -o UsersGuide.ps) ;
	@echo $(DIVIDER)



# Remove non-essential files

.PHONY: downsize
downsize:
	@echo $(DIVIDER)
	@echo Remove flesh and thorn documentation \(\no\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xyes" -o "x$$yesno" = "xy" -o "x$$yesno" = "xYES" -o "x$$yesno" = "xY" ] ;\
	then  \
	rm -rf doc; rm -rf packages/*/*/doc; \
	echo $(DIVIDER)   ;  \
	fi 
	@echo Remove thorn testsuites \(\no\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xyes" -o "x$$yesno" = "xy" -o "x$$yesno" = "xYES" -o "x$$yesno" = "xY" ] ;\
	then  \
	rm -rf packages/*/*/test; \
	echo $(DIVIDER)   ;  \
	fi 
	@echo Remove all configurations \(\no\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xyes" -o "x$$yesno" = "xy" -o "x$$yesno" = "xYES" -o "x$$yesno" = "xY" ] ;\
	then  \
	gmake distclean; \
	echo $(DIVIDER)   ;  \
	fi 

# Last resort rule.  Assume it is the name of a configuration

%::
	@echo $(DIVIDER)
	echo Setup configuration $@ \(no\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xyes" -o "x$$yesno" = "xy" -o "x$$yesno" = "xYES" -o "x$$yesno" = "xY" ] ;\
	then  \
	echo Setting up new configuration $@; \
	$(SETUP_ENV) $(PERL) -s $(SETUP) $(SETUP_OPTIONS) $@; \
	echo $(DIVIDER)   ;  \
	echo Use $(MAKE) $@ to build the configuration.; \
	else \
	echo Setup cancelled ;     \
	fi 
	@echo $(DIVIDER)

