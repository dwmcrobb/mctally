include Makefile.vars

all:: apps

apps: classes/lib/libDwmMcTally.la
	${MAKE} -C apps

classes/lib/libDwmMcTally.la::
	${MAKE} -C classes

tarprep:: classes/lib/libDwmMcTally.la apps
	${MAKE} -C classes $@
	${MAKE} -C apps $@
ifeq ("${BUILD_DOCS}", "yes")
	${MAKE} -C doc $@
endif
	${MAKE} -C packaging $@

package: ${OSNAME}-pkg

freebsd-pkg: tarprep
	mkfbsdmnfst -r packaging/fbsd_manifest -s staging classes/tests > staging/+MANIFEST
	pkg create -o . -r staging -m staging

darwin-pkg: tarprep
	pkgbuild --root staging --identifier net.mcplex.mctally --version ${VERSION} mctally-${VERSION}.pkg

linux-pkg: tarprep
	if [ ! -d staging/DEBIAN ]; then mkdir staging/DEBIAN; fi
	mkdebcontrol -r packaging/debcontrol -s staging/usr/local classes/tests > staging/DEBIAN/control
	dpkg-deb -b --root-owner-group staging
	dpkg-name -o staging.deb

clean::
	${MAKE} -C apps $@
	${MAKE} -C classes $@

distclean:: clean
	${MAKE} -C classes $@
	${MAKE} -C doc $@
	${MAKE} -C packaging $@
	rm -Rf autom4te.cache staging
	rm -f config.log config.status Makefile.vars
	rm -f mctally_*.deb mctally-*.pkg

