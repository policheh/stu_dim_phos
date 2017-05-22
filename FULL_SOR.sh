#!/bin/bash

# This script is executed by DCS SOR
# Yuri Kharlov. 21 April 2015
# Updated: 12 October 2016

cd ~/PHOSControl_5
# Clear FEE busy in 14 SRU in parallel
./ClearBusyAll.sh

# Synchronize ALTRO clock in 14 SRU in parallel
./SyncALTRO.sh all

# TRU SOR reset
./TRUsorReset.sh all

# STU configuration and SOR sync
cd ~/PHOS_STU_DIM
./dim_stu_command /STU_PHOS/Configure
sleep 1

./dim_stu_command /STU_PHOS/SOR_sync
#sleep 20
sleep 12; # Daiki changed sleep time because of update of STU SIM server on 08.March.2016

