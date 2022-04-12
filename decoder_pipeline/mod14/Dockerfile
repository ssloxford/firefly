FROM centos
WORKDIR /root
COPY ./MOD14_6.2.1_SPA_1.1.tar.gz /root
# COPY ./MOD14_6.2.1_SPA_1.1_testdata.tar.gz /root
RUN tar -xzvf MOD14_6.2.1_SPA_1.1.tar.gz
# RUN tar -xzvf MOD14_6.2.1_SPA_1.1_testdata.tar.gz

RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-Linux-* &&\
    sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-Linux-*
# TODO: some of these dependencies may be unnecessary
RUN dnf -y install java-1.8.0-openjdk python3 python3-requests python2 file
