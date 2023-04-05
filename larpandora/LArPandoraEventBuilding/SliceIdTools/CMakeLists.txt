include(larpandora::SliceIdTool)

cet_build_plugin(SimpleBeamParticleId larpandora::SliceIdTool
  LIBRARIES PRIVATE
  larpandora::Slice
  fhiclcpp::fhiclcpp
)

cet_build_plugin(SimpleNeutrinoId larpandora::SliceIdTool
  LIBRARIES PRIVATE
  larpandora::Slice
  fhiclcpp::fhiclcpp
)

install_headers()
install_fhicl()
install_source()
