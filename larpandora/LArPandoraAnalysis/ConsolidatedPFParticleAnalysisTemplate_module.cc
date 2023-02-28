/**
 *  @file   larpandora/LArPandoraAnalysis/ConsolidatedPFParticleAnalysisTemplate_module.cc
 *
 *  @brief  A template analysis module for using the Pandora consolidated output
 */

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"

#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Track.h"

#include <string>

//------------------------------------------------------------------------------------------------------------------------------------------

namespace lar_pandora {

  /**
 *  @brief  ConsolidatedPFParticleAnalysisTemplate class
 */
  class ConsolidatedPFParticleAnalysisTemplate : public art::EDAnalyzer {
  public:
    typedef art::Handle<std::vector<recob::PFParticle>> PFParticleHandle;
    typedef std::map<size_t, art::Ptr<recob::PFParticle>> PFParticleIdMap;
    typedef std::vector<art::Ptr<recob::PFParticle>> PFParticleVector;
    typedef std::vector<art::Ptr<recob::Track>> TrackVector;
    typedef std::vector<art::Ptr<recob::Shower>> ShowerVector;

    /**
     *  @brief  Constructor
     *
     *  @param  pset the set of input fhicl parameters
     */
    ConsolidatedPFParticleAnalysisTemplate(fhicl::ParameterSet const& pset);

    /**
     *  @brief  Configure memeber variables using FHiCL parameters
     *
     *  @param  pset the set of input fhicl parameters
     */
    void reconfigure(fhicl::ParameterSet const& pset);

    /**
     *  @brief  Analyze an event!
     *
     *  @param  evt the art event to analyze
     */
    void analyze(const art::Event& evt);

  private:
    /**
     *  @brief  Produce a mapping from PFParticle ID to the art ptr to the PFParticle itself for fast navigation
     *
     *  @param  pfParticleHandle the handle for the PFParticle collection
     *  @param  pfParticleMap the mapping from ID to PFParticle
     */
    void GetPFParticleIdMap(const PFParticleHandle& pfParticleHandle,
                            PFParticleIdMap& pfParticleMap);

    /**
     * @brief Print out scores in PFParticleMetadata
     *
     * @param evt the art event to analyze
     * @param pfParticleHandle the handle for the PFParticle collection
     */
    void PrintOutScores(const art::Event& evt, const PFParticleHandle& pfParticleHandle) const;

    /**
     *  @brief  Produce a mapping from PFParticle ID to the art ptr to the PFParticle itself for fast navigation
     *
     *  @param  pfParticleMap the mapping from ID to PFParticle
     *  @param  crParticles a vector to hold the top-level PFParticles reconstructed under the cosmic hypothesis
     *  @param  nuParticles a vector to hold the final-states of the reconstruced neutrino
     */
    void GetFinalStatePFParticleVectors(const PFParticleIdMap& pfParticleMap,
                                        PFParticleVector& crParticles,
                                        PFParticleVector& nuParticles);

    /**
     *  @brief  Collect associated tracks and showers to particles in an input particle vector
     *
     *  @param  particles a vector holding PFParticles from which to find the associated tracks and showers
     *  @param  pfParticleHandle the handle for the PFParticle collection
     *  @param  evt the art event to analyze
     *  @param  tracks a vector to hold the associated tracks
     *  @param  showers a vector to hold the associated showers
     */
    void CollectTracksAndShowers(const PFParticleVector& particles,
                                 const PFParticleHandle& pfParticleHandle,
                                 const art::Event& evt,
                                 TrackVector& tracks,
                                 ShowerVector& showers);

    std::string m_pandoraLabel; ///< The label for the pandora producer
    std::string m_trackLabel;   ///< The label for the track producer from PFParticles
    std::string m_showerLabel;  ///< The label for the shower producer from PFParticles
    bool m_printOutScores; ///< Option to investigate the associations to scores for PFParticles
  };

  DEFINE_ART_MODULE(ConsolidatedPFParticleAnalysisTemplate)

} // namespace lar_pandora

//------------------------------------------------------------------------------------------------------------------------------------------
// implementation follows

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardataobj/RecoBase/PFParticleMetadata.h"

#include "Pandora/PdgTable.h"

#include <iostream>

namespace lar_pandora {

