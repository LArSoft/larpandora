cet_make_library(LIBRARY_NAME ShowerElements INTERFACE
  SOURCE ShowerElementHolder.hh
  LIBRARIES INTERFACE
  art::Framework_Principal
  canvas::canvas
  messagefacility::MF_MessageLogger
  cetlib_except::cetlib_except
)

cet_make_library(LIBRARY_NAME ShowerProducedPtrs INTERFACE
  SOURCE ShowerProducedPtrsHolder.hh
  LIBRARIES INTERFACE
  larpandora::ShowerElements
  art::Framework_Principal
  art::Persistency_Common
  canvas::canvas
  messagefacility::MF_MessageLogger
  cetlib_except::cetlib_except
)

cet_make_library(SOURCE
  LArPandoraShowerAlg.cxx
  LArPandoraShowerCheatingAlg.cxx
  LIBRARIES
  PUBLIC
  larsim::MCCheater_BackTrackerService_service
  larsim::MCCheater_ParticleInventoryService_service
  larcore::Geometry_Geometry_service
  art_root_io::TFileService_service
  art::Framework_Services_Registry
  canvas::canvas
  ROOT::Physics
  PRIVATE
  larpandora::ShowerElements
  lardataalg::DetectorInfo
  larevt::SpaceChargeService
  larcore::ServiceUtil
  lardataobj::RecoBase
  art::Framework_Principal
  ROOT::Core
  ROOT::Gpad
  ROOT::Graf3d
)

install_headers()
install_source()
install_fhicl()
