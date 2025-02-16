set target_library ../FreePDK45/gscl45nm.db
set link_library ../FreePDK45/gscl45nm.db
analyze -f verilog cla4.v
elaborate cla4
create_clock clk -name clock1 -period 0.21
set_max_area 300
compile -area_effort high
write -format verilog -hierarchy -output cla4_synth.v
write_sdc -nosplit cla4_synth.sdc
report_timing
report_area
exit
