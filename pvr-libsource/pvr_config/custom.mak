## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: linker.cmd

linker.cmd: \
  package/cfg/pvr_xv5T.ov5T \
  package/cfg/pvr_xv5T.xdl
	$(SED) 's"^\"\(package/cfg/pvr_xv5Tcfg.cmd\)\"$""\"/home/jason/dm365/work-dir/dvssdk402/dvsdk-demos_4_02_00_01/dm365/pvr/pvr_config/\1\""' package/cfg/pvr_xv5T.xdl > $@
