Tool-independent Licensing and Encryption of Modelica Libraries
====

This specification describes a system for distributing Modelica libraries which support:
	- encryption
	- licensing
	- secure communication between a Modelica tool and encrypted/licensed Modelica libraries
	- platform independence
	
It is achieved by: 
	- a "Modelica Library Containers" format (MLC) and an associated _Manifest.xml_ meta-data file
	- an interface to the MLCs via a "Library Vendor Executable" (LVE)
		- using the "Standardized Encryption of Modelica Libraries and Artifacts" (SEMLA) protocol

The availability of an open source project containing:
	- this documentation
	- source code of libraries implementing SEMLA and example LVEs
	- source code for example encryption and licensing of Libraries
	- **packagetool**, a utility you can use to create MLCs.
	
See **Requirements** in the Appendix for background information.

## LICENSING AND ENCRYPTION

Licensing and encryption are controlled by the library vendor.

The library vendor chooses if, and how, the library is 
	- encrypted/decrypted
	- licensed (through a mechanism chosen by the library vendor or through an existing tool’s licensing mechanism)
	- which tool can access the library
	- what features are made available to the tool
	
Implementation of the above is **compiled** in a "Library Vendor Executable" (LVE).

## LIBRARY VENDOR EXECUTABLE (LVE)

An LVE is a platform specific binary executable which:
	- is created by the library vendor
	- handles decryption of libraries (packaged in an MLC)
	- handles licensing of libraries (packaged in an MLC)
	- is packaged within a MLC
	- communicates, through the secured SEMLA protocol, with a Modelica tool 
 
### Platform Specific LVE
 
Four platforms are supported, for each platform a LVE must be compiled. Platform and LVE name :
	- windows 32 bits: lve_win32.exe
	- windows 64 bits: lve_win64.exe
	- linux 32 bits: lve_linux32
	- linux 64 bits: lve_linux64

And MLC, containing encrypted/licensed libraries, will also contain 1 to 4 LVEs depending on which platforms need to be supported.

#### Packagetool

If encryption is enabled and an LVE directory exists under the directory where **packagetool** is located:
	- LVEs from that directory are copied into the MLC in the ".library" directory
	- the appropriate fields in the _Manifest.xml_ File are filled

#### SLL and RANDOMIZER KEY

The encryption in the default SEMLA build uses OpenSSL to encrypt libraries. 

The SSL keys are embedded in **packagetool** and **LVE** after obfuscation with a Randomizer key.

As LVEs are created different builds (one or each platform), you need to share the **same** SSL keys and Randomizer Key in **every** builds to create a MLC that is platform independent.

## MODELICA LIBRARY CONTAINER

The container is a _zip_ file, with a _.mol_ file extension.

The container has one top-level directory for each contained top-level package (library)
	- named and structured according to section 13.2.2 of the Modelica Language Specification version 3.2r2.
	- Each top-level directory contains a directory “.library”.

Each “.library” directory contains:
	- A _manifest.xml_ file, containing meta-data about the library. 
	- If the library is encrypted, one or more LVEs, build with the same SSL and Randomizer keys. 
	- Additional directories containing any extra files needed by the LVEs or a Modelica tool. The names of each such directory should be the name of the vendor that introduces it.

Open-source libraries, neither encrypted nor licensed, can be also be distributed in the MLC format.

### Packagetool

The SEMLA distribution contains source code for a, platform independent, utility which can be used to create MLCs. It must be build with the same SSL and Randomizer keys as the LVEs. 
	
If encryption of the library is desired, **packagetool** will also encrypted the .mo-files (to .moc) before adding them to the MLC.

### Manifest File

An example for the manifest file _manifest.xml_ can be found at the end of this document. A DTD (Document Type Definition) or an XML schema will be specified. Here is an overview of its structure, mandatory fields are highlighted:

