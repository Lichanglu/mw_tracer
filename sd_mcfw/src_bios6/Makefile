
MODULE_NAME = dvr_rdk_bios6


INCDIR = .  $(jpegenc_PATH)/inc $(jpegdec_PATH)/inc  $(h264enc_PATH)/packages/ti/sdo/codecs/h264enc/ $(h264dec_PATH)/packages/ti/sdo/codecs/h264dec/ $(rpe_INCLUDE) $(rpei_INCLUDE)

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc ipc syslink hdvpss dvr_rdk fc xdais h264enc h264dec mpeg4dec jpegenc jpegdec swosd track stutrack stusidetrack studoubletrack ceiltrack blackboardtrack edma3lld

MODULE_SRC_BASE_PATH := $(dvr_rdk_PATH)/mcfw/src_bios6

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
SRCS_ti816x =
SRCS_ti814x =
CFLAGS_LOCAL_COMMON = $(DVR_RDK_CFLAGS)

ifneq ($(CORE),c6xdsp)

SRCDIR = utils/src links_m3video links_m3vpss
SRCDIR += ../demos/chains/common ../demos/chains/vpss_chains ../demos/chains/vpss_video_chains
include $(MODULE_SRC_BASE_PATH)/links_common/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/links_m3vpss/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/links_m3video/SRC_FILES.MK

else

ifneq ($(PLATFORM), ti810x-evm)
SRCDIR = utils/src links_c6xdsp
SRCDIR += ../demos/chains/common 
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/swosd
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/scd
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/track
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/stutrack
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/stusidetrack
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/studoubletrack
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/ceiltrack
INCDIR  += $(MODULE_SRC_BASE_PATH)/alg/blackboardtrack
include $(MODULE_SRC_BASE_PATH)/links_common/SRC_FILES_DSP.MK
include $(MODULE_SRC_BASE_PATH)/links_c6xdsp/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/swosd/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/track/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/stutrack/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/stusidetrack/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/studoubletrack/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/ceiltrack/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/blackboardtrack/SRC_FILES.MK
endif
endif

include $(MODULE_SRC_BASE_PATH)/utils/SRC_FILES.MK

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
