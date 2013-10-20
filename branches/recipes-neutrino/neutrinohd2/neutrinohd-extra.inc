require neutrinohd-bin.inc

PR_append = ".12"

dirs755 = "\
	/usr/bin \
	/usr/sbin \
	/usr/lib \
	/share/fonts \
	/share/iso-codes \
	/share/tuxbox/lcdd/icons \
	/share/tuxbox/neutrino/httpd \
	/share/tuxbox/neutrino/httpd/images \
	/share/tuxbox/neutrino/httpd/languages \
	/share/tuxbox/neutrino/httpd/scripts \
	/share/tuxbox/neutrino/icons \
	/share/tuxbox/neutrino/locale \
	/share/tuxbox/neutrino/themes \
	/share/tuxbox/tuxwetter \
	/etc/tuxbox/config \
	/etc/tuxbox/config/tuxtxt \
	/etc/tuxbox/config/zapit \
	/etc/tuxbox/plugins \
	"

FILES_${PN} = "${dirs755}"

inherit autotools

do_install_append() {
	#install -m 0644 ${WORKDIR}/etc/.version ${D}/etc/.version
	# generate /.version
	echo "version=1200${DATETIME:0:12}" > ${D}/etc/.version
	echo "creator=${DISTRO_NAME}" >> ${D}/etc/.version
	echo "imagename=neutrinoHD2" >> ${D}/etc/.version
	echo "homepage=${URL}" >> ${D}/etc/.version
}

pkg_preinst_${PN} () {
}

pkg_postinst_${PN} () {
#!/bin/sh
cd $D/usr/lib
if [ ! -e libavcodec.so.52  ]; then if [ -e libavcodec.so.53  ]; then ln -s libavcodec.so.53  libavcodec.so.52;  fi; fi
if [ ! -e libavformat.so.52 ]; then if [ -e libavformat.so.53 ]; then ln -s libavformat.so.53 libavformat.so.52; fi; fi
if [ ! -e libavutil.so.49   ]; then if [ -e libavutil.so.51   ]; then ln -s libavutil.so.51   libavutil.so.49;   fi; fi
cd - > /dev/null
echo " "
echo " "
echo " "
echo " "
echo " "
echo "neutrinoHD2 successfully installed!"
echo " "
echo "have fun with neutrino!!!"
#sleep 15
echo " "
echo " please restart your STB!!!"
}

pkg_postrm_${PN} () {
#!/bin/sh

rm -rf /usr/bin/neutrino
rm -rf /usr/bin/pzapit
rm -rf /usr/bin/sectionsdcontrol
rm -rf /usr/bin/backup.sh
rm -rf /usr/bin/restore.sh
rm -rf /usr/bin/install.sh
rm -rf /usr/bin/init_hdd.sh

rm -rf /usr/sbin/udpstreampes

rm -rf /share/fonts/neutrino.ttf

rm -rf /share/iso-codes/iso-639.tab
rm -R /share/tuxbox/neutrino/

rm -rf /etc/tuxbox/config/Y-Web.conf
rm -rf /etc/tuxbox/config/cables.xml
rm -rf /etc/tuxbox/config/moviebrowser.conf
rm -rf /etc/tuxbox/config/nhttpd.conf
rm -rf /etc/tuxbox/config/radio-stations.xml
rm -rf /etc/tuxbox/config/satellites.xml
rm -rf /etc/tuxbox/config/terrestrial.xml
rm -rf /etc/tuxbox/config/timerd.conf
rm -rf /etc/tuxbox/config/giga.conf

rm -R /etc/tuxbox/config/tuxtxt/
rm -R /etc/tuxbox/config/zapit/
rm -R /etc/tuxbox/plugins

echo " "
echo " "
echo " "
echo " "
echo " "
echo "neutrino successfully removed! please restart enigma2!"
sleep 15
exit 0
}

