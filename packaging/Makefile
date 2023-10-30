include ../Makefile.vars

ifeq ("${OSNAME}", "freebsd")
PKGPCDIR = libdata/pkgconfig
else
PKGPCDIR = lib/pkgconfig
endif

TARTARGS = ${TARDIR}/${PKGPCDIR}/mctally.pc

all::
	echo "nothing to be done for 'all' target."

tarprep: ${TARTARGS}

${TARDIR}/${PKGPCDIR}/mctally.pc: mctally.pc
	../install-sh -c -m 444 $< $@

distclean::
	rm -f debcontrol fbsd_manifest mctally.pc
