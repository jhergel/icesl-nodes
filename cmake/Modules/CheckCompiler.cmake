################################################################################
CMake_Minimum_Required(VERSION 2.8.11)
################################################################################

If(MSVC)
  If(MSVC_VERSION VERSION_LESS 1800)
    Message(FATAL_ERROR "You should use a more recent compiler (MSVC 12.0 or higher).")
  Endif()
Endif()
