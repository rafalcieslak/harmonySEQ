AUTOMAKE_OPTIONS = foreign

SUBDIRS = po examples pixmaps src

ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST = config.rpath CHANGELOG LICENSE harmonyseq.desktop harmonyseq-mime.xml

appsmenudir = $(datadir)/applications
appsmenu_DATA = harmonyseq.desktop

mimedir = $(datadir)/mime/packages
mime_DATA = harmonyseq-mime.xml

install-data-hook:
	update-mime-database $(datadir)/mime
	update-desktop-database
	gtk-update-icon-cache -f -t $(datadir)/icons/gnome

uninstall-hook:
	update-mime-database $(datadir)/mime
	update-desktop-database
	gtk-update-icon-cache -f -t $(datadir)/icons/gnome
