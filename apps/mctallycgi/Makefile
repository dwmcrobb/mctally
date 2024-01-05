include ../../Makefile.vars

LTLINK       = ${LIBTOOL} --mode=link --tag=CXX ${CXX}

OBJFILES = mctallycgi.o
OBJDEPS	 = $(OBJFILES:%.o=deps/%_deps)
CXXFLAGS += -g
LDFLAGS  += -Wl,-rpath ${DWMDIR}/lib
ALLINC   = ${DWMINCS}
ALLINC   += ${NLOHMANNJSONINC} -I../../classes/include -I.
ALLLIBS  = ${DWMLIBS}
TARTARGETS  = ${TARDIR}/bin/mctallycgi

mctallycgi: ${OBJFILES} ../../classes/lib/libDwmMcTally.la
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
-include ${OBJDEPS}
endif

%.o: %.cc deps/%_deps
	${CXX} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $<

../../classes/lib/libDwmMcTally.la::
	${MAKE} -C ../../classes/src

tarprep: ${TARTARGETS}

${TARDIR}/bin/mctallycgi: mctallycgi
	${LIBTOOL} --mode=install ../../install-sh -s -c -m 555 $< $@

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f mctallycgi ${OBJFILES}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARTARGETS}

distclean:: clean
	rm -f deps/*_deps
