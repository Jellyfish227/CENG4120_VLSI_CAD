if {![namespace exists ::IMEX]} { namespace eval ::IMEX {} }
set ::IMEX::dataVar [file dirname [file normalize [info script]]]
set ::IMEX::libVar ${::IMEX::dataVar}/libs

create_library_set -name libs_typical\
   -timing\
    [list ${::IMEX::libVar}/lib/typ/gscl45nm.lib]
create_rc_corner -name default_rc_corner\
   -preRoute_res 1\
   -postRoute_res 1\
   -preRoute_cap 1\
   -postRoute_cap 1\
   -postRoute_xcap 1\
   -preRoute_clkres 0\
   -preRoute_clkcap 0
create_delay_corner -name delay_default\
   -library_set libs_typical
create_constraint_mode -name constraints_default\
   -sdc_files\
    [list /dev/null]
create_analysis_view -name analysis_default -constraint_mode constraints_default -delay_corner delay_default
set_analysis_view -setup [list analysis_default] -hold [list analysis_default]
