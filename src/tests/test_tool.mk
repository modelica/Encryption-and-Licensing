
# Targets: clean and test
#	clean_ remove test artefacts
#	test: create an encrypted library and test it with current SEMLA packagetool and test_tool


# $> mkdir -p builds/build directory 
# $> cd builds/build_directory
# $> CMake /SEMLA/repos/SEMLA/src -G"generator" # generator is "Unix Makefiles" or "NMAKE Makefiles"
# or $> cmake /SEMLA/repos/SEMLA/src -DKEYS_SOURCE_DIRECTORY=/SEMLA/repos/openssl_keys/  -G"Unix Makefiles"
# $> make -f test_tool.mk clean 
# $> cmake --build .
# $> make -f test_tool.mk test

PACKAGE_TOOL_TEMP_DIR:=packagetool_temp_dir
PACKAGETOOL_LVE_DIR:=LVE

ifeq ($(OS),Windows_NT)
	EXE:=.exe
    LVE:=lve.exe
	PLATFORM_LVE:=lve_win64.exe
	PACKAGE_TOOL_TEMP_VARABLE:=TEMP
else
    LVE:=lve
	PLATFORM_LVE:=lve_linux64
	PACKAGE_TOOL_TEMP_VARABLE:=TMP
endif

PACKAGETOOL=./packagetool$(EXE)
TESTTOOL=./test_tool$(EXE)

test: test_library_encrypted_result

test_library_encrypted_result: test_library.mol $(TESTTOOL) 
	unzip -o test_library.mol
	chmod a+x test_library/.library/lve*
	$(TESTTOOL) $(PLATFORM_LVE)
	echo OK > $@

test_library.mol: $(PACKAGETOOL_LVE_DIR)/$(PLATFORM_LVE) test_facit $(PACKAGETOOL)
	cp -r test_facit test_library
	
	mkdir -p $(PACKAGE_TOOL_TEMP_DIR)
	export $(PACKAGE_TOOL_TEMP_VARABLE)=$(PACKAGE_TOOL_TEMP_DIR)	
	
	$(PACKAGETOOL) -librarypath test_library -version "2.0" -language "3.2" -encrypt "true"
	rm -rf test_library

$(PACKAGETOOL_LVE_DIR)/$(PLATFORM_LVE): $(PLATFORM_LVE)
	rm -f $(LVE) && sleep 1
	mkdir -p $(PACKAGETOOL_LVE_DIR)
	cp $(PLATFORM_LVE) $(PACKAGETOOL_LVE_DIR)

$(PLATFORM_LVE): 
	test -f $(LVE) && mv $(LVE) $(PLATFORM_LVE)

clean:
	rm -rf $(PACKAGETOOL_LVE_DIR)/
	rm -rf test_library/
	rm -f test_library_encrypted_result 
	rm -f test_library.mol

.PHONY: test clean 


