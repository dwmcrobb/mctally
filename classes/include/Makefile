include ../../Makefile.vars

HEADERS		= $(shell ls *.hh)
TARPREPHEADERS	= $(HEADERS:%=${TARDIR}/include/libDwmMcTally/%)
INSTALLHEADERS	= $(HEADERS:%=${INSTALLPREFIX}/include/libDwmMcTally/%)

all::
	@echo "Nothing to be done for 'all'"

tarprep: ${TARPREPHEADERS}

${TARDIR}/include/libDwmMcTally/%.hh: %.hh
	../../install-sh -c -m 444 $< $@

install: ${INSTALLHEADERS}

${INSTALLPREFIX}/include/libDwmMcTally/%.hh: %.hh
	../../install-sh -c -m 444 $< $@

uninstall:: clean-install

clean:: clean-tarprep

clean-install::
	rm -f ${INSTALLHEADERS}

clean-tarprep::
	rm -f ${TARPREPHEADERS}

distclean:: clean
