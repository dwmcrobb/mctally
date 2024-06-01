include ../../Makefile.vars

LTLINK       = ${LIBTOOL} --mode=link --tag=CXX ${CXX}

OBJFILES = mctally.o
OBJDEPS	 = $(OBJFILES:%.o=deps/%_deps)
CXXFLAGS += -g
LDFLAGS  += -Wl,-rpath ${DWMDIR}/lib
ALLINC   = ${DWMINCS}
ALLINC   += ${NLOHMANNJSONINC} -I../../classes/include -I.
ALLLIBS  = ${DWMLIBS}
TARTARGETS  = ${TARDIR}/bin/mctally

mctally: ${OBJFILES} ../../classes/lib/libDwmMcTally.la
	${LTLINK} ${LDFLAGS} -o $@ $^ ${ALLLIBS}

#  dependency rule
deps/%_deps: %.cc 
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${ALLINC} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

%.o: %.cc deps/%_deps
	${CXX} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $<

../../classes/lib/libDwmMcTally.la::
	${MAKE} -C ../../classes/src

tarprep: ${TARTARGETS}

${TARDIR}/bin/mctally: mctally
	${LIBTOOL} --mode=install ../../install-sh -s -c -m 555 $< $@

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f mctally ${OBJFILES}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARTARGETS}

distclean:: clean
	rm -f deps/*_deps
