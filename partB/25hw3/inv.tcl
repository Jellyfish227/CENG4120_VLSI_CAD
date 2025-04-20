# inv.tcl 

setMultiCpuUsage -localCpu 8
source cla4.globals
init_design
floorPlan -r 1.0 0.98 4.0 4.0 4.0 4.0
globalNetConnect vdd -type pgpin -pin vdd -inst *
globalNetConnect gnd -type pgpin -pin gnd -inst *
sroute -nets {vdd gnd}
addRing -center 1 -spacing 0.5 -width 0.5 -layer {top 3 bottom 3 left 4 right 4} -nets {gnd vdd}
addStripe -nets {vdd gnd} -layer 4 -direction vertical -width 0.4 -spacing 0.5 -set_to_set_distance 5 -start 0.5
addStripe -nets {vdd gnd} -layer 3 -direction horizontal -width 0.4 -spacing 0.5 -set_to_set_distance 5 -start 0.5
place_design
addFiller -cell FILL -prefix FILL -fillBoundary
assignIoPins
routeDesign
verify_drc
puts "The die area is [get_db designs .area] square microns. "
