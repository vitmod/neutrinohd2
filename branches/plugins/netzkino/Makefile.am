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

noinst_LTLIBRARIES = netzkino.la

netzkino_la_SOURCES = nkparser.cpp netzkino.cpp
	 
netzkino_la_LDFLAGS = -rpath $(PLUGINDIR) -module

netzkino_la_CXXFLAGS = -fno-rtti -fno-exceptions

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install .libs/netzkino.so $(DESTDIR)$(PLUGINDIR)
	install -m 0644  netzkino.cfg $(DESTDIR)$(PLUGINDIR)
	install -m 0644  netzkino.png $(DESTDIR)$(PLUGINDIR)

uninstall-local:
	-rm $(DESTDIR)$(PLUGINDIR)/netzkino.so
	-rm $(DESTDIR)$(PLUGINDIR)/netzkino.cfg
	-rm $(DESTDIR)$(PLUGINDIR)/netzkino.png



