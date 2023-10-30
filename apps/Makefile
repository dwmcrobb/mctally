APPS      = mctally mctallyd
APPSCLEAN = $(APPS:%=%-clean)
DISTCLEAN = $(APPS:%=%-distclean)
TARPREP   = $(APPS:%=%-tarprep)

all:: ${APPS}

${APPS}::
	${MAKE} -C $@

tarprep:: ${TARPREP}
${TARPREP}::
	${MAKE} -C $(patsubst %-tarprep,%,$@) tarprep

distclean:: ${DISTCLEAN}
${DISTCLEAN}::
	${MAKE} -C $(patsubst %-distclean,%,$@) distclean

clean:: ${APPSCLEAN}
${APPSCLEAN}::
	${MAKE} -C $(patsubst %-clean,%,$@) clean

