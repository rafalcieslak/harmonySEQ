AUTOMAKE_OPTIONS = foreign

SUBDIRS = po examples pixmaps src

ACLOCAL_AMFLAGS = -I m4

.PHONY: harmonyseq.desktop

EXTRA_DIST = config.rpath CHANGELOG LICENSE harmonyseq.desktop harmonyseq-mime.xml README

appsmenudir = $(datadir)/applications
appsmenu_DATA = harmonyseq.desktop

mimedir = $(datadir)/mime/packages
mime_DATA = harmonyseq-mime.xml

harmonyseq.desktop: harmonyseq.desktop.in
	sed 's:@PREFIXPATH@:$(prefix):' harmonyseq.desktop.in > harmonyseq.desktop
	chmod +x harmonyseq.desktop
