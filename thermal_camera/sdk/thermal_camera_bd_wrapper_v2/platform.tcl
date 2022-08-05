# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Xilinx\work\artys7\arty_thermal_camera_5\sdk\thermal_camera_bd_wrapper_v2\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Xilinx\work\artys7\arty_thermal_camera_5\sdk\thermal_camera_bd_wrapper_v2\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {thermal_camera_bd_wrapper_v2}\
-hw {C:\Xilinx\work\artys7\arty_thermal_camera_5\thermal_camera_bd_wrapper_v2.xsa}\
-out {C:/Xilinx/work/artys7/arty_thermal_camera_5/sdk}

platform write
domain create -name {standalone_microblaze_0} -display-name {standalone_microblaze_0} -os {standalone} -proc {microblaze_0} -runtime {cpp} -arch {32-bit} -support-app {memory_tests}
platform generate -domains 
platform active {thermal_camera_bd_wrapper_v2}
platform generate -quick
platform generate
catch {platform remove thermal_camera_bd_wrapper}
bsp reload
platform generate -domains 
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform active {thermal_camera_bd_wrapper_v2}
platform config -updatehw {C:/Xilinx/work/artys7/arty_thermal_camera_5/thermal_camera_bd_wrapper_v2.xsa}
platform clean
platform generate
platform clean
platform generate
bsp reload
platform config -updatehw {C:/Xilinx/work/artys7/arty_thermal_camera_5/thermal_camera_bd_wrapper_v2.xsa}
platform clean
platform generate
bsp reload
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform active {thermal_camera_bd_wrapper_v2}
platform config -updatehw {C:/Xilinx/work/artys7/arty_thermal_camera_5/thermal_camera_bd_wrapper_v2.xsa}
platform clean
platform generate
bsp reload
platform generate -domains 
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
