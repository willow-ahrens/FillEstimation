#
# Main project files
#
# TO DO: Add tests.
#
# Omitted:
#	gen gen/driver
#	include/oski/BDIAG1
#	src/BDIAG1
#

include src/Make.extlibs

SUBDIRS = libltdl \
	lua/src lua/src/lib lua/include \
	include/oski \
	include/oski/CSR \
	include/oski/CSC \
	include/oski/BCSR \
	include/oski/MBCSR \
	include/oski/GCSR \
	include/oski/CB \
	include/oski/DENSE \
	include/oski/VBR \
	include/oski/heur \
	src/corelib \
	src/timer \
	src/heur \
	src \
	src/CSR \
	src/CSC \
	src/GCSR \
	src/BCSR \
	src/MBCSR \
	src/CB \
	src/DENSE \
	src/VBR \
	src/heur/mregblock \
	src/heur/regprof \
	src/heur/symmrb \
	src/lt \
	util \
	bench \
	tests \
	doc

EXTRA_DIST = oski-ug.pdf

doxdir = $(top_builddir)/doc

docs:
	echo Making $@ in $(doxdir)
	cd $(doxdir) && $(MAKE) dox

benchmarks:
	echo Running off-line benchmarks...
	cd bench && $(MAKE) benchmarks

site_modules_files = site-modules-static.txt site-modules-shared.txt \
					 site-modules-mats.txt site-modules-heur.txt

site-modules-static.txt:
	rm -f $@
	touch $@
	for p in $(site_modules_static) ; do \
		echo $$p >> $@ ; \
	done

site-modules-shared.txt:
	rm -f $@
	touch $@
	for p in $(site_modules_shared) ; do \
		echo $$p >> $@ ; \
	done

site-modules-mats.txt:
	rm -f $@
	touch $@
	for p in $(site_modules_core) ; do \
		base= ; \
		case "$$p" in \
			oski_mat_*_T??) base=`echo "$$p" | sed 's,^oski_mat_,,' | sed 's,_T..$$,,'` ;; \
		esac ; \
		itype= ; \
		case "$$p" in \
			*_Ti?) itype=1 ;; \
			*_Tl?) itype=2 ;; \
		esac ; \
		vtype= ; \
		case "$$p" in \
			*_T?s) vtype=1 ;; \
			*_T?d) vtype=2 ;; \
			*_T?c) vtype=3 ;; \
			*_T?z) vtype=4 ;; \
		esac ; \
		if test -n "$$base" && test -n "$$itype" && test -n "$$vtype" ; then \
			echo "$$base $$itype $$vtype" >> $@ ; \
		fi \
	done

site-modules-heur.txt:
	rm -f $@
	touch $@
	for p in $(site_modules_heur) ; do \
		base= ; \
		case "$$p" in \
			oski_heur_*_T??) base=`echo "$$p" | sed 's,^oski_heur_,,' | sed 's,_T..$$,,'` ;; \
		esac ; \
		itype= ; \
		case "$$p" in \
			*_Ti?) itype=1 ;; \
			*_Tl?) itype=2 ;; \
		esac ; \
		vtype= ; \
		case "$$p" in \
			*_T?s) vtype=1 ;; \
			*_T?d) vtype=2 ;; \
			*_T?c) vtype=3 ;; \
			*_T?z) vtype=4 ;; \
		esac ; \
		if test -n "$$base" && test -n "$$itype" && test -n "$$vtype" ; then \
			echo "$$base $$itype $$vtype" >> $@ ; \
		fi \
	done

install-exec-hook: $(site_modules_files)
	test -z "$(pkglibdir)" || $(mkdir_p) "$(DESTDIR)$(pkglibdir)"
	$(INSTALL_DATA) $(site_modules_files) "$(DESTDIR)$(pkglibdir)/."

uninstall-local:
	for p in $(site_modules_files) ; do \
		pbase=`basename $$p` ; \
		rm -f "$(DESTDIR)/$(pkglibdir)/$$p" ; \
	done

distclean-local:
	-rm -rf $(site_modules_files) csymx.sh

# eof
