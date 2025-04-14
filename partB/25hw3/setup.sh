#!/bin/bash

# Synopsys Design Compiler
export SYNOPSYS=/opt/synopsys
export SNPSLMD_LICENSE_FILE=$SYNOPSYS/key
export LM_LICENSE_FILE=$SYNOPSYS/key

export VCS_HOME=$SYNOPSYS/vcs
export VERA_HOME=$SYNOPSYS/vera/linux
export LMC_HOME=$SYNOPSYS/lmc

export PATH=$PATH:$SYNOPSYS/pts/bin
export PATH=$PATH:$SYNOPSYS/syn/bin
export PATH=$PATH:$SYNOPSYS/vcs/bin
export PATH=$PATH:$SYNOPSYS/vera/linux/bin
export PATH=$PATH:$SYNOPSYS/lmc/bin
export PATH=$PATH:$SYNOPSYS/fm64/bin
export PATH=$PATH:$SYNOPSYS/icc/N-2017.09/bin
export PATH=$PATH:$SYNOPSYS/verdi/Q-2020.03-SP2-1/bin

# Cadence Innovus
export CDS_ROOT=/opt2/cadence
export CDS_LIC_FILE=/opt2/cadence/cds.lic
export LM_LICENSE_FILE=/opt2/cadence/cds.lic
export PATH=/opt2/cadence/INNOVUS201/bin:/opt2/cadence/GENUS1813/bin:/opt2/cadence/EDI142/bin:$PATH

