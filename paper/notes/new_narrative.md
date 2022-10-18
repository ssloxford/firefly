# Paper structure
"understanding the barriers to entry for signal injection attacks against the downlink"

open questions:
* can attackers get their signal into the dish?
* how can an attacker know that their signal will cause harm?
* what downstream effects could they seek to have, therefore what cost/benefit ratio?

## background
Legacy systems exist and are used
This causes downstream issues

## related work
Point to existing satellite downstream work
If poss, cite Johannes' modem work

## threat model
Signal injection is cheaper than we think, across various antenna systems

## section 1: physical layer

### Experiment 1: Out-of-band angular emission

Outcome: Understand the key factors in antenna attenuation affecting injection capability
Summary: Emit out-of-band signals at various dishes and measure the gain

Factors to test:
* type of antenna
    - omnidirectional, as used in Iridium, GPS
    - small dish, as used in Starlink
    - large dish, as used in gov. satellites
* distance to dish
* angle of injection
* physical obstacles

Outputs:
* polar plot of gain against angle and distance in "clean room" setting
* measurements in the real works, with obstacles, multipath, etc.
* development of a model to understand how/whether we can estimate the equipment needed beforehand

Method 1:
* Set up several antenna types in a clean room/real world setting
* Emit legal out-of-band signals at the dish
* Filter down to just the band that we emit, and measure signal gain before/after emission
* Make a nice polar plot

### related work
Measuring dish attenuation on wet dishes
https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6916611
Experiment was done in Malaysia
> The distance (R) between the transmitter and receiver was 17m and 14.25m
> The experiment was located in USM engineering campus.

## Section 2: protocol

### Protocol and decoder analysis

Outcome: Understand how common protocols/decoders are weak to injection on the downlink
Summary: Reverse several protocols/decoders to determine packets that break things if injected

Factors to test:
* Data integrity checks e.g. checksums
* Cryptographic integrity checks e.g. crypto, what's breakable, etc.
* Causing pipeline stages to crash

### Experiment 2: end-to-end "for real" in a shielded box

Outcome: Understand the key protocol factors affecting injection capability
Summary: Emit in-band signals in a shielded box, calibrated to mirror the real-world setup gain, that cause erroneous bytes to be decoded

Factors to test:
* overshadowing
    - for each encoding type (BPSK, QPSK, etc.), how much louder does it need to be?
    - resetting protocol sync headers
    - attacker's silence being filled in with legitimate data
    - not necessary in all cases, e.g. if the groundstation is on when the satellite is not overhead e.g. NASA's software in the White Sands complex

* In real time - perhaps this is a follow-up paper?
    - Just flipping bits?
    - Performing in real time modification pipelines
    - Real-time monitoring and jamming?

### Related work

## section 3: downstream consequences

We map out which systems may be similarly vulnerable, pointing to future research
We classify which systems use what antenna types, and so forth
