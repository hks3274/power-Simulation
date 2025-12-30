# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Qtv2_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Qtv2_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\qcustomplot_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\qcustomplot_autogen.dir\\ParseCache.txt"
  "Qtv2_autogen"
  "qcustomplot_autogen"
  )
endif()
