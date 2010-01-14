DIST_SUBDIRS = ru

if USE_NLS
   SUBDIRS = $(DIST_SUBDIRS)
else
   SUBDIRS =
endif

man_MANS = texpack.1
man_XMANS = texpack.1.xml

EXTRA_DIST = $(man_MANS) $(man_XMANS)

if REGENERATE_MAN

texpack.1: texpack.1.xml
	$(XSLTPROC) -nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $<

CLEANFILES = $(man_MANS)
endif