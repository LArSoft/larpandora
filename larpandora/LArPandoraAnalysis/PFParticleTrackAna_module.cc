/**
 *  @file   larpandora/LArPandoraAnalysis/PFParticleTrackAna_module.cc
 *
 *  @brief  Analysis module for created particles
 */

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"

#include "TTree.h"

#include <string>

//------------------------------------------------------------------------------------------------------------------------------------------

namespace lar_pandora {

  /**
 *  @brief  PFParticleTrackAna class
 */
  class PFParticleTrackAna : public art::EDAnalyzer {
  public:
    /**
     *  @brief  Constructor
     *
     *  @param  pset
     */
    PFParticleTrackAna(fhicl::ParameterSet const& pset);

    /**
     *  @brief  Destructor
     */
    virtual ~PFParticleTrackAna();

    void beginJob();
    void endJob();
    void analyze(const art::Event& evt);
    void reconfigure(fhicl::ParameterSet const& pset);

  private:
    TTree* m_pCaloTree; ///<

    int m_run;     ///<
    int m_event;   ///<
    int m_index;   ///<
    int m_ntracks; ///<
    int m_trkid;   ///<
    int m_plane;   ///<

    double m_length;        ///<
    double m_dEdx;          ///<
    double m_dNdx;          ///<
    double m_dQdx;          ///<
    double m_residualRange; ///<

    double m_x;  ///<
    double m_y;  ///<
    double m_z;  ///<
    double m_px; ///<
    double m_py; ///<
    double m_pz; ///<

    bool m_useModBox; ///<
    bool m_isCheated; ///<

    std::string m_trackModuleLabel; ///<
  };

  DEFINE_ART_MODULE(PFParticleTrackAna)

} // namespace lar_pandora

//------------------------------------------------------------------------------------------------------------------------------------------
// implementation follows

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "larcore/Geometry/Geometry.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/DetectorInfoServices/LArPropertiesService.h"
#include "lardataobj/RecoBase/Track.h"

#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

#include <iostream>

namespace lar_pandora {

