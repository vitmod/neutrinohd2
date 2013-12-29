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

noinst_LTLIBRARIES = movieplayer.la

movieplayer_la_SOURCES = movieplayer.cpp
	 
movieplayer_la_LDFLAGS = -rpath $(PLUGINDIR) -module

movieplayer_la_CXXFLAGS = -fno-rtti -fno-exceptions

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install .libs/movieplayer.so $(DESTDIR)$(PLUGINDIR)
	install -m 0644  movieplayer.cfg $(DESTDIR)$(PLUGINDIR)

uninstall-local:
	-rm $(DESTDIR)$(PLUGINDIR)/movieplayer.so
	-rm $(DESTDIR)$(PLUGINDIR)/movieplayer.cfg




