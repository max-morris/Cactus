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
# gmake help
#
#   
#   @enddesc 
#   @version $Id: Makefile,v 1.62 2000-01-03 12:25:07 allen Exp $
# @@*/

##################################################################################
# Version number
##################################################################################
CCTK_VERSION_MAJOR = 4
CCTK_VERSION_MINOR = 0
CCTK_VERSION_OTHER = b6
##################################################################################
CCTK_VERSION=$(CCTK_VERSION_MAJOR).$(CCTK_VERSION_MINOR).$(CCTK_VERSION_OTHER)
##################################################################################
export CCTK_VERSION_MAJOR CCTK_VERSION_MINOR CCTK_VERSION_OTHER CCTK_VERSION
##################################################################################

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

# Arrangement options

ifdef MPI
ifneq ($(strip $(origin MPI)), default)
SETUP_ENV += MPI=$(MPI) ; export MPI ;
endif
endif

# Debug options
ifdef DEBUG
ifneq ($(strip $(origin DEBUG)), default)
SETUP_ENV += DEBUG=$(DEBUG) ; export DEBUG ;
endif
endif



# Various auxilary programs
PERL = perl
SETUP    = lib/make/setup_configuration.pl
NEWTHORN = lib/make/new_thorn.pl
BUILD_ACTIVETHORNS = lib/sbin/BuildActiveThorns.pl

# Dividers to make the screen output slightly nicer
DIVEL   =  __________________
DIVIDER =  $(DIVEL)$(DIVEL)$(DIVEL)$(DIVEL)

# Work out where we are
export CCTK_HOME := $(shell pwd | sed 's,^//\(.\)/,\1:/,' )

# Work out where the configuration directory is
ifdef CACTUS_CONFIGS_DIR
CONFIGS_DIR = $(CACTUS_CONFIGS_DIR)
else
CONFIGS_DIR = $(CCTK_HOME)/configs
endif

export CONFIGS_DIR

# Work out which configurations are available
CONFIGURATIONS = $(patsubst $(CONFIGS_DIR)/%,%,$(wildcard $(CONFIGS_DIR)/*))

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
	cd $(CONFIGS_DIR)/$@ 
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CONFIGS_DIR)/$@ CCTK_HOME=$(CCTK_HOME) -j $(TJOBS)

# Clean target
.PHONY: distclean

distclean:
	@echo $(DIVIDER)
	@echo Deleting all your configurations !
	rm -rf $(CONFIGS_DIR)
	@echo $(DIVIDER)

# Targets to make tags files

# Mark these targets phony to force an update when gmake TAGS is done.
.PHONY: TAGS tags

TAGS:
	@echo $(DIVIDER)
	@echo Updating the Emacs TAGS file
	rm -f TAGS ; touch TAGS 
	find src arrangements \( -name '*.[chCF]' -o -name '*.F77' -o -name '*.cc' -o -name '*.[ch]pp' \) \
          -exec etags -a {} \;
#	find src arrangements \( -name '*.[cChF]' -o -name '*.F77' -o -name '*.cc'\) \
#          -exec etags --append --regex '/[a-z A-Z \t]*FORTRAN_NAME[^)]*/' {} \;
	perl -pi -e 's/(subroutine\s*)([a-zA-Z0-9_]+)/\1\L\2/g;' TAGS
	@echo $(DIVIDER)

tags:
	@echo $(DIVIDER)
	@echo Updating the vi tags file
	rm -f tags ; touch tags 
	find src arrangements \( -name '*.[cChF]' -o -name '*.F77' -o -name '*.cc' -o -name '*.[ch]pp' \) \
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
	@echo "****************************** "
	@echo "* Welcome to the Cactus Code *"
	@echo "******************************"
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
	@echo "  -clean         : to clean a configuration."
	@echo "                  (deletes all object and dependency files in "
	@echo "                   the configuration)."
	@echo "  -cleandeps     : to clean a configuration\'s dependency files."
	@echo "  -cleanobjs     : to clean a configuration\'s object files."
	@echo "  -config        : to (re)configure a configuration. "
	@echo "                  (runs or reruns the configuration scripts)."
	@echo "  -delete        : to delete a configuration." 
	@echo "  -editthorns    : edits the ThornList file. "
	@echo "  -realclean     : to restore a configuration to almost a new state. "
	@echo "                  (deletes all but the config-data directory "
	@echo "                   and the ThornList file)."
	@echo "  -rebuild       : to rebuild a configuration." 
	@echo "                  (forces the CST to be rerun)."
	@echo "  -testsuite     : run the test program."
	@echo "  -thornlist     : regenerates the ThornList file. "
	@echo "  -thornparfiles : copies example parameter files to thornparfiles."
endif
	@echo $(DIVIDER)
	@echo $(MAKE) also knows the following targets
	@echo
	@echo "  checkout  - checkout public arrangements/thorns."
	@echo "  default   - creates a new configuration with a default name."
	@echo "  distclean - deletes all existing configurations."
	@echo "  doc       - creates UserGuide.ps"
	@echo "  downsize  - remove non-essential files."
	@echo "  newthorn  - creates a new thorn."
	@echo "  TAGS      - creates an Emacs TAGS file."
	@echo "  tags      - creates a Vi TAGS file."
	@echo "  thorninfo - give information about all available thorns."
	@echo "  <anything else> prompts to create such a configuration."
	@echo $(DIVIDER)

# Clean a configuration

.PHONY clean:

clean:
	@echo $(DIVIDER)
	@echo Please specify a configuration to clean.
	@echo $(DIVIDER)

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -clean,$(CONFIGURATIONS)):

$(addsuffix -clean,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-clean=%)
	cd $(CONFIGS_DIR)/$(@:%-clean=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CONFIGS_DIR)/$(@:%-clean=%) CCTK_HOME=$(CCTK_HOME) clean
	@echo $(DIVIDER)

endif

%-clean:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-clean=%) does not exist.
	@echo Cleaning aborted.


