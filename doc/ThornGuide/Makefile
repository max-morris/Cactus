# make (defaults to ThornGuide.ps)
# make ThornGuide.tex
# make ThornGuide.dvi
# make ThornGuide.ps
# make ThornGuide.pdf


# Make quietly unless told not to
ifneq ($(strip $(SILENT)),no)
.SILENT:
endif

ifeq ($(strip $(CCTK_HOME)), )
   CCTK_HOME=$(shell pwd | sed 's,^/cygdrive/\(.\)/,\1:/,' | sed 's,^//\(.\)/,\1:/,' | sed 's/\/doc\/ThornGuide.*//' )
endif

ARRANGEMENTS_DIR=$(CCTK_HOME)/arrangements

ifeq ($(strip $(THORNLIST)), )
   THORNLIST=allthorns.th
endif

ifeq ($(strip $(PERL)), )
   PERL=/usr/bin/perl
endif

ifeq ($(strip $(MASTER_FILE)), )
   MASTER_FILE=ThornGuide
endif

IMAGE_FILE=$(CCTK_HOME)/doc/UsersGuide/bincactus2.eps


-include make.thornlist

.PHONY: default

default : $(MASTER_FILE).ps

$(MASTER_FILE).ps: $(MASTER_FILE).dvi 
	@echo "  Running dvips on $<"
	dvips -f $< 2> LOG_DVIPS_MSGS | $(CCTK_HOME)/lib/sbin/FixPageNumbersInPostscript.pl > $@ 
	if test -e "$@"; then                                  \
	   echo "     $@ created.";                              \
	else                                                   \
	   echo "  Cannot find $@, an error must have occured."; \
	fi ;                                                   

$(MASTER_FILE).dvi: $(MASTER_FILE).tex
	@echo "  Latexing $<"
	latex -interaction=nonstopmode $< >  LOG_LATEX_MSGS 2>&1; \
	latex -interaction=nonstopmode $< >> LOG_LATEX_MSGS 2>&1; \
	latex -interaction=nonstopmode $< >> LOG_LATEX_MSGS 2>&1; \
	latex -interaction=nonstopmode $< >> LOG_LATEX_MSGS 2>&1;  \
	if test -e "$@"; then                                  \
	   echo "     $@ created.";                              \
	else                                                   \
	   echo "  Cannot find $@, an error must have occured."; \
	fi ;                                                   

$(MASTER_FILE).pdf: $(MASTER_FILE).tex
	@echo "  Running pdflatex on $<"

	pdflatex -interaction=nonstopmode $< >  LOG_PDFLATEX_MSGS 2>&1; \
	pdflatex -interaction=nonstopmode $< >> LOG_PDFLATEX_MSGS 2>&1; \
	pdflatex -interaction=nonstopmode $< >> LOG_PDFLATEX_MSGS 2>&1; \
	pdflatex -interaction=nonstopmode $< >> LOG_PDFLATEX_MSGS 2>&1

	if test -e "$@"; then                                  \
	   echo "     $@ created.";                              \
	else                                                   \
	   echo "  Cannot find $@, an error must have occured."; \
	fi ;                                                  

$(MASTER_FILE).tex:  $(foreach thorn, $(THORNS), $(thorn)/param.ccl $(thorn)/interface.ccl $(thorn)/schedule.ccl)
	@echo "  Creating thorn documentation $(MASTER_FILE).tex"
	@echo "    Processing ..."
	@echo "       param.ccl ..."
	$(PERL) -s $(CCTK_HOME)/lib/sbin/ParamLatex.pl -cctk_home=$(CCTK_HOME) -directory=$(ARRANGEMENTS_DIR) -thornlist=$(THORNLIST) -outdir=./ -section -grouping=bythorn -sort=scope > LOG_PARAMLATEX_MSGS 2>&1
	@echo "       interface.ccl ..."
	$(PERL) -s $(CCTK_HOME)/lib/sbin/InterLatex.pl -cctk_home=$(CCTK_HOME) -directory=$(ARRANGEMENTS_DIR) -thornlist=$(THORNLIST) -outdir=./ -section -grouping=bythorn > LOG_INTERLATEX_MSGS 2>&1
	@echo "       schedule.ccl ..."
	$(PERL) -s $(CCTK_HOME)/lib/sbin/SchedLatex.pl -cctk_home=$(CCTK_HOME) -directory=$(ARRANGEMENTS_DIR) -thornlist=$(THORNLIST) -outdir=./ -section > LOG_SCHEDLATEX_MSGS 2>&1
	@echo "  Creating $@ ..."
	$(PERL) -s $(CCTK_HOME)/lib/sbin/ThornGuide.pl -cctk_home=$(CCTK_HOME) -directory=$(ARRANGEMENTS_DIR) -thornlist=$(THORNLIST) -outdir=./ -outfile=$(MASTER_FILE).tex > LOG_THORNGUIDE_MSGS 2>&1
	if test -e $(IMAGE_FILE) && test ! -e "bincactus.eps"; then \
	   cp $(IMAGE_FILE) bincactus.eps;                          \
	fi ; 

make.thornlist: $(THORNLIST) 
	THORNS=`$(PERL) -s $(CCTK_HOME)/lib/sbin/ThornList.pl -arrangements_dir=$(ARRANGEMENTS_DIR) -thornlist=$(THORNLIST)`

allthorns.th:
	@echo "Creating $@ ..."
	$(PERL) -s $(CCTK_HOME)/lib/sbin/ThornList.pl -arrangements_dir=$(ARRANGEMENTS_DIR) > $@ 