- archive
	- manifest
		- version – version of the manifest format (this is version 1.0)
	- library
		- id – (**mandatory**)name of the top-level package
		- enabled – if the library should be loaded by default in the tool
		- title – official title of the library
		- description – description of the library
		- version
			- number – (**mandatory**) the version number
			- build – the build number
			- date – the release date
		- language
			- version – (**mandatory**) the version of the Modelica language that the library uses
		- copyright – textual copyright information
		- license – textual license information
		- encryption – only for encrypted libraries
			- executable – one for each LVE
				- path – relative path from the top-level directory of this library to the executable
				- platform – the platform that this executable should be used for, the values are the same as in the Modelica Language Specification, currently “win32”, “win64”, “linux32”, “linux64”
				- licensing – (default is true) true if this executable can handle licensing of the library
		- icon – an icon to use for the library
			- file – relative path from the top-level directory of this library to an image file containing the icon
		- compatibility – a list of Modelica tools that this library is compatible with
			- tool – (multiple)
				- name – the name of the tool
				- minversion – minimum version of the tool
	- dependencies – libraries that this library depends on
		- dependency – (multiple) 
			- id – name of the top-level package of the library that is the target of the dependency
			- versions – list of compatible versions
				- version – (multiple)
					- number – the version number
					- providers – list of locations that the target library can be downloaded from
						- provider – (multiple)
							- name – human-readable name of provider
							- uri – direct-download URI to the target library

### Library Section

The library part of the manifest file contains the only mandatory fields in the file and they are library version and language version.

Minimal manifest file without encryption, tools or dependencies:

```XML
<?xml version="1.0" encoding="utf-8"?>
<archive>  
  <manifest version="1.0"/>  
  <library id="MyLibrary">
    <version number="1.0"/>
    <language version="3.2"/>    
  </library>
</archive>
```

### Icon Section

<icon file="Resources/Images/MyIcon.png" />

The icon attribute of the manifest file must be set to a path to an icon file starting from the top-level directory of the library. 

### Packagetool

**packagetool** validates that the icon file exist and set the attribute in the manifest file if option -icon is used.

### Compatibility Section

The compatibility section of the manifeste is created by reading an xml-file that contains the tools to add. The structure of the tool xml file is shown below.

```XML
<?xml version="1.0" encoding="utf-8"?>
<compatibility>  
  <tool name="tool 1" minversion="1.0"/>  
  <tool name="tool 2" minversion="3.0"/>  
</compatibility>
```

The first line in the file is ignored but the rest of the file is copied into the manifest file. If the supplied path to the xml-file is wrong the tool will abort. The tool doesn't perform any validation of the xml-file, like spell checking, so it's up to the user to make sure the content of the file is valid.

### Dependencies Section

The dependencies part of the manifest file is created by reading an xml-file that contains the dependencies to add. The structure of the file is shown below.

```XML
<?xml version="1.0" encoding="utf-8"?>
<dependencies>  
  <dependency id="depend 1" >  
    <versions>
      <version number="1"/>
        <providers>
          <provider name="Example Company" uri="www.example.com" />
        </providers>
      </version>
    </versions>
  </dependency>
</dependencies>
```

The first line in the file is ignored but the rest of the file is copied into the manifest file. If the supplied path to the xml-file is wrong the tool will abort. The tool doesn't perform any validation of the xml-file, like spell checking, so it's up to the user to make sure the content of the file is valid.

## SEMLA - COMMUNICATION PROTOCOL BETWEEN TOOL AND "Library Vendor Executable"

