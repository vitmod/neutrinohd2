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
	-I$(top_srcdir)/../nhd2-exp/lib/jsoncpp/include \
	@FREETYPE_CFLAGS@ 

noinst_LTLIBRARIES = mediaportal.la

mediaportal_la_SOURCES = mediaportal.cpp nkparser.cpp netzkino.cpp ytparser.cpp youtube.cpp
	 
mediaportal_la_LDFLAGS = -rpath $(PLUGINDIR) -module

mediaportal_la_CXXFLAGS = -fno-rtti -fno-exceptions

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)/mediaportal
	install .libs/mediaportal.so $(DESTDIR)$(PLUGINDIR)/mediaportal
	install -m 0644 mediaportal.cfg $(DESTDIR)$(PLUGINDIR)/mediaportal
	install -m 0644 mp.png $(DESTDIR)$(PLUGINDIR)/mediaportal
	install -m 0644 deluxemusic.png $(DESTDIR)$(PLUGINDIR)/mediaportal
	install -m 0644 filmon.png $(DESTDIR)$(PLUGINDIR)/mediaportal
	install -m 0644 orf.png $(DESTDIR)$(PLUGINDIR)/mediaportal
	install -m 0644  netzkino.png $(DESTDIR)$(PLUGINDIR)/mediaportal
	install -m 0644  youtube.png $(DESTDIR)$(PLUGINDIR)/mediaportal

uninstall-local:
	-rm $(DESTDIR)$(PLUGINDIR)/mediaportal/mediaportal.so
	-rm $(DESTDIR)$(PLUGINDIR)/mediaportal/mediaportal.cfg
	-rm $(DESTDIR)$(PLUGINDIR)/mediaportal/mp.png
	-rm $(DESTDIR)$(PLUGINDIR)/mediaportal/deluxemusic.png
	-rm $(DESTDIR)$(PLUGINDIR)/mediaportal/filmon.png
	-rm $(DESTDIR)$(PLUGINDIR)/mediaportal/orf.png
	-rm $(DESTDIR)$(PLUGINDIR)/mediaportal/netzkino.png
	-rm $(DESTDIR)$(PLUGINDIR)/mediaportal/youtube.png
	-rmdir $(DESTDIR)$(PLUGINDIR)/mediaportal



