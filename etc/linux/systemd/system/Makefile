include ../../../../Makefile.vars

TARTARGETS  = ${STAGING_DIR}/etc/systemd/system/mctallyd.service

all::
	echo "Nothing to be done for 'all'."

tarprep:: ${TARTARGETS}

${STAGING_DIR}/etc/systemd/system/mctallyd.service: mctallyd.service
	../../../../install-sh -c -m 644 $< $@

clean:: clean-tarprep

clean-tarprep::
	rm -f ${TARTARGETS}

