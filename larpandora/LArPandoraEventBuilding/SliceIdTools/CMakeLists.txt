include_directories( $ENV{PANDORA_INC} )
include_directories( $ENV{LARPANDORACONTENT_INC} )

set( 
          sliceid_tool_lib_list larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        larsim_Simulation lardataobj_Simulation
                        lardataobj_RawData
                        lardataobj_RecoBase
                        lardataobj_AnalysisBase
                        lardata_ArtDataHelper
                        lardata_Utilities
                        larreco_RecoAlg_ClusterRecoUtil
                        larreco_RecoAlg
                        larreco_ClusterFinder
                        larreco_Calorimetry
                        PandoraSDK
                        PandoraMonitoring
                        LArPandoraContent
                        larpandora_LArPandoraInterface
                        larpandora_LArPandoraEventBuilding
                        nusimdata_SimulationBase
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_BASIC}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        art_Persistency_Common
                        art_Persistency_Provenance
                        art_Utilities
                        canvas
                        ${MF_MESSAGELOGGER}
                        ${FHICLCPP}
                        cetlib cetlib_except
                        ${Boost_SYSTEM_LIBRARY}
                        ${Boost_FILESYSTEM_LIBRARY}
                        ${ROOT_GEOM}
                        ${ROOT_BASIC_LIB_LIST}
                        ${ROOT_GENVECTOR}
                        MODULE_LIBRARIES larpandora_LArPandoraEventBuilding
          )

art_make(TOOL_LIBRARIES ${sliceid_tool_lib_list})

install_headers()
install_fhicl()
install_source()
