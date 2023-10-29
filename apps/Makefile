APPS      = mctally
APPSCLEAN = $(APPS:%=%-clean)
DISTCLEAN = $(APPS:%=%-distclean)

all:: ${APPS}

${APPS}::
	${MAKE} -C $@

distclean:: ${DISTCLEAN}
${DISTCLEAN}::
	${MAKE} -C $(patsubst %-distclean,%,$@) distclean

clean:: ${APPSCLEAN}
${APPSCLEAN}::
	${MAKE} -C $(patsubst %-clean,%,$@) clean

