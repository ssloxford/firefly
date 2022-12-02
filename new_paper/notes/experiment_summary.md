Attacker needs to calculate the required transmit power given the setup

Physical layer factors

# Attacker Tx Power (W)

The calculated power the attacker needs to transmit from their hardware

Derived from:
- Satellite Rx power - derived from physical layer public information
- Overshadow factor - derived from the protocol layer information
- Injection loss (from graph) - derived from the physical layer
    - attacker direction
    - attacker distance

Depends on:
- Amplifier power
- IF power
- Antenna type


# Legitimate Rx power (W)

The power of the legitimate signal as received by the victim dish

Might be known by the attacker already as they could set up their own dish.
Otherwise, calculated from:

- Satellite Tx power (W)
- Satellite transfer losses (dB) - based on distance

We assume:
- the dish is pointed directly at the satellite, so max gain (if it's not, the attack is easier)

Could theoretically be measured

## Evaluation
We perform a calculation of how this can be determined either by experiment or by calculation


# Overshadow factor (dB)

The ratio of Attacker Rx power to Legitimate Rx power required to decode the attacker's signal.
The attacker must achieve this to successfully attack.

Depends upon the protocol, including:
- Modulation scheme
- Checksum-correctable bits

Should also consider if the attacker wants to ensure that the checksum error rate doesn't change
Then they can't rely upon checksum correction

We measure for a number of different protocols:

* NASA's CADU
* DVB-S2

Protocol layer

## Evaluation

Simulation with GNURadio


# Injection gain (dB)

Depends upon:

- Injection angle
- Dish type
