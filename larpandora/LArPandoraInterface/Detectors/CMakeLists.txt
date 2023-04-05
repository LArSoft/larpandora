cet_make_library(LIBRARY_NAME_VAR detectors_lib_target
  SOURCE
  LArPandoraDetectorType.h
  DUNEFarDetVDThreeView.h
  ICARUS.h
  ProtoDUNEDualPhase.h
  VintageLArTPCThreeView.h
  LArPandoraDetectorType.cxx
  ProtoDUNEDualPhase.cxx
  LIBRARIES PUBLIC
  larpandora::GeometryComponents
  art::Framework_Services_Registry
  larcore::Geometry_Geometry_service
  larcoreobj::SimpleTypesAndConstants
  larcoreobj::geo_vectors
  PandoraPFA::PandoraSDK
  PRIVATE
  larpandora::GeometryComponents
)

cet_make_library(LIBRARY_NAME GetDetectorType
  SOURCE GetDetectorType.cxx
  LIBRARIES PRIVATE
  ${detectors_lib_target}
  cetlib_except::cetlib_except
)  

install_headers()
install_source()
