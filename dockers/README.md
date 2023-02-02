### Docker files
- build -- implements a docker file that describes the image for the build process. The changes could be done manually in case of the will to change the build target.
- native_run -- docker file for the test run of the PHSM
- tests -- validation docker file. As soon as the tests are done the docker exists. The printed status will be displayed in the command line.