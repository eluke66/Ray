#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.57)
AC_INIT("NewRayTracer", "0.x", "/dev/null")
AC_CONFIG_SRCDIR([config_vars.mk.in])
AC_CONFIG_HEADER([include/config.h])
AC_CANONICAL_HOST

# Command line arguments
AC_ARG_ENABLE( 	[glut], 
               	AC_HELP_STRING([--enable-glut],
                	       [Enable the GLUT interface (default is YES)]),
		[use_glut=$enableval],
		[use_glut=yes] )

AC_ARG_ENABLE( [lib3ds],
		AC_HELP_STRING([--enable-lib3ds],
       				[Enable lib3ds (default is YES if found)]),
		[use_lib3ds=$enableval],
		[use_lib3ds=yes] )

AC_ARG_ENABLE( [debug],
	    	AC_HELP_STRING([--enable-debug],
			        [Build a debugged version (default is NO)]),
	       [debug=$enableval],
               [debug=no] )

AC_ARG_ENABLE( [profile],
	    	AC_HELP_STRING([--enable-profile],
			        [Build a version useful for profiling (default is NO)]),
	       [profile=$enableval],
               [profile=no] )

AC_ARG_ENABLE( [static],
	    	AC_HELP_STRING([--enable-static],
			        [Build a static version (default is NO)]),
	       [static=$enableval],
               [static=no] )

AC_ARG_ENABLE(  [build_imgdiff],
		AC_HELP_STRING( [--enable-imgdiff],
				[Build the ppm image diff utility (default is ON)]),
		[build_imgdiff=$enableval],
		[build_imgdiff=yes] )

AC_ARG_ENABLE( [use_magicwand],
		AC_HELP_STRING( [--enable-MagickWand],
				[Use ImageMagicks MagickWand for image I/O (default is ON)]),
		[use_magicwand=$enableval],
		[use_magicwand=yes] )

AC_ARG_ENABLE( [swig],
		AC_HELP_STRING([--enable-swig],
				[Enable the Python interface via SWIG (default is YES)]),
		[swig=$enableval],
		[swig=yes] )

AC_ARG_ENABLE( [tracealloc],
		AC_HELP_STRING([--enable-tracealloc],
				[Enable geometry allocation counting (default is OFF)]),
		[tracealloc=$enableval],
		[tracealloc=no] )


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_CHECK_PROG([MV], [mv], [/bin/mv] )

# Check for python

if test $swig = yes; then

AC_PATH_PROG([PYTHON], [python], "NOPYTHON")

if test $PYTHON = "NOPYTHON"; then
	AC_MSG_WARN([No python found - disabling dynamic scene support.])
else
	pyver=`$PYTHON -V 2>&1 | awk '{print $2}' | cut  -d"." -f1,2`
	AC_MSG_NOTICE([Using python version $pyver])
	PYTHON_INCLUDE="python${pyver}"
	pyheader="$PYTHON_INCLUDE/Python.h"
	AC_CHECK_HEADER($pyheader)
	AC_CHECK_LIB([python${pyver}], [Py_Main],)
	AC_SUBST(PYTHON_INCLUDE)
	AC_DEFINE_UNQUOTED(PYTHON_VERSION, <$pyheader>, [Version of python])

	AC_PATH_PROG([SWIG], [swig], "" )
	if test $SWIG = ""; then
		AC_MSG_WARN([Swig not found - disabling dynamic scene support.])
	else
		SWIGFLAGS="-python -c++ -w503 -w362"
		AC_SUBST(SWIGFLAGS)
		AC_DEFINE(USE_SWIG, [], [Enable dynamic scene support via swig])
	fi 

fi
fi # Swig test


# How to move dependency files.
MV_CC_DEP='@if [[ -e $(subst .cc,.d,$(notdir $<)) ]]; then $(MV) $(subst .cc,.d,$(notdir $<)) $(DEP_DIR); fi'
MV_CPP_DEP='@if [[ -e $(subst .cpp,.d,$(notdir $<)) ]]; then $(MV) $(subst .cpp,.d,$(notdir $<)) $(DEP_DIR); fi'
MV_C_DEP='@if [[ -e $(subst .c,.d,$(notdir $<)) ]]; then $(MV) $(subst .c,.d,$(notdir $<)) $(DEP_DIR); fi'

