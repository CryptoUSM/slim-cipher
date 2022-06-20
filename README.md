# SLIM
SLIM is a ultra lightweight block cipher using 32-bit blocks and 80 bits master key [[1]](#1).

## References
<a id="1">[1]</a> 
Aboushosha, B., Ramadan, R. A., Dwivedi, A. D., El-Sayed, A., & Dessouky, M. M. (2020). 
SLIM: A lightweight block cipher for internet of health things. 
IEEE Access, 8, 203747-203757.


## Summary
- ./cryptosmt-slim: folder contains all scripts used in cryptoSMT tool for differential attack
  - cryptosmt-boomerang-slim.py: main script to execute slim.py
  - slim.py: SLIM structure modelling in SMT model
  - slim32.yaml: input file for running differential clustering (mode 4)

- slim.cpp: SLIM encryption and decryption 
- slim-key.cpp: SLIM key scheduling algorithm
- slim-ddt.cpp: generate SLIM Differential Distribution Table 
- slim-attack.cpp: SLIM key recovery attack

