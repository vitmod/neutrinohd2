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

noinst_LTLIBRARIES = webtv.la

webtv_la_SOURCES = webtv.cpp
	 
webtv_la_LDFLAGS = -rpath $(PLUGINDIR) -module

webtv_la_CXXFLAGS = -fno-rtti -fno-exceptions

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install .libs/webtv.so $(DESTDIR)$(PLUGINDIR)
	install -m 0644  webtv.cfg $(DESTDIR)$(PLUGINDIR)
	install -m 0644  webtv.png $(DESTDIR)$(PLUGINDIR)

uninstall-local:
	-rm $(DESTDIR)$(PLUGINDIR)/webtv.so
	-rm $(DESTDIR)$(PLUGINDIR)/webtv.cfg
	-rm $(DESTDIR)$(PLUGINDIR)/webtv.png