  ConsolidatedPFParticleAnalysisTemplate::ConsolidatedPFParticleAnalysisTemplate(
    fhicl::ParameterSet const& pset)
    : art::EDAnalyzer(pset)
  {
    this->reconfigure(pset);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void ConsolidatedPFParticleAnalysisTemplate::reconfigure(fhicl::ParameterSet const& pset)
  {
    m_pandoraLabel = pset.get<std::string>("PandoraLabel");
    m_trackLabel = pset.get<std::string>("TrackLabel");
    m_showerLabel = pset.get<std::string>("ShowerLabel");
    m_printOutScores = pset.get<bool>("PrintOutScores", true);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void ConsolidatedPFParticleAnalysisTemplate::analyze(const art::Event& evt)
  {
    // Collect the PFParticles from the event
    PFParticleHandle pfParticleHandle;
    evt.getByLabel(m_pandoraLabel, pfParticleHandle);

    if (!pfParticleHandle.isValid()) {
      mf::LogDebug("ConsolidatedPFParticleAnalysisTemplate")
        << "  Failed to find the PFParticles." << std::endl;
      return;
    }

    // Produce a map of the PFParticle IDs for fast navigation through the hierarchy
    PFParticleIdMap pfParticleMap;
    this->GetPFParticleIdMap(pfParticleHandle, pfParticleMap);

    /// Investigate scores associated as larpandoraobject::metadata for the PFParticles
    if (m_printOutScores) this->PrintOutScores(evt, pfParticleHandle);

    // Produce two PFParticle vectors containing final-state particles:
    // 1. Particles identified as cosmic-rays - recontructed under cosmic-hypothesis
    // 2. Daughters of the neutrino PFParticle - reconstructed under the neutrino hypothesis
    std::vector<art::Ptr<recob::PFParticle>> crParticles;
    std::vector<art::Ptr<recob::PFParticle>> nuParticles;
    this->GetFinalStatePFParticleVectors(pfParticleMap, crParticles, nuParticles);

    // Use as required!
    // -----------------------------
    //   What follows is an example showing how one might access the reconstructed neutrino final-state tracks and showers

    // These are the vectors to hold the tracks and showers for the final-states of the reconstructed neutrino
    std::vector<art::Ptr<recob::Track>> tracks;
    std::vector<art::Ptr<recob::Shower>> showers;
    this->CollectTracksAndShowers(nuParticles, pfParticleHandle, evt, tracks, showers);

    // Print a summary of the consolidated event
    std::cout << "Consolidated event summary:" << std::endl;
    std::cout << "  - Number of primary cosmic-ray PFParticles   : " << crParticles.size()
              << std::endl;
    std::cout << "  - Number of neutrino final-state PFParticles : " << nuParticles.size()
              << std::endl;
    std::cout << "    ... of which are track-like   : " << tracks.size() << std::endl;
    std::cout << "    ... of which are showers-like : " << showers.size() << std::endl;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void ConsolidatedPFParticleAnalysisTemplate::GetPFParticleIdMap(
    const PFParticleHandle& pfParticleHandle,
    PFParticleIdMap& pfParticleMap)
  {
    for (unsigned int i = 0; i < pfParticleHandle->size(); ++i) {
      const art::Ptr<recob::PFParticle> pParticle(pfParticleHandle, i);
      if (!pfParticleMap.insert(PFParticleIdMap::value_type(pParticle->Self(), pParticle)).second) {
        throw cet::exception("ConsolidatedPFParticleAnalysisTemplate")
          << "  Unable to get PFParticle ID map, the input PFParticle collection has repeat IDs!";
      }
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void ConsolidatedPFParticleAnalysisTemplate::PrintOutScores(
    const art::Event& evt,
    const PFParticleHandle& pfParticleHandle) const
  {
    // Get the associations between PFParticles and larpandoraobj::PFParticleMetadata
    art::FindManyP<larpandoraobj::PFParticleMetadata> pfPartToMetadataAssoc(
      pfParticleHandle, evt, m_pandoraLabel);

    for (unsigned int i = 0; i < pfParticleHandle->size(); ++i) {
      const std::vector<art::Ptr<larpandoraobj::PFParticleMetadata>>& pfParticleMetadataList(
        pfPartToMetadataAssoc.at(i));
      if (!pfParticleMetadataList.empty()) {
        const art::Ptr<recob::PFParticle> pParticle(pfParticleHandle, i);
        for (unsigned int j = 0; j < pfParticleMetadataList.size(); ++j) {
          const art::Ptr<larpandoraobj::PFParticleMetadata>& pfParticleMetadata(
            pfParticleMetadataList.at(j));
          const larpandoraobj::PFParticleMetadata::PropertiesMap& pfParticlePropertiesMap(
            pfParticleMetadata->GetPropertiesMap());
          if (!pfParticlePropertiesMap.empty())
            std::cout << " Found PFParticle " << pParticle->Self() << " with: " << std::endl;
          for (larpandoraobj::PFParticleMetadata::PropertiesMap::const_iterator it =
                 pfParticlePropertiesMap.begin();
               it != pfParticlePropertiesMap.end();
               ++it)
            std::cout << "  - " << it->first << " = " << it->second << std::endl;
        }
      }
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void ConsolidatedPFParticleAnalysisTemplate::GetFinalStatePFParticleVectors(
    const PFParticleIdMap& pfParticleMap,
    PFParticleVector& crParticles,
    PFParticleVector& nuParticles)
  {
    for (PFParticleIdMap::const_iterator it = pfParticleMap.begin(); it != pfParticleMap.end();
         ++it) {
      const art::Ptr<recob::PFParticle> pParticle(it->second);

      // Only look for primary particles
      if (!pParticle->IsPrimary()) continue;

      // Check if this particle is identified as the neutrino
      const int pdg(pParticle->PdgCode());
      const bool isNeutrino(std::abs(pdg) == pandora::NU_E || std::abs(pdg) == pandora::NU_MU ||
                            std::abs(pdg) == pandora::NU_TAU);

      // All non-neutrino primary particles are reconstructed under the cosmic hypothesis
      if (!isNeutrino) {
        crParticles.push_back(pParticle);
        continue;
      }

      // ATTN. We are filling nuParticles under the assumption that there is only one reconstructed neutrino identified per event.
      //       If this is not the case please handle accordingly
      if (!nuParticles.empty()) {
        throw cet::exception("ConsolidatedPFParticleAnalysisTemplate")
          << "  This event contains multiple reconstructed neutrinos!";
      }

      // Add the daughters of the neutrino PFParticle to the nuPFParticles vector
      for (const size_t daughterId : pParticle->Daughters()) {
        if (pfParticleMap.find(daughterId) == pfParticleMap.end())
          throw cet::exception("ConsolidatedPFParticleAnalysisTemplate")
            << "  Invalid PFParticle collection!";

        nuParticles.push_back(pfParticleMap.at(daughterId));
      }
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void ConsolidatedPFParticleAnalysisTemplate::CollectTracksAndShowers(
    const PFParticleVector& particles,
    const PFParticleHandle& pfParticleHandle,
    const art::Event& evt,
    TrackVector& tracks,
    ShowerVector& showers)
  {
    // Get the associations between PFParticles and tracks/showers from the event
    art::FindManyP<recob::Track> pfPartToTrackAssoc(pfParticleHandle, evt, m_trackLabel);
    art::FindManyP<recob::Shower> pfPartToShowerAssoc(pfParticleHandle, evt, m_showerLabel);

    for (const art::Ptr<recob::PFParticle>& pParticle : particles) {
      const std::vector<art::Ptr<recob::Track>> associatedTracks(
        pfPartToTrackAssoc.at(pParticle.key()));
      const std::vector<art::Ptr<recob::Shower>> associatedShowers(
        pfPartToShowerAssoc.at(pParticle.key()));
      const unsigned int nTracks(associatedTracks.size());
      const unsigned int nShowers(associatedShowers.size());

      // Check if the PFParticle has no associated tracks or showers
      if (nTracks == 0 && nShowers == 0) {
        mf::LogDebug("ConsolidatedPFParticleAnalysisTemplate")
          << "  No tracks or showers were associated to PFParticle " << pParticle->Self()
          << std::endl;
        continue;
      }

      // Check if there is an associated track
      if (nTracks == 1 && nShowers == 0) {
        tracks.push_back(associatedTracks.front());
        continue;
      }

      // Check if there is an associated shower
      if (nTracks == 0 && nShowers == 1) {
        showers.push_back(associatedShowers.front());
        continue;
      }

      throw cet::exception("ConsolidatedPFParticleAnalysisTemplate")
        << "  There were " << nTracks << " tracks and " << nShowers
        << " showers associated with PFParticle " << pParticle->Self();
    }
  }

} //namespace lar_pandora
