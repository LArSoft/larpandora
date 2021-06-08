include_directories( ${PandoraSDK_INCLUDE_DIRS} )
include_directories( ${PandoraMonitoring_INCLUDE_DIRS} )
include_directories( ${EIGEN3_INCLUDE_DIRS} )
include_directories( ${LArPandoraContent_INCLUDE_DIRS} )

set(CORE_LIB_LIST
    larcorealg_Geometry
    larcore_Geometry_Geometry_service
    larsim_Simulation lardataobj_Simulation
    larsim_MCCheater_ParticleInventoryService_service
    lardataalg_DetectorInfo
    lardataobj_RawData
    lardataobj_RecoBase
    lardataobj_AnalysisBase
    lardata_ArtDataHelper
    lardata_Utilities
    larreco_RecoAlg_ClusterRecoUtil
    larreco_RecoAlg
    larreco_ClusterFinder
    larreco_Calorimetry
    ${PANDORASDK}
    ${PANDORAMONITORING}
    LArPandoraContent
    nusimdata_SimulationBase
    ${ART_FRAMEWORK_CORE}
    ${ART_FRAMEWORK_PRINCIPAL}
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
    ${ART_FRAMEWORK_SERVICES_BASIC}
    ${ART_ROOT_IO_TFILE_SUPPORT} ROOT::Core
    ${ART_ROOT_IO_TFILESERVICE_SERVICE}
    art_Persistency_Common
    art_Persistency_Provenance
    art_Utilities
    canvas
    messagefacility::MFMessageLogger
    fhiclcpp::fhiclcpp fhiclcpp::types
    cetlib cetlib_except
    ROOT::Geom
    ${ROOT_BASIC_LIB_LIST})

option(PANDORA_LIBTORCH "Flag for building with Pandora's LibTorch-aware algorithms" ON)
if( ${PANDORA_LIBTORCH} AND DEFINED ENV{LIBTORCH_DIR})
    message(STATUS "Building with deep learning support")
    add_definitions("-DLIBTORCH_DL")
    art_make(
        LIB_LIBRARIES ${CORE_LIB_LIST} LArPandoraDLContent
        MODULE_LIBRARIES larpandora_LArPandoraInterface)
else()
    message( STATUS "Building without deep learning support" )
    art_make(
        LIB_LIBRARIES ${CORE_LIB_LIST}
        MODULE_LIBRARIES larpandora_LArPandoraInterface)
endif()

install_headers()
install_fhicl()
install_source()

add_subdirectory(scripts)
