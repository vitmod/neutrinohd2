####################################################
# Makefile for building native neutrino / libstb-hal
# (C) 2012,2013 Stefan Seyfried
#
# prerequisite packages need to be installed,
# no checking is done for that
# 2013.05.24 modded by mohousch to build nhd2-exp
####################################################

BOXTYPE = generic
DEST = $(PWD)/$(BOXTYPE)

N_SRC  = $(PWD)/nhd2-exp

CFLAGS =  -funsigned-char -g -W -Wall -Wshadow -O2
CFLAGS += -rdynamic
CFLAGS += -DPEDANTIC_VALGRIND_SETUP
### enable --as-needed for catching more build problems...
CFLAGS += -Wl,--as-needed

CXXFLAGS = $(CFLAGS)

export CFLAGS CXXFLAGS

# first target is default...
default: neutrino
	make run

run:
	gdb -ex run $(DEST)/bin/neutrino

neutrino: $(N_SRC)/config.status
	$(MAKE) -C $(N_SRC) install

$(N_SRC)/config.status: | $(N_SRC) $(DEST)
	$(N_SRC)/autogen.sh
	set -e; cd $(N_SRC); \
		$(N_SRC)/configure \
			--prefix=$(DEST)  \
			--build=i686-pc-linux-gnu \
			--enable-maintainer-mode \
			--with-debug \
			--with-boxtype=generic \
			--with-datadir=$(DEST)/share/tuxbox \
			--with-fontdir=$(DEST)/share/fonts \
			--with-gamesdir=$(DEST)/var/tuxbox/games \
			--with-plugindir=$(DEST)/var/tuxbox/plugins \
			--with-configdir=$(DEST)/var/tuxbox/config \
			--with-isocodesdir=$(DEST)/share/iso-codes \
			--enable-opengl \
			--enable-gstreamer \
			--enable-freesatepg \
			--enable-upnp \
			--enable-radiotext \
			--enable-netzkino
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

PHONY = neutrino-checkout
.PHONY: $(PHONY)
