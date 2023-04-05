add_subdirectory(Algs)
add_subdirectory(Tools)

cet_build_plugin(LArPandoraModularShowerCreation art::EDProducer
  LIBRARIES PRIVATE
  larpandora::ShowerTool
  lardataobj::RecoBase
  lardata::AssociationUtil
  art_plugin_support::toolMaker
)

cet_build_plugin(LArPandoraShowerCreation art::EDProducer
  LIBRARIES PRIVATE
  larpandora::LArPandoraInterface
  larpandoracontent::LArPandoraContent
  lardata::AssociationUtil
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  art::Framework_Principal
  art::Persistency_Common
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
)

install_fhicl()
install_source()
