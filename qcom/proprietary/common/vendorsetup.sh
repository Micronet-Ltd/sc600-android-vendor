# *********************************************************************
# Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
# *********************************************************************

# QC specific definitions & variables

export QCPATH="vendor/qcom/proprietary"
export QCPATH_COMMONSYS="${QCPATH}/commonsys"
export SDCLANG_AE_CONFIG="${QCPATH}/common-noship/etc/sdclang.json"
export SDCLANG_CONFIG="${QCPATH}/common/config/sdclang.json"
if [ -f "$(pwd)/${QCPATH}/qrsp/mpgen/mpgen.py" ]; then
export RTIC_MPGEN="python $(pwd)/${QCPATH}/qrsp/mpgen/mpgen.py"
fi
