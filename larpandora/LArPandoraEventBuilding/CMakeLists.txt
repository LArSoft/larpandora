include_directories( ${PandoraSDK_INCLUDE_DIRS} )
include_directories( ${PandoraMonitoring_INCLUDE_DIRS} )
include_directories( ${EIGEN3_INCLUDE_DIRS} )
include_directories( ${LArPandoraContent_INCLUDE_DIRS} )

add_subdirectory(LArPandoraShower)

art_make(
          EXCLUDE
                SimpleNeutrinoId_tool.cc
          LIB_LIBRARIES 
                larcorealg_Geometry
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
                art::Framework_Core
                art::Framework_Principal
                art::Framework_Services_Registry
                art::Framework_Services_Registry 
                art::Framework_Services_FileServiceInterfaces
                art_root_io::tfile_support ROOT::Core
                art_root_io::TFileService_service
                art_Persistency_Common
                art_Persistency_Provenance
                art_Utilities
                canvas
                messagefacility::MF_MessageLogger
                fhiclcpp::fhiclcpp fhiclcpp::types
                cetlib cetlib_except
                Boost::system
                Boost::filesystem
                ROOT::Geom
                ROOT::Graf3d
                ROOT::Gpad
                ROOT::Physics
                ROOT::MathCore
                ROOT::Hist
                ROOT::Core
                ROOT::GenVector
                MODULE_LIBRARIES larpandora_LArPandoraEventBuilding
          )

      simple_plugin(SimpleNeutrinoId "tool" LIBRARIES larpandora_LArPandoraEventBuilding)

install_headers()
install_fhicl()
