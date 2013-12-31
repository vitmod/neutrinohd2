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

noinst_LTLIBRARIES = fbcallmonitor.la

fbcallmonitor_la_SOURCES = fbcallmonitor.cpp
	 
fbcallmonitor_la_LDFLAGS = -rpath $(PLUGINDIR) -module

fbcallmonitor_la_CXXFLAGS = -fno-rtti -fno-exceptions

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install .libs/fbcallmonitor.so $(DESTDIR)$(PLUGINDIR)
	install -m 0644 fbcallmonitor.cfg $(DESTDIR)$(PLUGINDIR)
	install -m 0644 fritzbox.png $(DESTDIR)$(PLUGINDIR)
	install -m 0755 fb.sh $(DESTDIR)$(PLUGINDIR)
	install -m 0644 fb.csv $(DESTDIR)$(PLUGINDIR)

uninstall-local:
	-rm $(DESTDIR)$(PLUGINDIR)/fbcallmonitor.so
	-rm $(DESTDIR)$(PLUGINDIR)/fbcallmonitor.cfg
	-rm $(DESTDIR)$(PLUGINDIR)/fritzbox.png
	-rm $(DESTDIR)$(PLUGINDIR)/fb.sh
	-rm $(DESTDIR)$(PLUGINDIR)/fb.csv

	




