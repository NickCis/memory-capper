AM_DISTCHECK_CONFIGURE_FLAGS = \
  --with-systemdsystemunitdir=$$dc_install_base/$(systemdsystemunitdir)

bin_PROGRAMS = memory-capper
memory_capper_SOURCES = main.c
dist_doc_DATA = README.md

.PHONY: reload
reload:
	-systemctl daemon-reload

.PHONY: disable
disable:
	systemctl disable funk.service
	systemctl disable funk_rw.socket
	systemctl disable funk_ro.socket

.PHONY: enable
enable:
	make disable
	systemctl daemon-reload
	systemctl enable funk.service
	systemctl enable funk_rw.socket
	systemctl enable funk_ro.socket

## https://stackoverflow.com/a/58028204
SERVICE_SUBS = \
	s,[@]prefix[@],$(prefix),g

memory-capper.service: memory-capper.service.in
	sed -e '$(SERVICE_SUBS)' < $< > $@

systemdsystemunitdir = @systemdsystemunitdir@
systemdsystemunit_DATA = \
	memory-capper.service
