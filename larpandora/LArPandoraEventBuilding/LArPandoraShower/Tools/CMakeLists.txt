add_subdirectory(Cheating)

art_make(TOOL_LIBRARIES
  larpandora_LArPandoraEventBuilding_LArPandoraShower_Algs
  larreco_Calorimetry
  LArPandoraContent
  ROOT::Graf3d
  ROOT::Gpad
  ROOT::Physics
  ROOT::MathCore
  ROOT::Hist
  ROOT::Tree
  ROOT::Core
  canvas
  fhiclcpp::fhiclcpp fhiclcpp::types
  cetlib_except
  messagefacility::MF_MessageLogger
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art_root_io::tfile_support
  art_root_io::TFileService_service
  art_Persistency_Provenance
  art_Utilities
  )

install_headers()
install_source()
install_fhicl()
