###############################################################################
#
# 
#
###############################################################################

LIBRARY := powerpc_quanta_ly2
$(LIBRARY)_SUBDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(BUILDER)/lib.mk