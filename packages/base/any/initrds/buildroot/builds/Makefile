############################################################
# <bsn.cl fy=2013 v=none>
#
#        Copyright 2013, 2014 BigSwitch Networks, Inc.
#
#
#
# </bsn.cl>
############################################################
#
# ONL base buildroot cpio.
#
############################################################
include $(ONL)/make/config.mk

#
# We build for these architectures
#
ARCHS := x86_64
BUILDROOT_ARCHDIRS := $(foreach a,$(ARCHS),buildroot-$(a))
BUILDROOT_SOURCE := buildroot-mirror

.PHONY: all clean setup $(BUILDROOT_ARCHDIRS)

all: setup $(BUILDROOT_ARCHDIRS)

clean:
	rm -rf $(BUILDROOT_ARCHDIRS)
	rm -rf .setup.done

setup:
	mkdir -p $(BUILDROOT_ARCHDIRS)
	$(foreach a,$(ARCHS),cp buildroot.config-$(a) buildroot-$(a)/.config ;)

setup-jq:
	mkdir -p $(BUILDROOT_SOURCE)/package/jq
	cp patches/jq.mk $(BUILDROOT_SOURCE)/package/jq/jq.mk
	cp patches/jq.Config.in $(BUILDROOT_SOURCE)/package/jq/Config.in
	cp patches/jq.hash $(BUILDROOT_SOURCE)/package/jq/jq.hash
	sed -i '/[/]jq[/]/d' $(BUILDROOT_SOURCE)/package/Config.in
	sed -i '/[/]yajl[/]/a\source "package/jq/Config.in"' $(BUILDROOT_SOURCE)/package/Config.in

setup-pyroute2:
	mkdir -p $(BUILDROOT_SOURCE)/package/python-pyroute2
	cp patches/python-pyroute2.mk $(BUILDROOT_SOURCE)/package/python-pyroute2/python-pyroute2.mk
	cp patches/python-pyroute2.Config.in $(BUILDROOT_SOURCE)/package/python-pyroute2/Config.in
	sed -i '/[/]python-pyroute2[/]/d' $(BUILDROOT_SOURCE)/package/Config.in
	sed -i '/[/]python-setuptools[/]/a\source "package/python-pyroute2/Config.in"' $(BUILDROOT_SOURCE)/package/Config.in

setup-dnspython:
	mkdir -p $(BUILDROOT_SOURCE)/package/python-dnspython
	cp patches/python-dnspython.mk $(BUILDROOT_SOURCE)/package/python-dnspython/python-dnspython.mk
	cp patches/python-dnspython.Config.in $(BUILDROOT_SOURCE)/package/python-dnspython/Config.in
	sed -i '/[/]python-dnspython[/]/d' $(BUILDROOT_SOURCE)/package/Config.in
	sed -i '/[/]python-setuptools[/]/a\source "package/python-dnspython/Config.in"' $(BUILDROOT_SOURCE)/package/Config.in

setup-pyyaml:
	mkdir -p $(BUILDROOT_SOURCE)/package/python-yaml
	cp patches/python-yaml.mk $(BUILDROOT_SOURCE)/package/python-yaml/python-yaml.mk
	cp patches/python-yaml.Config.in $(BUILDROOT_SOURCE)/package/python-yaml/Config.in
	sed -i '/[/]python-yaml[/]/d' $(BUILDROOT_SOURCE)/package/Config.in
	sed -i '/[/]python-setuptools[/]/a\source "package/python-yaml/Config.in"' $(BUILDROOT_SOURCE)/package/Config.in
	sed -i 's/pyyaml\//pyyaml/g' $(BUILDROOT_SOURCE)/package/python-yaml/python-yaml.mk

setup-libyaml:
	cp patches/libyaml.mk $(BUILDROOT_SOURCE)/package/libyaml/libyaml.mk

setup-pyparted:
	mkdir -p $(BUILDROOT_SOURCE)/package/python-pyparted
	cp patches/python-pyparted.mk $(BUILDROOT_SOURCE)/package/python-pyparted/python-pyparted.mk
	cp patches/python-pyparted.Config.in $(BUILDROOT_SOURCE)/package/python-pyparted/Config.in

define buildroot_arch
buildroot-$(1):
	$(MAKE) -C $(BUILDROOT_SOURCE) O=../buildroot-$(1)

buildroot-menuconfig-$(1):
	$(MAKE) -C $(BUILDROOT_SOURCE) menuconfig O=../buildroot-$(1)
	cp buildroot-$(1)/.config buildroot.config-$(1)
endef

$(foreach a,$(ARCHS),$(eval $(call buildroot_arch,$(a))))

busybox-menuconfig:
	$(MAKE) -C $(BUILDROOT_SOURCE) busybox-menuconfig O=../buildroot-powerpc
	cp buildroot-powerpc/build/busybox-*/.config busybox.config
