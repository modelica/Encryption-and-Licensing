# Copyright (C) 2022 Modelica Association
from pymodelica import compile_fmu
compile_fmu('test_library.test', 'test_library')
print('Compiled')
