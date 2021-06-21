art_make(LIB_LIBRARIES
  larcorealg_Geometry
  ROOT::Gpad
  ROOT::Physics
  ROOT::MathCore
  ROOT::Hist
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
  nusimdata_SimulationBase
  lardataobj_RecoBase
  larsim_MCCheater_BackTrackerService_service
  larsim_MCCheater_ParticleInventoryService_service
  lardataalg_DetectorInfo
  )

install_headers()
install_source()
install_fhicl()
