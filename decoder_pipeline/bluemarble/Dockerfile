FROM centos
WORKDIR /root
COPY ./src/BLUEMARBLE_2.2_SPA_2.2.tar.gz /root
# COPY ./src/BLUEMARBLE_2.2_SPA_2.2_testdata.tar.gz /root

RUN tar -xzvf BLUEMARBLE_2.2_SPA_2.2.tar.gz
# RUN tar -xzvf BLUEMARBLE_2.2_SPA_2.2_testdata.tar.gz
RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-Linux-* &&\
    sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-Linux-*
RUN rpm --import https://fedora.mirrorservice.org/epel/RPM-GPG-KEY-EPEL-8
RUN dnf -y install epel-release javapackages-tools java-1.8.0-openjdk python3 python3-requests file 
# TODO: load dnf key for epel into the right place
RUN dnf -y install ImageMagick
WORKDIR data/tmp
