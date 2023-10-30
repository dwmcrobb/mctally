CXXFLAGS    = -O2 -std=c++17
INCS        = -I../include
OBJSNOPATH  = DwmMcTallyUtils.o
OBJS        = $(OBJSNOPATH:%.o=../obj/%.o)
OBJDEPS     = $(OBJSNOPATH:%.o=deps/%_deps)

../lib/libDwmMcTally.a: ${OBJS}
	ar rv $@ $^
	-ranlib $@

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${INCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

../obj/%.o: %.cc
	${CXX} ${CXXFLAGS} ${INCS} -o $@ -c $<

clean::
	rm -f ../lib/libDwmMcTally.a ${OBJS}

distclean:: clean
	rm -f ${OBJDEPS}
