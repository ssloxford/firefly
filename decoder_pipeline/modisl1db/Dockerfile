FROM centos
WORKDIR /root
COPY ./src/MODISL1DB_1.9_SPA_1.2.tar.gz /root
RUN tar -xzvf MODISL1DB_1.9_SPA_1.2.tar.gz
RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-Linux-* &&\
    sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-Linux-*
RUN dnf -y install java-1.8.0-openjdk python3 python3-requests python2 file
RUN ln -s /usr/bin/python2 /usr/bin/python

# Add a whole load of hacks to get l1atob running
RUN sed -ie "s/6\.1\.15b/6\.2\.1_obpg/" /root/SPA/modisl1db/algorithm/run/scripts/modules/modis_utils.py 
RUN sed -ie 's/<env name="OCSSW_BIN" value="{modis_L1_home}{\/}run{\/}bin"\/>/<env name="OCSSW_BIN" value="{modis_L1_home}{\/}modisl1db{\/}bin"\/>/' /root/SPA/modisl1db/wrapper/l1atob/generic.xml

COPY netrc /root/.netrc
