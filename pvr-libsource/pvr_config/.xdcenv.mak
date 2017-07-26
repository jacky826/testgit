#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/jason/dm365/work-dir/dvssdk402/dvsdk-demos_4_02_00_01/packages;/home/jason/dm365/work-dir/dvssdk402/dmai_2_20_00_15/packages;/home/jason/dm365/work-dir/dvssdk402/codec-engine_2_26_02_11/packages;/home/jason/dm365/work-dir/dvssdk402/framework-components_2_26_00_01/packages;/packages;/home/jason/dm365/work-dir/dvssdk402/xdais_6_26_01_03/packages;/home/jason/dm365/work-dir/dvssdk402/linuxutils_2_26_01_02/packages;/home/jason/dm365/work-dir/dvssdk402/codecs-dm365_4_02_00_00/packages;/home/jason/dm365/work-dir/dvssdk402/codec-engine_2_26_02_11/examples
override XDCROOT = /home/jason/dm365/work-dir/dvssdk402/xdctools_3_16_03_36
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/jason/dm365/work-dir/dvssdk402/dvsdk-demos_4_02_00_01/packages;/home/jason/dm365/work-dir/dvssdk402/dmai_2_20_00_15/packages;/home/jason/dm365/work-dir/dvssdk402/codec-engine_2_26_02_11/packages;/home/jason/dm365/work-dir/dvssdk402/framework-components_2_26_00_01/packages;/packages;/home/jason/dm365/work-dir/dvssdk402/xdais_6_26_01_03/packages;/home/jason/dm365/work-dir/dvssdk402/linuxutils_2_26_01_02/packages;/home/jason/dm365/work-dir/dvssdk402/codecs-dm365_4_02_00_00/packages;/home/jason/dm365/work-dir/dvssdk402/codec-engine_2_26_02_11/examples;/home/jason/dm365/work-dir/dvssdk402/xdctools_3_16_03_36/packages;..
HOSTOS = Linux
endif
