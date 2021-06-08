add_subdirectory(Tools)
add_subdirectory(Algs)

art_make(
  MODULE_LIBRARIES
  larpandora_LArPandoraEventBuilding
  larpandora_LArPandoraEventBuilding_LArPandoraShower_Algs
  )


install_fhicl()
install_source()
