#SUBDIRS = daemon

AM_CPPFLAGS =  \
	-I$(top_srcdir)/../nhd2-exp/src \
	-I$(top_srcdir)/../nhd2-exp/src/zapit/include/zapit \
	-I$(top_srcdir)/../nhd2-exp/lib \
	-I$(top_srcdir)/../nhd2-exp/lib/connection \
	-I$(top_srcdir)/../nhd2-exp/lib/libeventserver \
	-I$(top_srcdir)/../nhd2-exp/lib/libconfigfile \
	-I$(top_srcdir)/../nhd2-exp/lib/libnet \
	-I$(top_srcdir)/../nhd2-exp/lib/libxmltree \
	-I$(top_srcdir)/../nhd2-exp/lib/libmd5sum \
	-I$(top_srcdir)/../nhd2-exp/lib/libdvbapi \
	-I$(top_srcdir)/../nhd2-exp/lib/libupnpclient \
	@FREETYPE_CFLAGS@

noinst_LTLIBRARIES = tuxcal.la

tuxcal_la_SOURCES = tuxcal.cpp

tuxcal_la_LIBADD = \
	@FREETYPE_LIBS@ \
	-lz

tuxcal_la_LDFLAGS = -rpath $(PLUGINDIR) -module -avoid-version

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install -d $(DESTDIR)$(CONFIGDIR)/tuxcal
	install .libs/tuxcal.so $(DESTDIR)$(PLUGINDIR)
	install -m 0644 $(srcdir)/tuxcal.cfg $(DESTDIR)$(PLUGINDIR)
	install -m 0644 $(srcdir)/tuxcal.conf $(DESTDIR)$(CONFIGDIR)/tuxcal
