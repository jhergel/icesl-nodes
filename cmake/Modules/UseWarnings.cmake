################################################################################
CMake_Minimum_Required(VERSION 2.6.3)
################################################################################

# For compiling with very a paranoid and verbose output

Message(STATUS "Setting warning levels to beyond paranoid")

# Disabled for now
If (0)
	Set(WLVL0 "-Wall -Wextra -pedantic")
	
	Set(WLVL1 "-Wconversion -Wunsafe-loop-optimizations -Wunused")
	
	Set(WLVL2 "-Wpointer-arith -Wmissing-declarations -Wformat=2")
	Set(WLVL2 "${WLVL2} -Wuninitialized -Wshadow -Wcast-qual -Wmissing-noreturn")
	Set(WLVL2 "${WLVL2} -Wmissing-format-attribute -Wredundant-decls")
	
	Set(WERR         "-Werror=implicit -Werror=nonnull -Werror=init-self")
	Set(WERR "${WERR} -Werror=main -Werror=missing-braces -Werror=sequence-point")
	Set(WERR "${WERR} -Werror=return-type -Werror=trigraphs -Werror=array-bounds")
	Set(WERR "${WERR} -Werror=write-strings -Werror=address")
	Set(WERR "${WERR} -Werror=int-to-pointer-cast -Werror=pointer-to-int-cast")
	
	Set(WOTHER "-Wno-unused-variable -Wno-write-strings -Wno-unused-parameter")
	Set(WOTHER "${WOTHER} -Wno-unused-but-set-variable -Wno-unused-result")
	
	Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WLVL0} ${WLVL1} ${WLVL2}")
	Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WERR} ${WOTHER}")
Endif()

# TODO: Clean LibSL headers with regards to -Wshadow and -Wconversion warnings
Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wunused -Wpointer-arith -Wformat=2 -Wuninitialized -Wcast-qual -Wmissing-noreturn -Wmissing-format-attribute -Wredundant-decls -Werror=implicit -Werror=nonnull -Werror=init-self -Werror=main -Werror=missing-braces -Werror=sequence-point -Werror=return-type -Werror=trigraphs -Werror=array-bounds -Werror=write-strings -Werror=address -Werror=int-to-pointer-cast -Werror=pointer-to-int-cast -Wno-unused-variable -Wno-write-strings -Wno-unused-parameter")

Include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-Wunsafe-loop-optimizations" WUNSAFE_LOOP)
If(WUNSAFE_LOOP)
	Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunsafe-loop-optimizations")
Endif()
