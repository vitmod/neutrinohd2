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

noinst_LTLIBRARIES = vlcplayer.la

vlcplayer_la_SOURCES = vlcplayer.cpp
	 
vlcplayer_la_LDFLAGS = -rpath $(PLUGINDIR) -module

vlcplayer_la_CXXFLAGS = -fno-rtti -fno-exceptions

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)/vlcplayer
	install .libs/vlcplayer.so $(DESTDIR)$(PLUGINDIR)/vlcplayer
	install -m 0644 vlcplayer.cfg $(DESTDIR)$(PLUGINDIR)/vlcplayer

uninstall-local:
	-rm $(DESTDIR)$(PLUGINDIR)/vlcplayer/vlcplayer.so
	-rm $(DESTDIR)$(PLUGINDIR)/vlcplayer/vlcplayer.cfg
	-rmdir $(DESTDIR)$(PLUGINDIR)/vlcplayer





