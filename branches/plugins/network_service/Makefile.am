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

noinst_LTLIBRARIES = network_service.la

network_service_la_SOURCES = network_service.cpp
	 
network_service_la_LDFLAGS = -rpath $(PLUGINDIR) -module

network_service_la_CXXFLAGS = -fno-rtti -fno-exceptions

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install .libs/network_service.so $(DESTDIR)$(PLUGINDIR)
	install -m 0644  network_service.cfg $(DESTDIR)$(PLUGINDIR)

uninstall-local:
	-rm $(DESTDIR)$(PLUGINDIR)/network_service.so
	-rm $(DESTDIR)$(PLUGINDIR)/network_service.cfg

	




