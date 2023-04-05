cet_build_plugin(ShowerDirectionCheater larpandora::ShowerTool
  LIBRARIES PRIVATE
  lardata::DetectorClocksService
  lardata::DetectorPropertiesService
  lardataobj::RecoBase
  ROOT::Tree
)

cet_build_plugin(ShowerStartPositionCheater larpandora::ShowerTool
  LIBRARIES PRIVATE
  lardata::DetectorClocksService
  lardata::DetectorPropertiesService
  lardataobj::RecoBase
)

cet_build_plugin(ShowerTrackFinderCheater larpandora::ShowerTool
  LIBRARIES PRIVATE
  lardata::DetectorClocksService
  lardata::DetectorPropertiesService
  lardataobj::RecoBase
)

install_headers()
install_source()
install_fhicl()
