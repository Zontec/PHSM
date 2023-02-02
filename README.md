# Portable Hardware Security Module

### Documentation
To make documentation one should invoke ***make docs*** command via the command file. The generated documentation can be found by path: *./docs/doxy*. One could choose to build to pdf or open *index.html*

Please, first generate the documentation as it will contain more information about files.

### Project structure
- [client](./client/README.md) -- python client for communication to PHSM
- [core](./core/README.md) -- PHSM core logic
- [cryptolib](./cryptolib/README.md) -- crypto library sources
- [dockers](./dockers/README.md) -- docker files used for docker-compose
- [docs](./docs/README.md) -- doxygen config file and target directory for documentation
- [hw](./hw/README.md) -- hardware-specific tools and source code
- [validation_tests](./validation_tests/README.md) -- validation engine source

More information about each component can be found in the component README.md

### How to run

A simple native build could be involved by  ***make*** command. 
By combining different arguments one could manage the building process. As a more advanced example the extended version of the native build: *BUILD_TYPE=test WARNINGS=y ARCH=NATIVE COMPILE_MODE=DEBUG make* 
- BUILD_TYPE -- final build type.
- WARNINGS -- can be 'n'-no or 'y'-yes
- ARCH -- aimed platform
- COMPILE_MODE -- DEBUG or RELEASE

### How to docker
For the building process, one could run "docker-compose -f docker-build.yaml up". The artifacts will be located in the ***build*** folder.
For the testing and validation process, one could run "docker-compose -f docker-tests.yaml up". 
For the emulation process of the PHSM, one could run "docker-compose -f docker-compose.yaml up". With the use of a python client, one could connect to the PHSM with *localhost:8888*

### Files
The detailed information about the files could be found in the "Files" menu from the generated documentation.