'''plantuml
@startuml

group PERFORMED ONCE
    group Handshake
        Tool -> LVE: VERSION <version>
        LVE [#blue]-> Tool: VERSION <version>
        
        Tool -> LVE: LIB <path>
        LVE [#blue]-> Tool: YES
    end

    group License Check
        Tool -> LVE: FEATURE <feature nam>
        LVE [#blue]-> Tool: YES

        Tool -> LVE: FEATURE <feature nam>
        LVE [#blue]-> Tool: NO
    end
end

group Used as many time as needed
    group Get decrypted file
        Tool -> LVE: FILE <path>
        LVE [#blue]-> Tool: FILECONTENT <contents>
    end
end

@enduml
'''

                              ┌────┐                  ┌───┐                    
                              │Tool│                  │LVE│                    
                              └─┬──┘                  └─┬─┘                    
                                │                       │                      
          ╔═════════════════╤═══╪═══════════════════════╪═════════════════════╗
          ║ PERFORMED ONCE  │   │                       │                     ║
          ╟─────────────────┘   │                       │                     ║
          ║         ╔═══════════╪╤══════════════════════╪═══════════╗         ║
          ║         ║ HANDSHAKE  │                      │           ║         ║
          ║         ╟────────────┘  VERSION <version>   │           ║         ║
          ║         ║           │ ──────────────────────>           ║         ║
          ║         ║           │                       │           ║         ║
          ║         ║           │   VERSION <version>   │           ║         ║
          ║         ║           │ <──────────────────────           ║         ║
          ║         ║           │                       │           ║         ║
          ║         ║           │       LIB <path>      │           ║         ║
          ║         ║           │ ──────────────────────>           ║         ║
          ║         ║           │                       │           ║         ║
          ║         ║           │          YES          │           ║         ║
          ║         ║           │ <──────────────────────           ║         ║
          ║         ╚═══════════╪═══════════════════════╪═══════════╝         ║
          ║                     │                       │                     ║
          ║                     │                       │                     ║
          ║         ╔═══════════╪════╤══════════════════╪═══════════╗         ║
          ║         ║ LICENSE CHECK  │                  │           ║         ║
          ║         ╟─────────────FEATURE <feature nam> │           ║         ║
          ║         ║           │ ──────────────────────>           ║         ║
          ║         ║           │                       │           ║         ║
          ║         ║           │          YES          │           ║         ║
          ║         ║           │ <──────────────────────           ║         ║
          ║         ║           │                       │           ║         ║
          ║         ║           │ FEATURE <feature nam> │           ║         ║
          ║         ║           │ ──────────────────────>           ║         ║
          ║         ║           │                       │           ║         ║
          ║         ║           │           NO          │           ║         ║
          ║         ║           │ <──────────────────────           ║         ║
          ║         ╚═══════════╪═══════════════════════╪═══════════╝         ║
          ╚═════════════════════╪═══════════════════════╪═════════════════════╝
                                │                       │                      
                                │                       │                      
          ╔═════════════════════╪════════╤══════════════╪═════════════════════╗
          ║ USED AS MANY TIME AS NEEDED  │              │                     ║
          ╟──────────────────────────────┘              │                     ║
          ║         ╔═══════════╪═════════╤═════════════╪═══════════╗         ║
          ║         ║ GET DECRYPTED FILE  │             │           ║         ║
          ║         ╟──────────────────FILE <path>      │           ║         ║
          ║         ║           │ ──────────────────────>           ║         ║
          ║         ║           │                       │           ║         ║
          ║         ║           │ FILECONTENT <contents>│           ║         ║
          ║         ║           │ <──────────────────────           ║         ║
          ║         ╚═══════════╪═══════════════════════╪═══════════╝         ║
          ╚═════════════════════╪═══════════════════════╪═════════════════════╝
                              ┌─┴──┐                  ┌─┴─┐                    
                              │Tool│                  │LVE│                    
                              └────┘                  └───┘                    

The protocol is intended to be used for proprietary libraries. Open-source libraries are stored as plain text within the library archive.

Encryption of the libraries and SEMLA communication use an entirely separate mechanism. 

The encrypted communication is done through the LVE's stdin and stdout; the tool and the LVE need a public-private key pair. 

The protocol defines : 
	- the establishment a secure communication channel between the tool and LVE (see Handshake below) 
	- the agreement over which version of the SEMLA protocol to use 
	- the decryption of Modelica code and/or licensing. 
	
### Authentication

The LVE contains a list of the public keys of the tools that it trusts; It will only connect to those tools. TODO: ???

