METASOURCES = AUTO

mandir = @mandir@/ru
man_MANS = texpack.1
man_XMANS = texpack.1.xml

EXTRA_DIST = $(man_MANS) $(man_XMANS)

if REGENERATE_MAN

texpack.1.utf8: texpack.1.xml
	$(XSLTPROC) -nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $<
	mv texpack.1 texpack.1.utf8

texpack.1: texpack.1.utf8
	iconv -f UTF-8 -t KOI8-R $< >$@

CLEANFILES = $(man_MANS)

endif

clean-local:
	rm -f $(man_MANS)
