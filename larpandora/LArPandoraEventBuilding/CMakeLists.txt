include_directories( $ENV{PANDORA_INC} )
include_directories( $ENV{LARPANDORACONTENT_INC} )
include_directories( $ENV{EIGEN_INC} )

add_subdirectory(LArPandoraShower)

art_make( EXCLUDE SimpleNeutrinoId_tool.cc
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
                        PANDORASDK
                        PANDORAMONITORING
                        LArPandoraContent
                        larpandora_LArPandoraInterface
                        nusimdata_SimulationBase
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        ${ART_FRAMEWORK_SERVICES_BASIC}
                        art_root_io::tfile_support ROOT::Core
                        art_root_io::TFileService_service
                        art::Persistency_Common
                        art::Persistency_Provenance
                        art::Utilities
                        canvas::canvas
                        messagefacility::MF_MessageLogger
                        fhiclcpp::fhiclcpp
                        cetlib::cetlib cetlib_except
                        ${Boost_SYSTEM_LIBRARY}
                        ${Boost_FILESYSTEM_LIBRARY}
                        ROOT::Geom
                        ${ROOT_BASIC_LIB_LIST}
                        ROOT::GenVector
          MODULE_LIBRARIES larpandora_LArPandoraEventBuilding
          )

      simple_plugin(SimpleNeutrinoId "tool" larpandora_LArPandoraEventBuilding)

install_headers()
install_fhicl()
install_source()
