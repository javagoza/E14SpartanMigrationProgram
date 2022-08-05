# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: C:\Xilinx\work\artys7\arty_thermal_camera_5\sdk\arty_thermal_camera_system\_ide\scripts\debugger_arty_thermal_camera-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source C:\Xilinx\work\artys7\arty_thermal_camera_5\sdk\arty_thermal_camera_system\_ide\scripts\debugger_arty_thermal_camera-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent Arty S7 - 50 210352A6C27EA" && level==0 && jtag_device_ctx=="jsn-Arty S7 - 50-210352A6C27EA-0362f093-0"}
fpga -file C:/Xilinx/work/artys7/arty_thermal_camera_5/sdk/arty_thermal_camera/_ide/bitstream/thermal_camera_bd_wrapper_v2.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw C:/Xilinx/work/artys7/arty_thermal_camera_5/sdk/thermal_camera_bd_wrapper_v2/export/thermal_camera_bd_wrapper_v2/hw/thermal_camera_bd_wrapper_v2.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow C:/Xilinx/work/artys7/arty_thermal_camera_5/sdk/arty_thermal_camera/Debug/arty_thermal_camera.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
