Standardized Encryption of Modelica Libraries and Artifacts (name is tentative)

In case you like to contribute, please, consult https://github.com/modelica/ModelicaAssociationCLA for the terms.

The working draft specification for the framework lives in a pull request to the Modelica Specification, see: https://github.com/modelica/ModelicaSpecification/pull/2931 and also the original draft specification for the framework in: [SEMLA specification](doc/SEMLA.md)


See build instruction in: [README](src/README)

Current implementation supports following protocol commands:
  * VERSION
  * LIB
  * FEATURE 
  * RETURNFEATURE 
  * FILE
  * FILECONTENT

Following commands are not supported:
  * TOOLS
  * LICENSEINFO
