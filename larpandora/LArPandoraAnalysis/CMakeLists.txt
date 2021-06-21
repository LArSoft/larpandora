art_make(

          LIB_LIBRARIES larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        larsim_Simulation lardataobj_Simulation
                        lardataobj_RawData
                        lardataobj_RecoBase
                        lardataobj_AnalysisBase
                        lardata_Utilities
                        nusimdata_SimulationBase
                        art::Framework_Core
                        art::Framework_Principal
                        art::Framework_Services_Registry
                        art::Framework_Services_Registry
                        art::Framework_Services_FileServiceInterfaces
                        art_root_io::tfile_support
                        art_root_io::TFileService_service
                        art_Persistency_Common
                        art_Persistency_Provenance
                        art_Utilities
                        canvas
                        messagefacility::MF_MessageLogger

                        fhiclcpp::fhiclcpp fhiclcpp::types
                        cetlib cetlib_except
                        ROOT::Hist
                        ROOT::Physics
                        ROOT::MathCore
                        ROOT::Tree
                        ROOT::Core
           MODULE_LIBRARIES larpandora_LArPandoraInterface
          )

install_headers()
install_fhicl()
install_source()
