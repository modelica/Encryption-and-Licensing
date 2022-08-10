# Standardized Encryption of Modelica Libraries and Artifacts, SEMLA

SEMLA is an open-source project originally developed by Modelon AB and now continued and hosted by the Modelica Association for formal standardization. The original draft specification and referene implementation is by Modelon AB, and has since 2015 been used in Modelon's Optimica Compiler Toolkit, in [Modelon Impact](https://modelon.com/modelon-impact/) and later also been integrated in [OpenModelica](https://openmodelica.org). It provides means for tool-independent licensing and encryption of Modelica Libraries. 

In case you like to contribute to the code, please, consult https://github.com/modelica/ModelicaAssociationCLA for the terms.

The working draft specification for the framework lives in a pull request to the Modelica Specification, see: https://github.com/modelica/ModelicaSpecification/pull/2931 and also the original draft specification for the framework in: [SEMLA specification](doc/SEMLA.md). If you are interested to contribute to the specification, you have to become a member of the Modelica Association. Please contact backoffice@modelica.org ofr more information. 


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
