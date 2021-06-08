include_directories( ${PandoraSDK_INCLUDE_DIRS} )
include_directories( ${PandoraMonitoring_INCLUDE_DIRS} )
include_directories( ${EIGEN3_INCLUDE_DIRS} )
include_directories( ${LArPandoraContent_INCLUDE_DIRS} )

add_subdirectory(LArPandoraShower)

art_make(
          LIB_LIBRARIES larcorealg_Geometry
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
                        ${PandoraSDK_LIBRARIES}
                        ${PandoraMonitoring_LIBRARIES}
                        LArPandoraContent
                        larpandora_LArPandoraInterface
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
                        Boost::system
                        Boost::filesystem
                        ROOT::Geom
                        ${ROOT_LIBRARIES}
                        ROOT::GenVector
                        MODULE_LIBRARIES larpandora_LArPandoraEventBuilding
          )

      simple_plugin(SimpleNeutrinoId "tool" larpandora_LArPandoraEventBuilding)

install_headers()
install_fhicl()
