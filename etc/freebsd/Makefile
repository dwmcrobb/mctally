include ../../Makefile.vars

TARTARGETS  = ${STAGING_DIR}/+POST_INSTALL ${TARDIR}/etc/rc.d/mctallyd

all::
	echo "Nothing to be done for 'all'."

tarprep:: ${TARTARGETS}

${STAGING_DIR}/+POST_INSTALL: +POST_INSTALL
	../../install-sh -c -m 755 $< $@

${TARDIR}/etc/rc.d/mctallyd::
	${MAKE} -C rc.d tarprep

clean:: clean-tarprep

clean-tarprep::
	rm -f ${TARTARGETS}

