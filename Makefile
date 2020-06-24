###########################################################
#
# Work in progress.
#
############################################################
ifneq ($(MAKECMDGOALS),docker)
ifneq ($(MAKECMDGOALS),docker-debug)

ifndef ONL
$(error Please source the setup.env script at the root of the ONL tree)
endif

include $(ONL)/make/config.mk

# All available architectures.
ALL_ARCHES := amd64 powerpc arm64 armel

# Build rule for each architecture.
define build_arch_template
$(1) :
	$(MAKE) -C builds/$(1)
endef
$(foreach a,$(ALL_ARCHES),$(eval $(call build_arch_template,$(a))))


# Available build architectures based on the current suite
BUILD_ARCHES_wheezy := amd64 powerpc
BUILD_ARCHES_jessie := amd64
BUILD_ARCHES_stretch := amd64

# Build available architectures by default.
.DEFAULT_GOAL := all
all: $(BUILD_ARCHES_$(ONL_DEBIAN_SUITE))


rebuild:
	$(ONLPM) --rebuild-pkg-cache


modclean:
	rm -rf $(ONL)/make/modules/modules.*

endif
endif

.PHONY: docker

ifndef VERSION
VERSION := 9
endif

docker_check:
	@which docker > /dev/null || (echo "*** Docker appears to be missing. Please install docker.io in order to build OpenNetworkLinux." && exit 1)

docker: docker_check
	@docker/tools/onlbuilder -$(VERSION) --isolate --hostname onlbuilder$(VERSION) --pull --autobuild --non-interactive

# create an interative docker shell, for debugging builds
docker-debug: docker_check
	@docker/tools/onlbuilder -$(VERSION) --isolate --hostname onlbuilder$(VERSION) --pull


versions:
	$(ONL)/tools/make-versions.py --import-file=$(ONL)/tools/onlvi --class-name=OnlVersionImplementation --output-dir $(ONL)/make/versions --force

relclean:
	@find $(ONL)/RELEASE -name "ONL-*" -delete


ONLP_DEV_TAR_NAME := onlp-dev_1.1.0_amd64
ONLP_BUILD := $(ONL)/packages/base/amd64/onlp/builds
TEMP_DIR := /tmp/$(ONLP_DEV_TAR_NAME)
onlp-dev:
	make -C $(ONLP_BUILD)
	mkdir -p $(TEMP_DIR)/lib $(TEMP_DIR)/include
	# Share libraries
	cp $(ONLP_BUILD)/*/BUILD/*/x86_64-linux-gnu/bin/*.so $(TEMP_DIR)/lib
	# Include files
	cp -r $(ONL)/sm/infra/modules/AIM/module/inc/* $(TEMP_DIR)/include
	cp -r $(ONL)/sm/bigcode/modules/BigData/BigList/module/inc/BigList $(TEMP_DIR)/include
	cp -r $(ONL)/sm/bigcode/modules/**/module/inc/* $(TEMP_DIR)/include
	cp -r $(ONL)/packages/base/any/onlp/src/*/module/inc/* $(TEMP_DIR)/include
	# Patch sff.h
	sed -i '/dependmodules.x/d' $(TEMP_DIR)/include/sff/sff.h
	# Package as tar.gz
	tar -zcf $(ONLP_DEV_TAR_NAME).tar.gz -C /tmp $(ONLP_DEV_TAR_NAME)
	rm -rf $(TEMP_DIR)