# Some defaults based on the type of host that we have
case "$host" in
*-*-irix6*)
	AC_MSG_NOTICE([Using settings for SGI])
	OPTFLAGS="-Ofast"
	DEBUGFLAGS="-O0 -g"
	CFLAGS="$CFLAGS -MDupdate Makedepend -LANG:std"
	MAKEDEPEND="-include Makedepend"
	MV_CC_DEP=""
	MV_CPP_DEP=""
	MV_C_DEP=""
	;;
i686-*-linux*)
	AC_MSG_NOTICE([Using settings for 32-bit linux])
	DEBUGFLAGS="-O0 -g"
	PROFILEFLAGS="-O -pg"
	FULLOPTFLAGS="-O3 -fomit-frame-pointer"
	CFLAGS="$CFLAGS  -MD -Wall"
	TEMPLATE_FLAG="-frepo"
	MAKEDEPEND="-include \$(subst .cc,.d,\$(addprefix \$(DEP_DIR)/,\$(ALLCPPFILES))) \$(subst .c,.d,\$(addprefix \$(DEP_DIR)/,\$(CFILES)))"
	cpu_family=`cat /proc/cpuinfo | grep "cpu family" | sort | uniq| awk '{print $4}'`
	cpu_model=`cat /proc/cpuinfo | grep "model" | sort | uniq | grep -v name | awk '{print $3}'`
	vendor_id=`cat /proc/cpuinfo | grep vendor_id | sort | uniq | awk '{print $3}'`
	AC_MSG_CHECKING([CPU-specific optimization flags])
	case "$cpu_family" in
		5)
			case "$cpu_model" in
			2)
				archflags = "-march=pentium"
			;;
			*)
				AC_MSG_WARN([Unknown pentium model $cpu_model])
				archflags = "-march=i386"
			;;
			esac
		;;
		6)
			case "$cpu_model" in
			4)
				archflags="-march=athlon"
			;;
			5)
				archflags="-march=pentium2"
			;;
			6|7)
				if test "$vendor_id" = "AuthenticAMD"; then
				  archflags="-march=athlon"
				else
				  archflags="-march=pentium3"
				fi
			;;
			8)
				if test "$vendor_id" = "AuthenticAMD"; then
				  archflags="-march=athlon-xp"
				else
				  archflags="-march=pentium3"
				fi
			;;
			10)
				archflags="-march=athlon-xp"
			;;
			*)
				AC_MSG_WARN([Unknown 686 model $cpu_model])
				archflags = "-march=i686"	
			;;
			esac
		;;
		15)
			case "$cpu_model" in
				1|2)
					archflags="-march=pentium4"
				;;
				5)
					AC_MSG_WARN([Using athlon-mp flags])	
					archflags="-march=athlon-mp -msse2"
				;;
				*)
					AC_MSG_WARN([Unknown p4 model])
					archflags="-march=pentium4"
				;;
			esac
			
		;;
		*)
			AC_MSG_WARN([Unknown cpu family $cpu_family])	
			archflags="-march=i386"
		;;
	esac
	AC_MSG_RESULT([$archflags])
	case "$CXX" in
	*g++)
		if test "$profile" = "yes"; then
 			AC_MSG_NOTICE([Using PROFILE settings])
 			OPTFLAGS="$PROFILEFLAGS"
		else
 			OPTFLAGS="$FULLOPTFLAGS"
		fi

		OPTFLAGS="$OPTFLAGS -malign-double $archflags"
		;;
	*icc)
		OPTFLAGS="$OPTFLAGS -O3 -tpp6 -align"
		which_icc=`which "$CXX"`
		icc_bin=`dirname $which_icc`
		icc_path=`dirname $icc_bin`
		LIBS="$LIBS -Wl,-rpath $icc_path/lib"
		CFLAGS="$CFLAGS -wd1418,981,810,383,279,1419"
		;;
	*)
		AC_MSG_WARN([Unknown compiler $CXX - not giving any good flags])
		;;
	esac
	;;
x86_64-*-linux*)
	archflags="-m64 -fPIC"
	FULLOPTFLAGS="-O3 -fomit-frame-pointer"
	OPTFLAGS="$FULLOPTFLAGS $archflags"
	CFLAGS="$CFLAGS  -MD -Wall"
	TEMPLATE_FLAG="-frepo"
	MAKEDEPEND="-include \$(subst .cc,.d,\$(addprefix \$(DEP_DIR)/,\$(ALLCPPFILES))) \$(subst .c,.d,\$(addprefix \$(DEP_DIR)/,\$(CFILES)))"
	
	;;
