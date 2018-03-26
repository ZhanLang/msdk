# Install script for directory: G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "G:\svn\maguojun\win\msdk\msdk\mscom\luahost\lua-5.1.4\libzip-1.2.0/libzip")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/libzip/include" TYPE FILE FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/build/zipconf.h")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/build/lib/cmake_install.cmake")
  include("G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/build/man/cmake_install.cmake")
  include("G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/build/src/cmake_install.cmake")
  include("G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/build/regress/cmake_install.cmake")
  include("G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/build/examples/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