# Clean just dependency files

.PHONY cleandeps:

cleandeps:
	@echo $(DIVIDER)
	@echo Please specify a configuration to clean the dependencies of.
	@echo $(DIVIDER)

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -cleandeps,$(CONFIGURATIONS)):

$(addsuffix -cleandeps,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-cleandeps=%)
	cd $(CONFIGS_DIR)/$(@:%-cleandeps=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CONFIGS_DIR)/$(@:%-cleandeps=%) CCTK_HOME=$(CCTK_HOME) cleandeps
	@echo $(DIVIDER)

endif

%-cleandeps:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-cleandeps=%) does not exist.
	@echo Cleaning dependencies aborted.


# Clean just object files

.PHONY cleanobjs:

cleanobjs:
	@echo $(DIVIDER)
	@echo Please specify a configuration to clean the object files of.
	@echo $(DIVIDER)


ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -cleanobjs,$(CONFIGURATIONS)):

$(addsuffix -cleanobjs,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-cleanobjs=%)
	cd $(CONFIGS_DIR)/$(@:%-cleanobjs=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CONFIGS_DIR)/$(@:%-cleanobjs=%) CCTK_HOME=$(CCTK_HOME) cleanobjs
	@echo $(DIVIDER)

endif

%-cleanobjs:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-cleanobjs=%) does not exist.
	@echo Cleaning object files aborted.


# Clean away all produced files (doesn't delete ThornList)

.PHONY realclean:

realclean:
	@echo $(DIVIDER)
	@echo Please specify a configuration to really clean.
	@echo $(DIVIDER)


ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -realclean,$(CONFIGURATIONS)):

$(addsuffix -realclean,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Cleaning configuration $(@:%-realclean=%)
	cd $(CONFIGS_DIR)/$(@:%-realclean=%)  
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CONFIGS_DIR)/$(@:%-realclean=%) CCTK_HOME=$(CCTK_HOME) realclean
	@echo $(DIVIDER)

endif

%-realclean:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-realclean=%) does not exist.
	@echo Cleaning aborted.

# Delete a configuration

.PHONY delete:

delete:
	@echo $(DIVIDER)
	@echo Please specify a configuration to delete.
	@echo $(DIVIDER)


ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -delete,$(CONFIGURATIONS)):

