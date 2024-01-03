include ../../Makefile.vars

LTCOMPILE    = ${LIBTOOL} --mode=compile --tag=CXX ${CXX}
LTLINK       = ${LIBTOOL} --mode=link --tag=CXX ${CXX}

ALLINC       = -I../include ${DWMINCS}
OBJFILESNP   = DwmMcTallyConfigLex.o \
               DwmMcTallyConfigParse.o \
               DwmMcTallyInstalledPackages.o \
               DwmMcTallyLoadAvg.o \
               DwmMcTallyLogins.o \
               DwmMcTallyRequest.o \
               DwmMcTallyResponse.o \
               DwmMcTallyServiceConfig.o \
               DwmMcTallyUname.o \
               DwmMcTallyUtils.o \
               DwmMcTallyVersion.o
OBJFILES     = $(OBJFILESNP:%=../obj/%)
OBJDEPS      = $(OBJFILESNP:%.o=deps/%_deps)
SHARED_OBJFILES = $(OBJFILES:%.o=%.lo)
shlib_version = $(shell ../../getvers.sh -s)

all: ../lib/libDwmMcTally.la

../lib/libDwmMcTally.la: ${SHARED_OBJFILES} ${OBJFILES}
	${LTLINK} -o $@ ${SHARED_OBJFILES} -rpath ${INSTALLPREFIX}/lib \
	  -version-info ${shlib_version} ${DWMLIBS}

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.lo $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

../obj/%.o ../obj/%.lo: %.cc deps/%_deps
	${LTCOMPILE} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $< -o $@

DwmMcTallyConfigParse.hh: DwmMcTallyConfigParse.cc

DwmMcTallyConfigParse.cc: DwmMcTallyConfigParse.y
	bison -d -o$@ $<

DwmMcTallyConfigLex.cc: DwmMcTallyConfigLex.lex DwmMcTallyConfigParse.hh
	flex -o$@ $<

tarprep: ${TARDIR}/lib/libDwmMcTally.la

${TARDIR}/lib/libDwmMcTally.la: ../lib/libDwmMcTally.la
	${LIBTOOL} --mode=install ../../install-sh -c $< $@

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f ../lib/libDwmMcTally.la ${OBJFILES}
	${LIBTOOL} --mode=clean rm -f DwmMcTallyConfigParse.cc DwmMcTallyConfigParse.hh DwmMcTallyConfigLex.cc

distclean:: clean
	rm -f ${OBJDEPS}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARDIR}/lib/libDwmMcTally.la
