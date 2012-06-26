CC = g++
MV = /bin/mv 
SWIG = /usr/bin/swig
SWIGFLAGS = -python -c++ -w503 -w362 $(INCDIR)
PYTHON_INCLUDE = -I/usr/include/python2.6
PYLIB = python2.6

DEFINES = -DHAVE_CONFIG_H
LIBS = -lGL -lnetpbm -lm -ljpeg -lpython2.6  -l3ds
CFLAGS = -g -O2  -MD -Wall -O3 -fomit-frame-pointer -m64 -fPIC  $(INCDIR)

WANDLIBS=

ifeq ($(USE_X),yes)
 CFLAGS +=  
 EXTRALIBS +=  -lXext -lXi -lX11 -lXmu -lglut -lGL $(LIBS)
endif