$(addsuffix -delete,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Deleting configuration $(@:%-delete=%)
	cd $(CONFIGS_DIR) ; rm -rf $(@:%-delete=%)  
	@echo $(DIVIDER)

endif

%-delete:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-delete=%) does not exist.
	@echo Deletion aborted.

# Rebuild a configuration

.PHONY rebuild:

rebuild:
	@echo $(DIVIDER)
	@echo Please specify a configuration to rebuild.
	@echo $(DIVIDER)

ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -rebuild,$(CONFIGURATIONS)):

$(addsuffix -rebuild,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Rebuilding $(@:%-rebuild=%)
	if [ -r $(CONFIGS_DIR)/$(@:%-rebuild=%)/config-data/make.thornlist ] ; then rm  $(CONFIGS_DIR)/$(@:%-rebuild=%)/config-data/make.thornlist ; fi
	$(MAKE) $(@:%-rebuild=%)
endif

%-rebuild:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-rebuild=%) does not exist.
	@echo Rebuild aborted.

# Regenerate the compiled thorns list

.PHONY thornlist:

thornlist:
	@echo $(DIVIDER)
	@echo Please specify a configuration to regenerate the thornlist of.
	@echo $(DIVIDER)


ifneq ($strip($(CONFIGURATIONS)),) 
.PHONY $(addsuffix -thornlist,$(CONFIGURATIONS)):

$(addsuffix -thornlist,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Regenerating compiled ThornList $(@:%-thornlist=%)
	if [ -r $(CONFIGS_DIR)/$(@:%-thornlist=%)/ThornList ] ; then rm $(CONFIGS_DIR)/$(@:%-thornlist=%)/ThornList ; fi
	$(MAKE) $(@:%-thornlist=%)
endif

%-thornlist:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-thornlist=%) does not exist.
	@echo Regeneration of compiled ThornList aborted.

# Edit the thornlist

.PHONY editthorn:

editthorns:
	@echo $(DIVIDER)
	@echo Please specify a configuration to edit the thornlist of.
	@echo $(DIVIDER)


ifneq ($strip($(CONFIGURATIONS)),) 
.PHONY $(addsuffix -editthorns,$(CONFIGURATIONS)):

$(addsuffix -editthorns,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Editing compiled ThornList $(@:%-editthorn=%)
	$(MAKE) -f $(CCTK_HOME)/lib/make/make.configuration TOP=$(CONFIGS_DIR)/$(@:%-editthorns=%) CCTK_HOME=$(CCTK_HOME) editthorns
endif

%-editthorns:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-editthorns=%) does not exist.
	@echo Editing of compiled ThornList aborted.

# Rerun the configuration script

.PHONY config:

config:
	@echo $(DIVIDER)
	@echo Please specify a configuration to configure.
	@echo $(DIVIDER)


ifneq ($strip($(CONFIGURATIONS)),)
.PHONY $(addsuffix -config,$(CONFIGURATIONS)):

