#!/bin/bash

##################################################### SYNC DATA ########################################
rsync -arv /data/DDBASE/DATA/CASSINI/MAG/1MIN/KRTP/ amda@cdpp1.cesr.fr:/data1/DDBASE/DATA/CASSINI/MAG/1MIN/KRTP/
rsync -arv /data/DDBASE/DATA/CASSINI/MAG/1MIN/KSM/ amda@cdpp1.cesr.fr:/data1/DDBASE/DATA/CASSINI/MAG/1MIN/KSM/
rsync -arv /data/DDBASE/DATA/CASSINI/MAG/1MIN/KSO/ amda@cdpp1.cesr.fr:/data1/DDBASE/DATA/CASSINI/MAG/1MIN/KSO/
rsync -arv /data/DDBASE/DATA/CASSINI/MAG/1MIN/RTN/ amda@cdpp1.cesr.fr:/data1/DDBASE/DATA/CASSINI/MAG/1MIN/RTN/
################################################ END OF SYNC DATA ########################################
