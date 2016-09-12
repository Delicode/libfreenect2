# FindIntelMediaSDK.cmake
# - IntelMediaSDK_FOUND
# - IntelMediaSDK_INCLUDE_DIRS
# - IntelMediaSDK_LIBRARIES

FIND_PATH(IntelMediaSDK_INCLUDE_DIRS
  mfxdefs.h
  DOC "Found IntelMediaSDK include directory"
  PATHS
    "${DEPENDS_DIR}/intel_media_sdk"
    "${DEPENDS_DIR}/intel_media_sdk64"
    "C:/Program Files (x86)/IntelSWTools/Intel(R)_Media_SDK_2016.0.2"   # How should this be done without depending on specific version?
    "/opt/intel/mediasdk/include"
    ENV IntelMediaSDK_ROOT
  PATH_SUFFIXES
    include
)

FIND_LIBRARY(IntelMediaSDK_LIBRARIES
  NAMES libmfx.a libmfx.lib
  DOC "Found IntelMediaSDK library path"
  PATHS
    "${DEPENDS_DIR}/intel_media_sdk"
    "${DEPENDS_DIR}/intel_media_sdk64"
    "C:/Program Files (x86)/IntelSWTools/Intel(R)_Media_SDK_2016.0.2/lib"
    "/opt/intel/mediasdk"
    ENV IntelMediaSDK_ROOT
  PATH_SUFFIXES
    lib
    lib64
    x64
    x86
)

IF(WIN32)
FIND_FILE(IntelMediaSDK_DLL
  libmfxsw64.dll
  DOC "Found IntelMediaSDK DLL path"
  PATHS
    "${DEPENDS_DIR}/intel_media_sdk"
    "${DEPENDS_DIR}/intel_media_sdk64"
    "C:/Program Files (x86)/IntelSWTools/Intel(R)_Media_SDK_2016.0.2"
    ENV IntelMediaSDK_ROOT
  PATH_SUFFIXES
    bin
)
ENDIF()

IF(IntelMediaSDK_INCLUDE_DIRS AND IntelMediaSDK_LIBRARIES)
INCLUDE(CheckCSourceCompiles)
set(CMAKE_REQUIRED_INCLUDES ${IntelMediaSDK_INCLUDE_DIRS})
set(CMAKE_REQUIRED_LIBRARIES ${IntelMediaSDK_LIBRARIES})
set(CMAKE_REQUIRED_DEFINITIONS)
set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_LIBRARIES)
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(IntelMediaSDK FOUND_VAR IntelMediaSDK_FOUND
  REQUIRED_VARS IntelMediaSDK_LIBRARIES IntelMediaSDK_INCLUDE_DIRS)
