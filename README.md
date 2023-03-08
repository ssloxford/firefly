# Firefly: Spoofing Earth Observation Satellite Data through Radio Overshadowing

This repository contains the code for the paper "Firefly: Spoofing Earth Observation Satellite Data through Radio Overshadowing".
The specifics of experimental design, methodology and evaluation can be found in the paper.
This work has been a collaboration between Edd Salkield, Joshua Smailes, Sebastian Köhler, Simon Birnbach, Richard Baker, and Ivan Martinovic from the Systems Security Lab at the University of Oxford, and Martin Strohmeier from armasuisse Science and Technology.

The repository contains the scripts used to create the figures for the paper, specifically to mask and insert fires into the raw packet sequence data of the Terra and Aqua satellites.
The code depends upon several libraries and tools to encode/decode the raw packet sequences, specifically [libgiis](https://github.com/ssloxford/libgiis), [libspp](https://github.com/ssloxford/libspp), and [libcadu](https://github.com/ssloxford/libcadu).

## Citation

If you use this repository please cite the paper as follows:

```
@article{salkield2023firefly,
  author = {Salkield, Edd and Smailes, Joshua and K\"ohler, Sebastian and Birnbach, Simon and Baker, Richard and Strohmeier, Martin and Martinovic, Ivan},
  title = {{Firefly: Spoofing Earth Observation Satellite Data through Radio Overshadowing}},
  year = {2023},
  journal = {{NDSS Workshop on the Security of Space and Satellite Systems (SpaceSec)}},
  doi = {10.14722/spacesec.2023.231879},
}
```

## Acknowledgements

The authors would like to thank Jonathan Tanner for his assistance in writing the packet encoding software.