*)
	AC_MSG_WARN([Don't know what you're running here, buddy])
	;;
esac

AC_SUBST(MAKEDEPEND)
AC_SUBST(MV_CC_DEP)
AC_SUBST(MV_CPP_DEP)
AC_SUBST(MV_C_DEP)
AC_SUBST(TEMPLATE_FLAG)

if test "$debug" = "yes"; then
 AC_MSG_NOTICE([Using DEBUG settings])
 CFLAGS="$CFLAGS $DEBUGFLAGS"
else
 CFLAGS="$CFLAGS $OPTFLAGS"
fi

if test "$static" = "yes"; then
 AC_MSG_NOTICE([Using STATIC settings])
 LDFLAGS="-static"
else
 LDFLAGS=""
fi
AC_SUBST(STATIC_FLAGS)


EXTRA_TARGETS=
if test "$build_imgdiff" = "yes"; then
	AC_MSG_NOTICE([Building ImgDiff])
	EXTRA_TARGETS="$EXTRA_TARGETS ImgDiff"
fi
AC_SUBST(EXTRA_TARGETS)


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h sys/time.h unistd.h values.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME
AC_CHECK_TYPES(unsigned long long)



# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday sqrt strdup strrchr strtoul])

AC_CHECK_LIB([jpeg], [jpeg_abort])
AC_CHECK_LIB([m], [atan])
AC_CHECK_LIB([pbm], [pbm_init])
AC_CHECK_LIB([pgm], [pgm_init])
AC_CHECK_LIB([ppm], [ppm_init])
AC_CHECK_LIB([netpbm], [ppm_init])

# Checks for libraries.
if test "$use_glut" = "yes"; then
	
	# FIXME - should somehow check for these guys...?
	AC_PATH_XTRA
	#AC_CHECK_LIB([X11], [XAllocColor])
	#AC_CHECK_LIB([Xext], [XdbeEndIdiom])
	#AC_CHECK_LIB([Xi], [XCloseDevice])
	#AC_CHECK_LIB([Xmu], [XctFree])
	X_LIBS="$X_LIBS -lXext -lXi -lX11 -lXmu"
	AC_CHECK_LIB([GL], [glAlphaFunc])
	AC_CHECK_LIB([GLU], [bezierPatchEval],,,$X_LIBS)
	AC_CHECK_LIB([glut], [glutGetColor],have_glut="yes",have_glut="no",$X_LIBS)
else
	AC_DEFINE(NO_GLUT, [], [True to not use glut in the code])
	have_glut="no"
fi

if test "$have_glut" = "yes"; then
	INCLUDE_GLUT='UI := $(GLUT) $(UI)'
	X_LIBS="$X_LIBS -lglut"
else
	INCLUDE_GLUT=""
	X_LIBS=""
fi

AC_SUBST(INCLUDE_GLUT)

HAVE_3DS="\"\""
if test "$use_lib3ds" = "yes"; then
	config=`which lib3ds-config`
	if test "$?" = "0"; then
		flags3ds=`$config --cflags`
		libs3ds=`$config --libs`
		CFLAGS="$CFLAGS $flags3ds"
		LIBS="$LIBS $libs3ds"
		HAVE_3DS="\"true\""
	else
		AC_MSG_WARN([Cannot find lib3ds-config. Make sure it's in the path, and re-run])	
	fi
fi
AC_SUBST(HAVE_3DS)

HAVE_MAGICWAND="\"no\""
if test "$use_magicwand" = "yes"; then
	AC_CHECK_HEADERS([wand/magick-wand.h], [], proceed=no)
	if test "$proceed" = no; then
		AC_MSG_WARN([Old-style magick wand not yet supported])
	else
		config=`which Wand-config`
		if test "$?" = "0"; then
			libsWand=`$config --libs`
			WANDLIBS="$X_LIBS $LIBS $libsWand"
			HAVE_MAGICWAND="\"yes\""
			AC_SUBST(WANDLIBS)
			AC_DEFINE(USE_MAGICK_WAND,[], [True to use magick wand])
		fi
	fi
else
	AC_MSG_WARN([Cannot find Wand-config. Make sure it's in the path, and re-run])	
fi
AC_SUBST(HAVE_MAGICWAND)

if test "$tracealloc" = "yes"; then
	AC_DEFINE(TRACE_ALLOCATIONS,[], [True to trace object allocation])
fi

AC_CONFIG_FILES([config_vars.mk targets.mk])
AC_OUTPUT
