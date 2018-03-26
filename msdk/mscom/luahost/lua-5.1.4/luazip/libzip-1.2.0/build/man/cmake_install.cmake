# Install script for directory: G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE RENAME "zipcmp.1" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zipcmp.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE RENAME "zipmerge.1" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zipmerge.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE RENAME "ziptool.1" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/ziptool.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "ZIP_SOURCE_GET_ARGS.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/ZIP_SOURCE_GET_ARGS.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "libzip.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/libzip.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_add.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_add.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_add_dir.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_add_dir.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_close.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_close.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_delete.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_delete.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_dir_add.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_dir_add.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_discard.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_discard.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_clear.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_clear.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_code_system.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_code_system.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_code_zip.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_code_zip.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_fini.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_fini.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_get.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_get.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_get_sys_type.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_get_sys_type.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_init.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_init.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_set.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_set.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_strerror.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_strerror.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_system_type.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_system_type.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_to_data.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_to_data.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_to_str.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_to_str.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_errors.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_errors.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fclose.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_fclose.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fdopen.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_fdopen.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_add.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_add.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_delete.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_extra_field_delete.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_get.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_extra_field_get.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_set.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_extra_field_set.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_fields_count.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_extra_fields_count.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_get_comment.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_get_comment.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_get_error.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_get_error.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_rename.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_rename.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_set_comment.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_set_comment.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_set_encryption.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_set_encryption.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_set_external_attributes.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_set_external_attributes.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_set_mtime.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_set_mtime.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_strerror.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_strerror.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fopen.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_fopen.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fopen_encrypted.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_fopen_encrypted.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fread.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_fread.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fseek.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_fseek.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_archive_comment.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_get_archive_comment.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_archive_flag.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_get_archive_flag.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_error.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_get_error.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_file_comment.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_get_file_comment.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_name.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_get_name.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_num_entries.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_get_num_entries.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_num_files.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_get_num_files.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_name_locate.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_name_locate.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_open.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_open.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_register_progress_callback.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_register_progress_callback.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_rename.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_rename.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_archive_comment.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_set_archive_comment.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_archive_flag.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_set_archive_flag.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_default_password.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_set_default_password.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_file_comment.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_set_file_comment.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_file_compression.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_set_file_compression.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_begin_write.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_begin_write.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_buffer.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_buffer.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_close.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_close.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_commit_write.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_commit_write.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_error.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_error.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_file.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_file.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_filep.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_filep.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_free.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_free.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_function.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_function.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_is_deleted.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_is_deleted.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_keep.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_keep.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_make_command_bitmap.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_make_command_bitmap.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_open.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_open.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_read.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_read.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_rollback_write.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_rollback_write.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_seek.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_seek.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_seek_compute_offset.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_seek_compute_offset.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_seek_write.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_seek_write.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_stat.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_stat.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_tell.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_tell.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_tell_write.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_tell_write.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_win32a.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_win32a.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_win32handle.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_win32handle.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_win32w.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_win32w.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_write.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_write.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_zip.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_zip.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_stat.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_stat.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_stat_init.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_stat_init.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_unchange.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_unchange.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_unchange_all.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_unchange_all.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_unchange_archive.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_unchange_archive.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_error_clear.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_clear.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_error_get.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_get.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_init_with_code.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_error_init.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_delete_by_id.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_extra_field_delete.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_get_by_id.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_extra_field_get.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_fields_count_by_id.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_extra_fields_count.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_replace.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_add.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fopen_index.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_fopen.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fopen_index_encrypted.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_fopen_encrypted.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_open_from_source.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_open.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_replace.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_add.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_function_create.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_source_function.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_stat_index.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_stat.man")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_strerror.3" FILES "G:/svn/maguojun/win/msdk/msdk/mscom/luahost/lua-5.1.4/libzip-1.2.0/man/zip_file_strerror.man")
endif()

