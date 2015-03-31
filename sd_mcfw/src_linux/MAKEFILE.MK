
dvr_rdk:
	$(MAKE) -fMAKEFILE.MK libs TARGET=depend
	$(MAKE) -fMAKEFILE.MK libs
	$(MAKE) -fMAKEFILE.MK utils TARGET=exe
	$(MAKE) -C$(dvr_rdk_PATH)/mcfw/src_linux/osa_kermod/build install

utils:
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/utils/mem_rdwr/src MODULE=mem_rdwr $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/utils/remote_debug_client/src MODULE=remote_debug_client $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/utils/sys_pri/src MODULE=sys_pri $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/utils/fw_load/src MODULE=fw_load $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/utils/i2c_rdwr/src MODULE=i2c_rdwr $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/utils/vpdma_dump MODULE=vpdma_dump $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/utils/mem_stats MODULE=mem_stats $(TARGET)	

mcfw_api:
	echo "---------------------mcfw_api git----------------------------------"
	@echo "const static char *g_make_build_date = \"`env LANG=en_US.UTF-8 date`\";" >$(dvr_rdk_PATH)/mcfw/src_linux/mcfw_api/build_info.h
	@sed -e "s#<version>#$$(git describe --dirty --always)#g" <$(dvr_rdk_PATH)/mcfw/git_version.h.in> $(dvr_rdk_PATH)/mcfw/git_version.h.tmp
	@if diff -q $(dvr_rdk_PATH)/mcfw/git_version.h.tmp $(dvr_rdk_PATH)/mcfw/git_version.h >/dev/null 2>&1; then \
		rm $(dvr_rdk_PATH)/mcfw/git_version.h.tmp; \
	else \
		echo "git_version.h.in =>$(dvr_rdk_PATH)/mcfw/src_linux/mcfw_api/git_version.h" ; \
	mv $(dvr_rdk_PATH)/mcfw/git_version.h.tmp $(dvr_rdk_PATH)/mcfw/src_linux/mcfw_api/git_version.h; \
	fi	
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/mcfw_api MODULE=dvr_rdk_mcfw_api $(TARGET)

link_api:
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/links/system MODULE=dvr_rdk_link_api $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/links/ipcBitsIn MODULE=dvr_rdk_link_api $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/links/ipcBitsOut MODULE=dvr_rdk_link_api $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/links/ipcFramesIn MODULE=dvr_rdk_link_api $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/links/ipcFramesOut MODULE=dvr_rdk_link_api $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/links/avsync MODULE=dvr_rdk_link_api $(TARGET)
ifeq ($(PLATFORM),ti810x-evm)
else
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/links/audio MODULE=dvr_rdk_link_api $(TARGET)
endif

osa:
	$(MAKE) -C$(dvr_rdk_PATH)/mcfw/src_linux/osa_kermod/build $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/osa/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/tvp5158/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/sii9022a/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/thsfilters/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/adv7441/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/adv7442/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/tvp7002/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/adv7619/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/ad9889/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/gs2971/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/gs2972/src MODULE=dvr_rdk_osa $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(dvr_rdk_PATH)/mcfw/src_linux/devices/ths8200/src MODULE=dvr_rdk_osa $(TARGET)

fbdev:
ifneq ($(OS),Windows_NT)
ifeq ($(PLATFORM),ti816x-evm)
	$(MAKE) -C$(dvr_rdk_PATH)/mcfw/src_linux/kernel_modules/ti816x_fb $(TARGET)
else
	@echo Bypassing ... $@: for 814x 
endif
else
	@echo Bypassing ... $@: Building module for windows
endif

libs: osa fbdev link_api mcfw_api utils

clean:
	$(MAKE) -fMAKEFILE.MK libs TARGET=clean

all: clean dvr_rdk

.PHONY: utils fbdev mcfw_api link_api osa fbdev libs clean all dvr_rdk
