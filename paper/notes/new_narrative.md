# Paper structure
"understanding the barriers to entry for signal injection attacks against the downlink"

open questions:
* can attackers get their signal into the dish?
* how can an attacker know that their signal will cause harm?
* what downstream effects could they seek to have, therefore what cost/benefit ratio?

## background

Legacy systems exist and are used

This causes downstream issues

Argue for the equivalence of radio bands, and how they'll come down in price over time

## related work

Comparative analysis to GPS spoofing

Point to existing satellite downstream work

If poss, cite Johannes' modem work

Non-satellite related work: WiFi, wireless sensor spoofing, etc.

## threat model

Signal injection is cheaper than we think, across various antenna systems

Argue that the equipment cost aligns with the attacker's goal

## section 1: physical layer

### Experiment 1: Out-of-band angular emission

Outcome: Validate antenna attenuation model and understand the key factors affecting injection capability

Summary: Emit out-of-band signals at various dishes and measure the gain

Factors to test:
* type of antenna
    - omnidirectional, as used in Iridium, GPS. Use Josh's one
    - small dish, as used in Starlink. Use the AOPP dish offered to us
    - large dish, as used in gov. satellites. Use the big AOPP dish
* distance to dish
* angle of injection
* physical obstacles

Outputs:
* a number of meaurements of gain/SNR in multiple injection settings
* compare against the polar plots we get from simulation
* understand how accurately the model lets an attacker estimate equipment needed beforehand

Method:
* Set up several antenna types in a real world setting
* Emit repeated, distinct legal out-of-band signals at the dish in amateur frequency bands
* Filter down to just the band that we emit, and measure signal gain before/after emission

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

Advantage over for-real: it's not illegal and allows us to control for physical-layer factors

Factors to test:
* Data integrity checks e.g. checksums
* Cryptographic integrity checks e.g. crypto, what's breakable, etc.
* Causing pipeline stages to crash

### Experiment 2: end-to-end for real on QO-100

Outcome: End-to-end attack to understand the key protocol factors affecting injection capability

Summary: Overshadow reflected bent-pipe signal from QO-100 or equivalent at antenna gains calibrated to mirror the real-world setup, causing erroneous bytes to be decoded

Possible satellites:
https://www.amsat.org/two-way-satellites/

FM: https://www.amsat.org/fm-satellite-frequency-summary/

so-50: 145.850 MHz up, 436.795 MHz down
https://www.amsat.org/two-way-satellites/so-50-satellite-information/

Allowed frequencies to transmit:

Factors to test:
* overshadowing
    - for each encoding type (BPSK, QPSK, etc.), how much louder does it need to be?
    - resetting protocol sync headers
    - attacker's silence being filled in with legitimate data
    - not necessary in all cases, e.g. if the groundstation is on when the satellite is not overhead e.g. NASA's software in the White Sands complex


## section 3: downstream consequences

We map out which systems may be similarly vulnerable, pointing to future research

We classify which systems use what antenna types, and so forth

Follow-up papers:
* In real time - perhaps this is a follow-up paper?
    - Just flipping bits?
    - Performing in real time modification pipelines
    - Real-time monitoring and jamming?

