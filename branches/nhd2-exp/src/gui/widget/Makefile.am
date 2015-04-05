INCLUDES = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/zapit/include/zapit \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/lib/libnet \
	-I$(top_srcdir)/lib/libconfigfile \
	-I$(top_srcdir)/lib/libxmltree \
	@FREETYPE_CFLAGS@

if BOXTYPE_COOLSTREAM
INCLUDES += -I$(top_srcdir)/src/zapit/include \
	-I$(top_srcdir)/lib/libcoolstream
else
INCLUDES += -I$(top_srcdir)/lib/libdvbapi
endif

noinst_LIBRARIES = libneutrino_gui_widget.a libneutrino_gui_widget2.a 

libneutrino_gui_widget_a_SOURCES = \
	drawable.cpp \
	buttons.cpp \
	menue.cpp \
	stringinput.cpp stringinput_ext.cpp \
	msgbox.cpp messagebox.cpp \
	hintbox.cpp hintboxext.cpp \
	helpbox.cpp \
	listbox.cpp \
	textbox.cpp \
	listframe.cpp

libneutrino_gui_widget2_a_SOURCES = \
	colorchooser.cpp keychooser.cpp mountchooser.cpp vfdcontroler.cpp \
	progresswindow.cpp progressbar.cpp



