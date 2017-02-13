# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")

FIND_PATH(UUID_INCLUDE_DIR uuid/uuid.h
  $ENV{UUID_HOME}/include
  /usr/include
  /usr/local/include
)

IF(NOT UUID_INCLUDE_DIR)
  MESSAGE(SEND_ERROR "- libuuid headers (uuid.h) not found.")
ENDIF()


FIND_LIBRARY(UUID_LIBRARY
  NAMES uuid UUID  libuuid.so.1
  PATHS
  $ENV{UUID_HOME}/lib
  /usr/lib
  /usr/local/lib
)

IF(NOT UUID_LIBRARY)
  MESSAGE(SEND_ERROR "- libuuid not found.")
ENDIF()

MARK_AS_ADVANCED(
  UUID_INCLUDE_DIR
  UUID_LIBRARY
)
