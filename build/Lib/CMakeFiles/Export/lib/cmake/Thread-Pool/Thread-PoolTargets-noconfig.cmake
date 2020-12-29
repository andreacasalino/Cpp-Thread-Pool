#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EFG::Thread-Pool" for configuration ""
set_property(TARGET EFG::Thread-Pool APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(EFG::Thread-Pool PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libThread-Pool.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS EFG::Thread-Pool )
list(APPEND _IMPORT_CHECK_FILES_FOR_EFG::Thread-Pool "${_IMPORT_PREFIX}/lib/libThread-Pool.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
