# libiov roadmap

### What we have:
- infrastructure

### What we need:
- an API definition
- a skeleton code implementation

### Steps to achieve it:

1. A document in the libiov repo on Github with the roadmap (this file)
1. Have documentation files for APIs
    * documents to have:
        * a text file with module-level APIs (functions, parameters, semantics)
        * a picture with interactions across modules
        * a text file with implementation details, toolchain details
        * example document with pseudocode
	* should be detailed enough to clear confusions, but not more
    * should solve all use cases:
        * iomodule writer
        * iomodule deployment administrator
1. Define libiov MVP
	* maybe bridge + arp + ping + broadcast + restartable, but no external configuration, no visibility on DP tables
1. Separate activities across team members
1. Prepare a code skeleton
1. Iterate through APIs