  PFParticleTrackAna::PFParticleTrackAna(fhicl::ParameterSet const& pset) : art::EDAnalyzer(pset)
  {
    this->reconfigure(pset);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  PFParticleTrackAna::~PFParticleTrackAna() {}

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleTrackAna::reconfigure(fhicl::ParameterSet const& pset)
  {
    m_useModBox = pset.get<bool>("UeModBox", true);
    m_isCheated = pset.get<bool>("IsCheated", false);
    m_trackModuleLabel = pset.get<std::string>("TrackModule", "pandora");
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleTrackAna::beginJob()
  {
    //
    art::ServiceHandle<art::TFileService const> tfs;

    m_pCaloTree = tfs->make<TTree>("calorimetry", "LAr Track Calo Tree");
    m_pCaloTree->Branch("run", &m_run, "run/I");
    m_pCaloTree->Branch("event", &m_event, "event/I");
    m_pCaloTree->Branch("index", &m_index, "index/I");
    m_pCaloTree->Branch("ntracks", &m_ntracks, "ntracks/I");
    m_pCaloTree->Branch("trkid", &m_trkid, "trkid/I");
    m_pCaloTree->Branch("plane", &m_plane, "plane/I");
    m_pCaloTree->Branch("length", &m_length, "length/D");
    m_pCaloTree->Branch("dEdx", &m_dEdx, "dEdx/D");
    m_pCaloTree->Branch("dNdx", &m_dNdx, "dNdx/D");
    m_pCaloTree->Branch("dQdx", &m_dQdx, "dQdx/D");
    m_pCaloTree->Branch("residualRange", &m_residualRange, "residualRange/D");
    m_pCaloTree->Branch("x", &m_x, "x/D");
    m_pCaloTree->Branch("y", &m_y, "y/D");
    m_pCaloTree->Branch("z", &m_z, "z/D");
    m_pCaloTree->Branch("px", &m_px, "px/D");
    m_pCaloTree->Branch("py", &m_py, "py/D");
    m_pCaloTree->Branch("pz", &m_pz, "pz/D");
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleTrackAna::endJob() {}

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleTrackAna::analyze(const art::Event& evt)
  {
    std::cout << " *** PFParticleTrackAna::analyze(...) *** " << std::endl;

    m_run = evt.run();
    m_event = evt.id().event();
    m_index = 0;

    m_ntracks = 0;
    m_trkid = 0;
    m_plane = 0;
    m_length = 0.0;
    m_dEdx = 0.0;
    m_dNdx = 0.0;
    m_dQdx = 0.0;
    m_residualRange = 0.0;

    m_x = 0.0;
    m_y = 0.0;
    m_z = 0.0;
    m_px = 0.0;
    m_py = 0.0;
    m_pz = 0.0;

    std::cout << "  Run: " << m_run << std::endl;
    std::cout << "  Event: " << m_event << std::endl;

    TrackVector trackVector;
    TracksToHits tracksToHits;
    LArPandoraHelper::CollectTracks(evt, m_trackModuleLabel, trackVector, tracksToHits);

    std::cout << "  Tracks: " << trackVector.size() << std::endl;

    // art::ServiceHandle<geo::Geometry const> theGeometry;
    // auto const* theDetector = lar::providerFrom<detinfo::DetectorPropertiesService>();

    ///// microboone_calorimetryalgmc.CalAreaConstants: [ 5.0142e-3, 5.1605e-3, 5.4354e-3 ]
    ///// lbne35t_calorimetryalgmc.CalAreaConstants: [ 5.1822e-3, 5.2682e-3, 5.3962e-3 ]

    // const double adc2eU(5.1e-3);
    // const double adc2eV(5.2e-3);
    // const double adc2eW(5.4e-3);
    // const double adc2eCheat(theDetector->ElectronsToADC());

    // const double tau(theDetector->ElectronLifetime());

    m_ntracks = trackVector.size();

    for (TrackVector::const_iterator iter = trackVector.begin(), iterEnd = trackVector.end();
         iter != iterEnd;
         ++iter) {
      const art::Ptr<recob::Track> track = *iter;

      m_trkid = track->ID();
      m_length = track->Length();

      m_plane = 0;
      m_dEdx = 0.0;
      m_dNdx = 0.0;
      m_dQdx = 0.0;
      m_residualRange = 0.0;

      m_x = 0.0;
      m_y = 0.0;
      m_z = 0.0;
      m_px = 0.0;
      m_py = 0.0;
      m_pz = 0.0;

      for (unsigned int p = 0; p < track->NumberTrajectoryPoints(); ++p) {
        auto pos = track->LocationAtPoint(p);
        auto dir = track->DirectionAtPoint(p);

        m_residualRange = track->Length(p);

        m_x = pos.x();
        m_y = pos.y();
        m_z = pos.z();
        m_px = dir.x();
        m_py = dir.y();
        m_pz = dir.z();

        /*************************************************************/
        /*                          WARNING                          */
        /*************************************************************/
        /* The dQdx information in recob::Track has been deprecated  */
        /* since 2016 and in 11/2018 the recob::Track interface was  */
        /* changed and DQdxAtPoint and NumberdQdx were removed.      */
        /* Therefore the code below is now commented out             */
        /* (note that it was most likely not functional anyways).    */
        /* For any issue please contact: larsoft-team@fnal.gov       */
        /*************************************************************/
        /*
            const double dQdxU(track->DQdxAtPoint(p, geo::kU)); // plane 0
            const double dQdxV(track->DQdxAtPoint(p, geo::kV)); // plane 1
            const double dQdxW(track->DQdxAtPoint(p, geo::kW)); // plane 2

            m_plane = ((dQdxU > 0.0) ? geo::kU : (dQdxV > 0.0) ? geo::kV : geo::kW);

            const double adc2e(m_isCheated ? adc2eCheat : (geo::kU == m_plane) ? adc2eU : (geo::kV == m_plane) ? adc2eV : adc2eW);

            m_dQdx = ((geo::kU == m_plane) ? dQdxU : (geo::kV == m_plane) ? dQdxV : dQdxW);

            // TODO: Need to include T0 information (currently assume T0 = 0)

            m_dNdx = ((m_dQdx / adc2e) * exp((m_x / theDetector->GetXTicksCoefficient()) * theDetector->SamplingRate() * 1.e-3 / tau));

            m_dEdx = (m_useModBox ? theDetector->ModBoxCorrection(m_dNdx) : theDetector->BirksCorrection(m_dNdx));
	    */
        /*************************************************************/

        m_pCaloTree->Fill();
        ++m_index;
      }
    }
  }

} //namespace lar_pandora
