.cc.o:
	$(CXX) $(CXXFLAGS) -c $<
	@if [ -e $(subst .cc,.d,$(notdir $<)) ]; then $(MV) $(subst .cc,.d,$(notdir $<)) $(DEP_DIR); fi

.c.o:
	$(CC) $(CFLAGS) -c $<
	@if [ -e $(subst .c,.d,$(notdir $<)) ]; then $(MV) $(subst .c,.d,$(notdir $<)) $(DEP_DIR); fi

%_pywrap.cc: %.sw
	$(SWIG) $(SWIGFLAGS) -o $@.1 $<
	cat $@.1 | grep -v "delete arg1;" | perl $(top_srcdir)/swigstrip.pl > $@
	$(RM) $@.1 
	$(MV) $(subst .sw,.py,$<) $(LIBDIR)

%_pywrap.o: %_pywrap.cc
	$(CXX) $(CXXFLAGS) $(PYTHON_INCLUDE) -c $<
	@if [ -e $(subst .cc,.d,$(notdir $<)) ]; then $(MV) $(subst .cc,.d,$(notdir $<)) $(DEP_DIR); fi

ifeq "$(ALL_TARGET)" ""
all: $(LIB) $(SWIGLIB)
endif

$(SWIGLIB): $(DEP_DIR) $(SWIGOBJS)
	$(CXX) $(LDFLAGS) -o $@ $(SWIGOBJS) -l$(PYLIB)

$(LIB):	$(DEP_DIR) $(OBJS) 
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(EXTRALIBS)

clean:
	rm -f $(OBJS) $(LIB) $(CLEANDIRT) $(SWIGOBJS) $(SWIGLIB)

tidy:	clean
	/bin/rm -rf ii_files  *~ *.il *.rpo $(DEP_DIR) $(TIDYDIRT) $(SWIGCPPFILES) $(addprefix $(LIBDIR)/,$(subst .sw,.py,$(SWIGFILES)))

-include $(subst .cc,.d,$(addprefix $(DEP_DIR)/,$(ALLCPPFILES))) $(subst .c,.d,$(addprefix $(DEP_DIR)/,$(CFILES)))

$(DEP_DIR):
	@mkdir $(DEP_DIR)

.PRECIOUS:	%_pywrap.cc
