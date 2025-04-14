###################################################################

# Created by write_sdc on Fri Apr 14 23:04:21 2023

###################################################################
set sdc_version 2.1

set_units -time ns -resistance kOhm -capacitance pF -voltage V -current uA
create_clock [get_ports clk]  -name clock1  -period 0.25  -waveform {0 0.125}