The tools implicitly trusts the LVE as the LVE licenses and decrypts the MLC.

### Communication flow

Todo: picture 
 

### Handshake

	- 1 the tool initiates a cryptographic handshake according to TLS 1.2.

	- 2 the tool sends the highest version of the protocol it supports (integer): “VERSION <version>”

	- 3 the LVE responds with the highest version of the protocol supported: “VERSION <version>”
		If tool-version < lve-version, the reply is: “NO <reason>”.

	- 4 the tool sends a path: “LIB <path>”
		- If only the “.library” folder was extracted from the MLC, "<path>" is the path to the library archive
		
		- If the entire contents of the MLC was extracted, "<path>" is the path to the top-level directory of the library

	- the LVE responds: “YES”.


### License check

#### Check for a feature

	- 1 the tool sends: “FEATURE <feature name>”

	- 2 the LVE responds: “YES” or “NO <reason>”

#### Simplified license check

The LVE may **optionally** allow a simplified license check.

In the simplified check, tool only asks for permission for the user to use an entire top-level package contents. 
 
	- 1 the tool sends: “LICENSE <package name>”
	- 2 the LVE answers one of:
		- “YES”
		- “NO <reason>” 
		- “NOTSIMPLE” in case simplified license check is not supported.

### Authentication

The LVE embeds a list of tool public keys that it trusts.

The Tool's public key is compared against the list of trusted keys during the TLS handshake. 

If the tool is not trusted, The LVE still supports SEMLA commands **except** the FILE command which is answered with an error.

Todo: how to add the public keys? default build only handles one tool
Todo: tests for supporting SEMLA for untrusted tools

### Return of licenses

#### Return a feature
1.	Tool sends – “RETURNFEATURE <feature name>”
2.	LVE answers – “YES”.

#### Return a simplified license
1.	Tool sends – “RETURNLICENSE <package name >”
2.	LVE answers - “YES”.

### DECRYPTION

The tool can request an decrypted library file from the LVE using the "FILE" command.

The LVE:
	- can decide to refuse, or accept, "FILE" commands at any time.
	- interprets paths relative to the top level package of the library
	- uses '/' as the path separator
	- ignores special path elements '.' and '..' which may not be used
	- accepts path to directories ending with '/' or not

 
Getting the contents of a file:
1.	Tool sends – “FILE <path>”
2.	LVE answers - “FILECONT <content>”

### ERROR HANDLING

The LVE can respond “ERROR <error code> <error message>” at any time. 
	- <error message> - an error message suitable for display
	- <error code> is one of the following:
		- 1 - Command not understood
		- 2 - Too low version of the protocol – only after VERSION
		- 3 - The LVE doesn’t handle licensing – only after FEATURE or LICENSE
		- 4 - File not found – only after FILE or LIB
		- 5 - This tool is not allowed to decrypt that file – only after FILE
		- 6 - File I/O error
		- 7 - License error
		- 8 - Other error

### General information Query

The tool may, after the cryptographic handshake, query the LVE for general information.

#### List of supported tools:
	- Tool sends – “TOOLS”
	- LVE answers: “TOOLLIST <list>” where "<list>" is:
		- a sequence of Modelica tools in the formatted as:
			- one line for each tool terminated by the line feed character
			- each line formated as: "tool-public-key name-of-the-tool"
				- tool-public-key: a hexadecimal number string without prefix (no “0x”)

#### License information

	- Tool sends– “LICENSEINFO”
	- LVE answers - “TEXT <info>”

	"<info>" can be used to solving license/configuration problems.

### Encoding

Messages are in 8-bit ASCII

## MESSAGE FORMAT

### Simple message with no arguments

	format: "<COMMAND>LN"
		- <COMMAND> - command name, in all caps
		- “LN” - line feed character
		
	Messages using this form: 
		- “LICENSEINFO”
		- “NOTSIMPLE”
		- “TOOLS”
		- “YES”

