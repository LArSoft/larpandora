# ======================================================================
#  larpandora main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larpandora/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

project(larpandora VERSION 09.05.10 LANGUAGES CXX)

set(larpandora_FW_DR fw)

find_package(cetmodules REQUIRED)
find_package(canvas_root_io)
find_package(art)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
  )

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larcoreobj )
find_ups_product( larcorealg )
find_ups_product( larcore )
find_ups_product( lardataobj )
find_ups_product( lardataalg )
find_ups_product( lardata )
find_ups_product( larevt )
find_ups_product( larsim )
find_ups_product( larreco )
find_ups_product( nug4)
find_ups_product( art )
find_ups_product(art_root_io)
find_ups_product( pandora )
find_package( postgresql )
find_ups_product( eigen )
find_package(LArPandoraContent REQUIRED)

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

cet_find_library( PANDORASDK NAMES PandoraSDK PATHS ENV PANDORA_LIB )
cet_find_library( PANDORAMONITORING NAMES PandoraMonitoring PATHS ENV PANDORA_LIB )

# find larpandoracontent headers if building at the same time
#message(STATUS "larpandora: checking for MRB_SOURCE")
set( mrb_source $ENV{MRB_SOURCE} )

message("LARPANDORACONTENT_INC '${LARPANDORACONTENT_INC}' LArPandoraContent_INCLUDE_DIRS '${LArPandoraContent_INCLUDE_DIRS}'")
if( NOT LARPANDORACONTENT_INC )
set(LARPANDORACONTENT_INC "${LArPandoraContent_INCLUDE_DIRS}")
endif()

if( LARPANDORACONTENT_INC )
  #message(STATUS "larpandora: found LARPANDORACONTENT_INC: $ENV{LARPANDORACONTENT_INC}")
  # note that mrb v1_07_00 or later will define LARPANDORACONTENT_INC
  if( mrb_source )
    include_directories( ${LARPANDORACONTENT_INC} )
  endif()
else()
  if( mrb_source )
    include_directories( ${mrb_source}/larpandoracontent )
  else()
    message(FATAL_ERROR "LARPANDORACONTENT_INC is undefined, please setup larpandoracontent")
  endif()
endif()

# source
add_subdirectory(larpandora)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)

