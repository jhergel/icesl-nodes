CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(NodeInterface)
Set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

Include(UseCXX11)

add_definitions(-DSRC_PATH=\"${CMAKE_SOURCE_DIR}/\")

IF(WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
  SET(CMAKE_EXE_LINKER_FLAGS "/LARGEADDRESSAWARE")
ELSE()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_NO_CXX11_SCOPED_ENUMS")
  find_package(Qt4 REQUIRED QtCore QtGui)
  INCLUDE(${QT_USE_FILE})
ENDIF()


SET(LibSL_DIR $ENV{LIBSL_DIR})
FIND_PACKAGE(LibSL)



include_directories(.
                    ${LIBSL_INCLUDE_DIR}
                    )



if(UNIX)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(. ${BOOST_INCLUDE_DIR})

ENDIF()

#SET(CMAKE_BUILD_TYPE Debug)


SET(SOURCES
 NodeWindow.h
 NodeWindow.cpp
 NodeLua.cpp
 NodeLua.h
 main.cpp
 graphMaker.cpp
 graphMaker.h
 graphError.h
 FileDialog.cpp
 FileDialog.h
 project.h
 tweaks.h
 tweaks.cpp
 GenericWindow.h
 GraphSaver.h
 GraphSaver.cpp

)

INCLUDE_DIRECTORIES( ${LIBSL_INCLUDE_DIR} )

add_subdirectory(libremote_public)

IF(NOT APPLE)
ADD_DEFINITIONS(-DUSE_GLUX)
ELSE()
ADD_DEFINITIONS(-DAPPLE)
ENDIF(NOT APPLE)

if(WIN32)
set(USER $ENV{USERNAME})
else(WIN32)
set(USER $ENV{USER})
endif(WIN32)

ADD_EXECUTABLE(NodeInterface ${SOURCES} ${HEADERS})
IF(WIN32)
TARGET_LINK_LIBRARIES(NodeInterface libremote_public ${LIBSL_LIBRARIES} ${LIBSL_GL_LIBRARIES} )
ELSE(WIN32)
TARGET_LINK_LIBRARIES(NodeInterface libremote_public LibSL LibSL_gl pthread
 ${Boost_FILESYSTEM_LIBRARY}
 ${Boost_SYSTEM_LIBRARY}
 ${QT_LIBRARIES}
)
ENDIF(WIN32)

add_definitions(-DPATHTOSRC="${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(NodeInterface PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

