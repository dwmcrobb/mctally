include ../../Makefile.vars

PKGTARGETS = ${TARDIR}/man/man1/mctally.1 \
             ${TARDIR}/man/man8/mctallyd.8

all: mctally.html mctallyd.html

tarprep:: ${PKGTARGETS}

${TARDIR}/man/man1/mctally.1: mctally.1
	../../install-sh -c -m 644 $< $@

${TARDIR}/man/man8/mctallyd.8: mctallyd.8
	../../install-sh -c -m 644 $< $@

mctally.html: mctally.1 mctally.css
	mandoc -T html -O style=./mctally.css $< > $@

mctallyd.html: mctallyd.8 mctally.css
	mandoc -T html -O style=./mctally.css $< > $@

clean::
	rm -f mctally.html mctallyd.html

distclean:: clean tarprep-clean

tarprep-clean::
	rm -f ${PKGTARGETS}