### Message with a decimal integer data

	format: "<COMMAND> <number>LN"
		- <COMMAND> - command name in all caps
		- <number> - decimal number – a 32-bit signed integer
		- “LN” - line feed character

	Messages using this form: 
		- “VERSION”

### Message with a variable length data

	format: "<COMMAND> <length>LN<data>"
		- <COMMAND> - command name in all caps
		- <length> - data length in bytes – 32-bit signed integer
		- <data> - data bytes

	Messages using this form: 
		- “FEATURE”
		- “FILE”
		- "FILECONT”
		- “LIB”
		- “LICENSE”
		- “NO”
		- “PUBKEY”
		- “RETURNFEATURE”
		- “RETURNLICENSE”
		- “TEXT”
		- “TOOLLIST”

### Message with an integer number and a variable length data

	format: "<COMMAND> <number> <length>LN<data>"
		- <COMMAND> - command name in all caps
		- <number> - a decimal number
		- <length> - data length in bytes (a decimal integer) – 32-bit signed integer TODO: decimal or integer?
		- <data> - data bytes

	Messages using this form:
		- “ERROR”
	
## INSTALLING MODELICA LIBRARY CONTAINER

A library can be installed by the tool:
	- by extracting only the “.library” folder from the library
	- by extracting entire directory structure of the library from the MLC.
		- the directory structure, and file names, of the library must be the same as in the MLC
		- the library can be placed in any location (tool decision) 
		- the library can be renamed (tool decision)

We recommended that:
	- the library is placed in a directory on the MODELICAPATH
	- named “PACKAGENAME” or “PACKAGENAME VERSION” (as per the Modelica Language Specification)

### Reading open-source library

Reading a non-encrypted library is equivalent to reading a library stored on disk (as per the Modelica Language Specification).

### Reading an encrypted library

	- read the XML file with metadata (“manifest.xml”) from the “.library” directory, specifically the path to the LVE suitable for the current platform.
	
	- start the platform specific LVE
	
	- communicate with LVE through its stdin & stdout
	
	- encrypted Modelica files, with a “.moc” extension, are read through the LVE
	
	- non encrypted Modelica files library can also be read through the LVE

## APPENDIX

#### TODO: building SEMLA

### Example “manifest.xml” file for an encrypted library:

<?xml version="1.0" encoding="utf-8"?>
<archive>
  <!-- All paths in the file are interpreted as relative to the directory of the top-level 
       package (e.g., the path to this file would be ".library/manifest.xml"), and allows 
       only forward slashes as directory separators. -->
  <manifest version="1.0"/>
  <!-- The id attribute is the actual Modelica identifier of the library. The file attribute
       from SMA is no longer needed, as it will always be "package.mo" or "package.moc" 
       (and the tool will need the logic of checking for both .mo and .moc anyway).
       The enabled attribute (optional, default value is true) indicates whether the library
       should be enabled/loaded by default. -->
  <library id="ExampleLib" enabled="true">
    <!-- Official title of the library (optional) -->
    <title>Example Encrypted Library</title>
    <!-- Description (optional) -->
    <description>
      Dummy library showing directory structure for an encrypted library (with empty files)
    </description>
    <!-- The version of the library. Version information is formatted according to the
         Modelica language specification. The build and date attributes are optional. -->
    <version number="1.0" build="1" date="2013-08-04"/>
    <!-- Version of the Modelica language that is used in this library. -->
    <language version="3.2" />
    <!-- Copyright notice (optional)-->
    <copyright>
      Copyright © 2014, Modelon AB.
    </copyright>
    <!-- License information (optional) -->
    <license>
      Some license information.
    </license>
    <!-- Encryption/license check information (only for proprietary libraries).
         If this is present, then the library is encrypted. -->
    <encryption>
      <!-- Library vendor executable. May be repeated - one for each supported platform. 
             path:       the path to the executable
             platform:   the OS/platform to use this executable on, must be unique among 
                         executable nodes
             licensing:  if this executable handles licensing (optional, default is true)
           The executable shall be placed under the vendor-specific directory 
           (i.e. .library/VENDORNAME/). The normal case is that licensing has the same 
           value for each executable node, but it is allowed to have different values 
           for different platforms. -->
      <executable path=".library/Modelon/vendor.exe" platform="win32" licensing="true" />
      <executable path=".library/Modelon/vendor32" platform="linux32" licensing="true" />
      <executable path=".library/Modelon/vendor64" platform="linux64" licensing="true" />
    </encryption>
    <!-- Icon for the library (PNG format) (optional) -->
    <icon file="Resources/Images/el.png" />
  </library>
  <!-- Leaving out optional compatibility and dependencies in this example. -->
