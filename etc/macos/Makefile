include ../../Makefile.vars

TARTARGETS  = ${STAGING_DIR}/Library/LaunchDaemons/net.mcplex.mctallyd.plist

all::
	echo "Nothing to be done for 'all'."

tarprep: ${TARTARGETS}

${STAGING_DIR}/Library/LaunchDaemons/net.mcplex.mctallyd.plist: net.mcplex.mctallyd.plist
	../../install-sh -c -m 755 $< $@

clean:: clean-tarprep

clean-tarprep::
	rm -f ${TARTARGETS}
