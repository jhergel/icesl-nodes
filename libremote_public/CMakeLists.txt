CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include(UseCXX11)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_DEFINITIONS( -DASIO_STANDALONE )

ADD_LIBRARY(libremote_public
        remote_handler.h
        remote_handler.cpp
)

TARGET_LINK_LIBRARIES(libremote_public
	LibSL
)


SET_PROPERTY(TARGET libremote_public APPEND PROPERTY
   INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR})
SET_TARGET_PROPERTIES(libremote_public PROPERTIES DEBUG_POSTFIX "-d")
