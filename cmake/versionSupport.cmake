# This script determines the git revision of the project if possible.
#
# Processed variables:
#   RELEASE....................omit the patch from all version strings when true
#
# Provided variables:
#   GIT_INITIALIZED............true if the git revision of the project could
#                              be determined
#   GIT_MODIFIED...............true if the working directory has been modified
#   GIT_SHORT_HASH.............the short hash of the repository
#   GIT_COMBINED_SHORT_HASH....the short hash with an "M" prefix
#                              when GIT_MODIFIED == true
#
#   VERSION_PATCH..............GIT_COMBINED_SHORT_HASH
#   VERSION_SHORT..............VERSION_MAJOR.VERSION_MINOR
#   VERSION_FULL...............VERSION_MAJOR.VERSION_MINOR(.VERSION_PATCH)?
#

FIND_PROGRAM(GIT_COMMAND NAMES git)
MARK_AS_ADVANCED(GIT_COMMAND)

SET(GIT_INITIALIZED "OFF")
SET(GIT_MODIFIED "OFF")
SET(GIT_SHORT_HASH  "")
SET(GIT_COMBINED_SHORT_HASH "")

IF(GIT_COMMAND)
  # fetch the hash from the last commit
  EXECUTE_PROCESS(COMMAND "${GIT_COMMAND}" "log" "--pretty=format:%h" "-1"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_SHORT_HASH)
    
  SET(GIT_COMBINED_SHORT_HASH "${GIT_SHORT_HASH}")
  
  # check if the source dir is a repository at all
  STRING(COMPARE NOTEQUAL "" "${GIT_SHORT_HASH}" GIT_INITIALIZED)
  IF(GIT_INITIALIZED)
    EXECUTE_PROCESS(COMMAND "${GIT_COMMAND}" "status"
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_MODIFIED)

    STRING(REGEX MATCH "modified:" GIT_MODIFIED "${GIT_MODIFIED}")
    IF(GIT_MODIFIED)
      MESSAGE(STATUS "GIT: workspace has been modified")
      SET(GIT_COMBINED_SHORT_HASH "M${GIT_SHORT_HASH}")
    ENDIF(GIT_MODIFIED)
  ELSE()
    MESSAGE(STATUS "GIT: hash couldn't be determined")
  ENDIF()
ENDIF()

OPTION(RELEASE "Omit the patch version for the release." OFF)

SET(VERSION_PATCH "")
SET(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}")
SET(VERSION_FULL  "${VERSION_SHORT}")

IF(GIT_INITIALIZED AND NOT RELEASE )
  SET(VERSION_PATCH "${GIT_COMBINED_SHORT_HASH}")
  SET(VERSION_FULL "${VERSION_SHORT}.${VERSION_PATCH}")
ENDIF()
