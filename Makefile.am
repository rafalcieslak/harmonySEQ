AUTOMAKE_OPTIONS = foreign

SUBDIRS = po examples pixmaps src

ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST = config.rpath CHANGELOG LICENSE harmonyseq.desktop

appsmenudir = $(datadir)/applications
appsmenu_DATA = harmonyseq.desktop

mimedir = $(datadir)/mime/packages
mime_DATA = harmonyseq-mime.xml

install-data-hook:
	update-mime-database /usr/share/mime
	gtk-update-icon-cache -f /usr/share/icons/gnome

uninstall-hook:
	update-mime-database /usr/share/mime
	gtk-update-icon-cache -f /usr/share/icons/gnome
