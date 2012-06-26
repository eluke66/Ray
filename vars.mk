top_srcdir = /disk/Software/git_workspace/Ray

INCDIR = -I$(top_srcdir)/include

DEP_DIR = .Dependencies


CXXFLAGS = $(CFLAGS)
LDFLAGS :=$(CXXFLAGS) -shared -rdynamic

ifneq "$(SWIG)" ""
BASESWIGFILES = $(wildcard *.sw)
SWIGFILES = $(filter-out $(EXCLUDED_FILES),$(BASESWIGFILES))
SWIGCPPFILES = $(subst .sw,_pywrap.cc,$(SWIGFILES))
SWIGOBJS = $(subst .cc,.o,$(SWIGCPPFILES))
endif

BASECPPFILES = $(wildcard *.cc)
BASECFILES =  $(wildcard *.c)
ALLCPPFILES = $(filter-out $(EXCLUDED_FILES),$(BASECPPFILES))
CPPFILES = $(filter-out %_pywrap.cc,$(ALLCPPFILES))
CFILES = $(filter-out $(EXCLUDED_FILES),$(BASECFILES))
OBJS = $(subst .cc,.o,$(CPPFILES)) $(subst .c,.o,$(CFILES)) 
CWD = $(shell pwd)
THISDIR = $(shell basename $(CWD))
LIBDIR = ..
LIB = $(LIBDIR)/lib$(THISDIR).so

ifneq "$(SWIGFILES)" ""
SWIGLIB = $(LIBDIR)/_$(THISDIR).so
endif
