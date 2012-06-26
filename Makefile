RAYDIRS = Background Core Filter Geometry Materials UI IO Jobs GlobalIllum scenes noise
SUBDIRS = $(RAYDIRS) Tools
TARGETS = all clean tidy

RAYLIBS = $(addprefix lib,$(addsuffix .so,$(RAYDIRS)))

.PHONY:	$(SUBDIRS) $(TARGETS)

ALL_TARGET = ray

all:	$(SUBDIRS) ray



TIDYDIRT = config.log configure config.status autom4te.cache
CLEANDIRT = ray ray.d

-include config_vars.mk
-include vars.mk
-include targets.mk


$(TARGETS):	$(SUBDIRS)
$(SUBDIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)


ray:	main.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath -Wl,$(shell pwd) -Wl,--export-dynamic main.o -o $@ $(RAYLIBS)