</archive>

### Requirements for commercial libraries

#### Library Vendor

	- Needs to define which libraries should be available on which tools, e.g.: explicit encoding of supported tools. This is achieved through a list of trusted tool keys.
	
	- Needs to enable/disable parts (features) in any given library based on standardized annotations. Achieved through standard license annotations.
	
	- Needs to specify the visibility of components based on the standardized annotations. Achieved through standard license annotations.
	
	- Need to check the license for a whole library without parsing the Modelica code. Achieved, optionally, through an alternate licensing mechanism for tools that do not understand license annotations.
	
	- Needs to license/protect external libraries (e.g.: with C and F77) independently of the licensing mechanism for the library.
	
	- Needs to make releases, of existing and new libraries, without any action from the tool vendor.
	
	- Needs an encryption mechanism to ensure protection of library vendor IP (the Modelica source code).
	
	- Needs an encryption mechanism which can be used with or without a licensing mechanism.
	
	- Can implement a custom licensing mechanism.
	
	- The protocol offers means to, with reasonable effort, replace encryption keys, in cases of security breaches.

	TODO: ???- All involved keys belonging to library vendors can be switched for any library release.

	Changing the key pair used by the tool could be done by adding a second key pair during a transition period.
	
	Library vendors could then replace the compromised key with the new one in their list of trusted tools.


#### Tool 

	- Store of Modelica source code on disc in unencrypted form is not allowed.
	
	- The Tool support standardized mechanisms for enabling/disabling parts of a library, by means of annotations defined in the Modelica specification.
	
	- Well documented low level API for licensing and encryption.
	
	- Possible customizations of licensing and encryption modules.
	
	- Can display Error messages for errors happening on the LVE side.
	
	- Alternative, tool specific, licensing mechanisms is possible.
	
	- Can create a container which includes multiple libraries/top-level packages, in a single file.

	- May have more than one key pair. For tools that have more than one key pair, the LVE needs to be restarted in between trying different key pairs.
	
#### User

	- The API shall enable convenient to installation procedures for libraries. Possible with tool support, 
	
	- library is distributed as a single file with a well-defined file extension.
	
	- Error messages enable users to isolate errors related to licensing and encryption.

## Vulnerability

### Analysis

Consequences if a cryptographic key is extracted or altered: 

	- Session key (generated for each session)
		- Extraction: allows eavesdropping on the communication.
		- Alteration: not possible, would break the communication between the tool and the LVE.
		
	- Tool private key 
		- Extraction: allows decryption of all libraries that trust the key.
		- Alteration: LVEs will stop trusting the key and no communication would be possible.
		
	- LVE trusted keys list
		- Extraction: not useful to attacker.
		- Alteration: adding the attacker key to the list allows the decryption of the library.

	- LVE key used to decrypt the library
		- Extraction: allows the decryption the library.
		- Alteration: not useful to attacker, makes it impossible for the LVE to decrypt libraries.
		
The most serious breach is an attacker obtaining the tool private key; it allows the decryption of any library released for that tool.
		
These vulnerabilities exists in any tool that supports encrypted libraries and embed encryption keys.

### Mitigation

Keys included in the tool and LVEs binaries must be protected with an obfuscation scheme.

For **each** release of libraries:
	- change the obfuscation code, 
	- **each** library gets a new randomly generated SSL keys set and Randomized Key, even within a single organization. 
	



