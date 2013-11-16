####################################################
# Makefile for building native neutrino / libstb-hal
# (C) 2012,2013 Stefan Seyfried
#
# prerequisite packages need to be installed,
# no checking is done for that
# 2013.05.24 modded by mohousch to build nhd2-exp (mipsel)
####################################################

BOXTYPE = gb800se

HOST = mipsel-oe-linux
CDK = $(PWD)/../build-$(BOXTYPE)/tmp/sysroots/i686-linux
TARGETPREFIX = $(PWD)/../build-$(BOXTYPE)/tmp/sysroots/mipsel-oe-linux

PATH := $(CDK)/usr/mipsel/bin/:$(PATH)

TARGET = $(CDK)/usr/mipsel/$(HOST)

CC = $(TARGET)-gcc
CPP = $(TARGET)-cpp
CXX = $(TARGET)-g++
LD = $(TARGET)-ld
AR = $(TARGET)-ar
NM = $(TARGET)-nm
AS = $(TARGET)-as
RANLIB = $(TARGET)-ranlib
OBJDUMP = $(TARGET)-objdump
BJCOPY = $(TARGET)-objcopy
STRIP = $(TARGET)-strip

DVBINCLUDES = $(TARGETPREFIX)/usr/include/linux/dvb

N_CFLAGS = "-Wall -g0 -O2 \
		-D__KERNEL_STRICT_NAMES \
		-I$(TARGETPREFIX)/usr/include -I$(TARGETPREFIX)/include -I$(DVBINCLUDES) -I$(TARGETPREFIX)/usr/include/gstreamer-0.10"

N_CXXFLAGS = "-Wall -g0 -O2 \
		-D__KERNEL_STRICT_NAMES \
		 -I$(TARGETPREFIX)/usr/include -I$(TARGETPREFIX)/include -I$(DVBUNCLUDES) -I$(TARGETPREFIX)/usr/include/gstreamer-0.10"

N_LDFLAGS = "-L$(TARGETPREFIX)/lib -L$(TARGETPREFIX)/usr/lib -Wl,-rpath-link,-lcurl -ldl"

FREETYPE_CONFIG = $(CDK)/usr/mipsel/bin/freetype-config
CURL_CONFIG = $(CDK)/usr/mipsel/bin/curl-config

export PKG_CONFIG_PATH = $(TARGETPREFIX)/usr/lib/pkgconfig
export PKG_CONFIG_LIBDIR = "$(TARGETPREFIX)/usr/lib/pkgconfig"

DEST = $(PWD)/$(BOXTYPE)

N_SRC  = $(PWD)/nhd2-exp

neutrino: $(N_SRC)/config.status
	$(MAKE) -C $(N_SRC) install DESTDIR=$(DEST)

$(N_SRC)/config.status: | $(N_SRC) $(DEST)
	$(N_SRC)/autogen.sh \
	set -e; cd $(N_SRC); \
		CFLAGS=$(N_CFLAGS) \
		CXXFLAGS=$(N_CFLAGS) \
		LDFLAGS=$(N_LDFLAGS) \
		$(N_SRC)/configure \
			--prefix=/ \
			--exec_prefix=/usr \
			--build=i686-pc-linux-gnu \
			--host=$(HOST) \
			--with-target=cdk \
			--enable-maintainer-mode \
			--without-debug \
			--with-boxtype=$(BOXTYPE) \
			--with-bindir=/usr/bin \
			--with-sbindir=/usr/sbin \
			--with-libdir=/usr/lib \
			--with-gamesdir=/etc/tuxbox/games \
			--with-plugindir=/etc/tuxbox/plugins \
			--with-configdir=/etc/tuxbox/config \
			--enable-radiotext \
			--enable-freesatepg \
			--enable-upnp \
			--enable-ci \
			--enable-gstreamer

$(DEST):
	mkdir $@

$(N_SRC):
	svn co http://neutrinohd2.googlecode.com/svn/branches/nhd2-exp nhd2-exp

neutrino-checkout: $(N_SRC)

neutrino-update:
	svn update http://neutrinohd2.googlecode.com/svn/branches/nhd2-exp nhd2-exp

neutrino-clean:
	-$(MAKE) -C $(N_SRC) clean

neutrino-distclean:
	-$(MAKE) -C $(N_SRC) distclean
	rm -f $(N_SRC)/config.status

usb-image: $(BOXTYPE).tar.gz

$(BOXTYPE).tar.gz: neutrino
	cd $(DEST) && \
	tar -cf $(BOXTYPE).tar *; \
	gzip $(BOXTYPE).tar; \
	mv -f $(BOXTYPE).tar.gz $(PWD)/$@

PHONY = neutrino-checkout
.PHONY: $(PHONY)
