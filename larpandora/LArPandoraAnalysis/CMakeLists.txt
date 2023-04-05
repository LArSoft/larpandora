cet_build_plugin(ConsolidatedPFParticleAnalysisTemplate art::EDAnalyzer
  LIBRARIES PRIVATE
  lardataobj::RecoBase
  larcorealg::Geometry
  larcoreobj::SimpleTypesAndConstants
  nusimdata::SimulationBase
  art_root_io::TFileService_service
  art_root_io::tfile_support
  art::Framework_Principal
  art::Framework_Services_Registry
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  PandoraPFA::PandoraSDK
)

cet_build_plugin(PFParticleAnalysis art::EDAnalyzer
  LIBRARIES PRIVATE
  larpandora::LArPandoraInterface
  larcore::Geometry_Geometry_service
  lardataobj::AnalysisBase
  lardataobj::RecoBase
  art_root_io::TFileService_service
  art_root_io::tfile_support
  art::Framework_Principal
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  ROOT::Physics
  ROOT::Tree
)

cet_build_plugin(PFParticleCosmicAna art::EDAnalyzer
  LIBRARIES PRIVATE
  larpandora::LArPandoraInterface
  larcore::Geometry_Geometry_service
  lardataobj::AnalysisBase
  lardataobj::RecoBase
  nusimdata::SimulationBase
  art_root_io::TFileService_service
  art_root_io::tfile_support
  art::Framework_Principal
  art::Framework_Services_Registry
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  ROOT::Tree
)

cet_build_plugin(PFParticleHitDumper art::EDAnalyzer
  LIBRARIES PRIVATE
  larpandora::LArPandoraInterface
  lardata::AssociationUtil
  lardata::DetectorClocksService
  lardata::DetectorPropertiesService
  lardata::LArPropertiesService
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  larcorealg::Geometry
  larcoreobj::SimpleTypesAndConstants
  nusimdata::SimulationBase
  art_root_io::TFileService_service
  art_root_io::tfile_support
  art::Framework_Principal
  art::Framework_Services_Registry
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  ROOT::Tree
)

cet_build_plugin(PFParticleMonitoring art::EDAnalyzer
  LIBRARIES PRIVATE
  larpandora::LArPandoraInterface
  lardata::AssociationUtil
  lardata::DetectorClocksService
  lardata::DetectorPropertiesService
  lardata::LArPropertiesService
  larcore::Geometry_Geometry_service
  lardataobj::AnalysisBase
  lardataobj::RecoBase
  larcorealg::Geometry
  nusimdata::SimulationBase
  art_root_io::TFileService_service
  art_root_io::tfile_support
  art::Framework_Principal
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  ROOT::Tree
)

cet_build_plugin(PFParticleTrackAna art::EDAnalyzer
  LIBRARIES PRIVATE
  larpandora::LArPandoraInterface
  lardata::DetectorPropertiesService
  lardata::LArPropertiesService
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  art_root_io::TFileService_service
  art_root_io::tfile_support
  art::Framework_Principal
  art::Framework_Services_Registry
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  ROOT::Tree
)

cet_build_plugin(PFParticleValidation art::EDAnalyzer
  LIBRARIES PRIVATE
  larpandora::LArPandoraInterface
  lardataobj::RecoBase
  larcoreobj::SimpleTypesAndConstants
  nusimdata::SimulationBase
  art::Framework_Principal
  fhiclcpp::fhiclcpp
)

install_headers()
install_fhicl()
install_source()
