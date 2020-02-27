### Created by Dr. Benjamin Richards (b.richards@qmul.ac.uk)

### Download base image from repo
FROM hkdaq/triggerapplication:base

### Run the following commands as super user (root):
USER root

#Setup HK prerequisites & get latest version of WCSim
WORKDIR /root/HyperK/WCSim
RUN source /root/HyperK/env-WCSim.sh; git pull; make clean; make rootcint; make;

#Setup TriggerApplication
WORKDIR /TriggerApplication/
RUN source ./Setup.sh; make update; make;

### Open terminal
ENTRYPOINT source /TriggerApplication/Setup.sh && /bin/bash
