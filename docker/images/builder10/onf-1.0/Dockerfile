FROM debian:10

# Dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        apt-file apt-utils autoconf automake autotools-dev bash-completion bc \
        bind9-host binfmt-support bison bsdmainutils build-essential ccache \
        cdbs cpio cryptsetup-bin debhelper device-tree-compiler devscripts \
        devscripts dialog dosfstools doxygen dpkg-cross dpkg-sig emacs file \
        flex gawk gcc gdb genisoimage git gperf ifupdown iproute2 iputils-ping \
        isolinux kmod less libc6-dev libcurl4-nss-dev libdouble-conversion-dev \
        libedit-dev libevent-dev libexpat1-dev libgoogle-glog-dev libi2c-dev \
        libkrb5-dev libnuma-dev libsasl2-dev libsnappy-dev libpam-dev libpcap-dev \
        libsnmp-dev libssl-dev libtool libtool-bin locales lsof make mingetty \
        mtd-utils mtools multistrap nano ncurses-dev netbase net-tools nfs-common \
        openssh-server pkg-config pkg-config procps psmisc qemu qemu-user-static \
        rsyslog rsync squashfs-tools sshpass stgit sudo syslinux-utils telnet \
        texinfo traceroute u-boot-tools vim wget xorriso zile zip ruby ruby-dev \
        rubygems chrpath devscripts dh-autoreconf dh-systemd flex libcap-dev \
        libc-ares-dev libjson-c-dev libpam0g-dev libpcre3-dev libreadline-dev \
        libsystemd-dev pkg-config texlive-generic-recommended texinfo \
        apt-transport-https texlive-latex-base apt-cacher-ng libelf-dev python3 \
        python3-pip lsb-release && \
        gem install rake && \
        gem install --version 1.3.3 fpm

# Place files
ADD requirements.txt /tmp
ADD multistrap-insecure-fix.patch /tmp
COPY docker_shell /bin/docker_shell
COPY container-id /bin/container-id

RUN pip3 install -r /tmp/requirements.txt

# Patching the builder
RUN cp /usr/include/linux/i2c-dev.h /usr/include/linux/i2c-devices.h
RUN patch -p1 < /tmp/multistrap-insecure-fix.patch /usr/sbin/multistrap
RUN echo "en_US ISO-8859-1" >> /etc/locale.gen && \
    echo "en_US.ISO-8859-15 ISO-8859-15" >> /etc/locale.gen && \
    echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen && \
    locale-gen
ENV LC_ALL=en_US.UTF-8



LABEL MAINTAINER="Yi Tseng<yi@opennetworking.org>"

