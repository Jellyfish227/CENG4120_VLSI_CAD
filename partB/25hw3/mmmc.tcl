# mmmc.tcl

create_library_set -name libs_typical -timing FreePDK45/gscl45nm.lib

create_constraint_mode -name constraints_default -sdc_files cla4_synth.sdc
 
create_delay_corner -name delay_default -library_set libs_typical

create_analysis_view -name analysis_default -constraint_mode constraints_default \
	-delay_corner delay_default

set_analysis_view -setup analysis_default -hold analysis_default
