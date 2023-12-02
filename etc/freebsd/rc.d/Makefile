include ../../../Makefile.vars

TARTARGETS  = ${TARDIR}/etc/rc.d/mctallyd

all::
	echo "Nothing to be done for 'all'."

tarprep:: ${TARTARGETS}

${TARDIR}/etc/rc.d/mctallyd: mctallyd
	../../../install-sh -c -m 755 $< $@

clean:: clean-tarprep

clean-tarprep::
	rm -f ${TARTARGETS}

