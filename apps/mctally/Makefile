CXXFLAGS = -std=c++17 -pthread
LDFLAGS  = -pthread
INCS     = -I../../classes/include
OBJFILES = mctally.o
OBJDEPS  = $(OBJFILES:%.o=deps/%_deps)

mctally: ${OBJFILES} ../../classes/lib/libDwmMcTally.a
	${CXX} ${LDFLAGS} -o $@ $^

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${INCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean'
ifneq ($(MAKECMDGOALS),clean)
-include ${OBJDEPS}
endif

%.o: %.cc
	${CXX} ${CXXFLAGS} ${INCS} -c $<

../../classes/lib/libDwmMcTally.a::
	${MAKE} -C ../../classes/src

distclean:: clean
	rm -f deps/*_deps

clean::
	rm -f mctally ${OBJFILES}
