Open Network Linux (with ONLPv2 and Debian 10)
==================

Check out the main/offical website at http://opennetlinux.org .

Open Network Linux (ONL) is a Linux distribution for bare metal switches.  ONL
builds an ONIE-compatible installer and a switch image which contains a complete
Debian distribution with added drivers and configuration for running on bare metal
switches.

Software License
-----------------

Licenses for the software are described under the [LICENSE](LICENSE) file.  Download or use of the software implies consent.

Supported platforms
-------------------

 - x86-64-accton-as7712-32x-r0
 - x86-64-accton-as5812-54x-r0
 - x86-64-inventec-d5254-r0
 - x86-64-inventec-d10064-r0
 - x86-64-inventec-d10056-r0
 - x86-64-accton-wedge100bf-32x-r0
 - x86-64-accton-wedge100bf-32qs-r0
 - x86-64-accton-wedge100bf-65x-r0
 - x86-64-inventec-d7032q28b-r0
 - x86-64-delta-ag9032v1-r0
 - x86-64-accton-as6712-32x-r0
 - x86-64-accton-as7716-24sc-r0
 - x86-64-accton-as7716-24xc-r0
 - x86-64-kvm-x86-64-r0

Build the ONL installer
------------------------

We use Docker container to build the ONIE installer

The first step is to build the ONL builder image:

```bash
make -C docker/images/builder10/onf-1.0 build
```

After the builder image built, use the follwing command to build the ONL image:

```bash
make docker
```

Documentation
-------------

Please see the docs directory for additional information.

![alt text] (https://scan.coverity.com/projects/8655/badge.svg "Coverity Scan Build Status")