$(addsuffix -config,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	$(SETUP_ENV) $(PERL) -s $(SETUP) -reconfig=1 $(SETUP_OPTIONS) $(@:%-config=%); 
endif

%-config:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-config=%) does not exist.
	echo Setup configuration $(@:%-config=%) \(yes\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xno" -o "x$$yesno" = "xn" -o "x$$yesno" = "xNO" -o "x$$yesno" = "xN" ] ;\
	then  \
	echo Setup cancelled ;     \
	else \
	echo Setting up new configuration $(@:%-config=%); \
	$(SETUP_ENV) $(PERL) -s $(SETUP) $(SETUP_OPTIONS) $(@:%-config=%); \
	echo $(DIVIDER)   ;  \
	echo Use $(MAKE) $(@:%-config=%) to build the configuration.; \
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

.PHONY testsuite:

testsuite:
	@echo $(DIVIDER)
	@echo Please specify a configuration to test.
	@echo $(DIVIDER)


ifneq ($strip($(CONFIGURATIONS)),) 
.PHONY $(addsuffix -testsuite,$(CONFIGURATIONS)):

$(addsuffix -testsuite,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Running test suite $(@:%-thornlist=%)
	if [ -r $(CONFIGS_DIR)/$(@:%-testsuite=%)/ThornList ] ; then $(PERL) lib/sbin/Runtest.pl $(@:%-testsuite=%) ; fi
endif

%-testsuite:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-testsuite=%) does not exist.
	@echo Test suite aborted.


ifneq ($strip($(CONFIGURATIONS)),) 
.PHONY $(addsuffix -thornparfiles,$(CONFIGURATIONS)):

# Copy thorn parameter files
.PHONY thornparfiles:

thornparfiles:
	@echo $(DIVIDER)
	@echo Please specify a configuration.
	@echo $(DIVIDER)


$(addsuffix -thornparfiles,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Copying parameter files $(@:%-thornparfiles=%)
	if [ -r $(CONFIGS_DIR)/$(@:%-thornparfiles=%)/ThornList ] ; then $(PERL) lib/sbin/CopyParFiles.pl $(@:%-thornparfiles=%) ; fi
endif

%-thornparfiles:
	@echo $(DIVIDER)
	@echo Configuration $(@:%-thornparfiles=%) does not exist.
	@echo Parameter file copying aborted.


# Checkout public thorns and arrangements

.PHONY: checkout
checkout:
	@echo $(DIVIDER)
	@echo Running app/arrangement/thorn checkout script
	$(PERL) ./lib/sbin/checkout.pl

# Create sysinfo file

.PHONY sysinfo:

sysinfo:
	@echo $(DIVIDER)
	@echo Please specify a configuration to run sysinfo with.
	@echo $(DIVIDER)

ifneq ($strip($(CONFIGURATIONS)),) 
.PHONY $(addsuffix -sysinfo,$(CONFIGURATIONS)):

$(addsuffix -sysinfo,$(CONFIGURATIONS)):
	@echo $(DIVIDER)
	@echo Running SystemInfo
	$(PERL) ./lib/sbin/SystemInfo.pl $(@:%-sysinfo=%) 
endif


# Make the users manuals

.PHONY: doc
doc:
	@echo $(DIVIDER)
	@echo Creating user documentation UsersGuide.ps
	(cd doc/UsersGuide; latex UsersGuide.tex; latex UsersGuide.tex; dvips ./UsersGuide.dvi -o $(CCTK_HOME)/UsersGuide.ps) ; cd $(CCTK_HOME);
	@echo $(DIVIDER)

# Rule to show thorn information

.PHONY: thorninfo
thorninfo:
	@echo $(DIVIDER)
	@echo Displaying info for all thorns in the arrangements directory
	$(PERL) -s $(BUILD_ACTIVETHORNS) $(CCTK_HOME)/arrangements > $@;
	cat $@;
	@echo $(DIVIDER)


# Remove non-essential files

.PHONY: downsize
downsize:
	@echo $(DIVIDER)
	@echo Remove flesh and thorn documentation \(\no\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xyes" -o "x$$yesno" = "xy" -o "x$$yesno" = "xYES" -o "x$$yesno" = "xY" ] ;\
	then  \
	rm -rf doc; rm -rf arrangements/*/*/doc; \
	echo $(DIVIDER)   ;  \
	fi 
	@echo Remove thorn testsuites \(\no\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xyes" -o "x$$yesno" = "xy" -o "x$$yesno" = "xYES" -o "x$$yesno" = "xY" ] ;\
	then  \
	rm -rf arrangements/*/*/test; \
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
	echo Setup configuration $@ \(yes\)?
	read yesno rest ;\
	if [ "x$$yesno" = "xno" -o "x$$yesno" = "xn" -o "x$$yesno" = "xNO" -o "x$$yesno" = "xN" ] ;\
	then  \
	echo Setup cancelled ;     \
	else \
	echo Setting up new configuration $@; \
	$(SETUP_ENV) $(PERL) -s $(SETUP) $(SETUP_OPTIONS) $@; \
	echo $(DIVIDER)   ;  \
	echo Use $(MAKE) $@ to build the configuration.; \
	fi 
	@echo $(DIVIDER)

