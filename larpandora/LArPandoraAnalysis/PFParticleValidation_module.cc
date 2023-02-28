/**
 *  @file   larpandora/LArPandoraAnalysis/PFParticleValidation_module.cc
 *
 *  @brief  Analysis module for created particles
 *
 */

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"

#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

#include <string>

//------------------------------------------------------------------------------------------------------------------------------------------

namespace lar_pandora {

  /**
 *  @brief  PFParticleValidation class
 */
  class PFParticleValidation : public art::EDAnalyzer {
  public:
    /**
     *  @brief  Constructor
     *
     *  @param  pset
     */
    PFParticleValidation(fhicl::ParameterSet const& pset);

    /**
     *  @brief  Destructor
     */
    virtual ~PFParticleValidation();

    void beginJob();
    void endJob();
    void analyze(const art::Event& evt);
    void reconfigure(fhicl::ParameterSet const& pset);

  private:
    /**
     *  @brief SimpleMCPrimary class
     */
    class SimpleMCPrimary {
    public:
      /**
         *  @brief  Constructor
         */
      SimpleMCPrimary();

      /**
         *  @brief  operator <
         *
         *  @param  rhs object for comparison
         *
         *  @return boolean
         */
      bool operator<(const SimpleMCPrimary& rhs) const;

      int m_id;                           ///< The unique identifier
      int m_pdgCode;                      ///< The pdg code
      int m_nMCHitsTotal;                 ///< The total number of mc hits
      int m_nMCHitsU;                     ///< The number of u mc hits
      int m_nMCHitsV;                     ///< The number of v mc hits
      int m_nMCHitsW;                     ///< The number of w mc hits
      float m_energy;                     ///< The energy
      int m_nMatchedPfos;                 ///< The number of matched pfos
      const simb::MCParticle* m_pAddress; ///< The address of the mc primary
    };

    typedef std::vector<SimpleMCPrimary> SimpleMCPrimaryList;

    /**
     *  @brief SimpleMatchedPfo class
     */
    class SimpleMatchedPfo {
    public:
      /**
         *  @brief  Constructor
         */
      SimpleMatchedPfo();

      int m_id;                ///< The unique identifier
      int m_parentId;          ///< The unique identifier of the parent pfo (-1 if no parent set)
      int m_pdgCode;           ///< The pdg code
      int m_nPfoHitsTotal;     ///< The total number of pfo hits
      int m_nPfoHitsU;         ///< The number of u pfo hits
      int m_nPfoHitsV;         ///< The number of v pfo hits
      int m_nPfoHitsW;         ///< The number of w pfo hits
      int m_nMatchedHitsTotal; ///< The total number of matched hits
      int m_nMatchedHitsU;     ///< The number of u matched hits
      int m_nMatchedHitsV;     ///< The number of v matched hits
      int m_nMatchedHitsW;     ///< The number of w matched hits
      const recob::PFParticle* m_pAddress; ///< The address of the pf primary
    };

    typedef std::vector<SimpleMatchedPfo> SimpleMatchedPfoList;

    /**
     * @brief   MatchingDetails class
     */
    class MatchingDetails {
    public:
      /**
         *  @brief  Default constructor
         */
      MatchingDetails();

      int m_matchedPrimaryId; ///< The total number of occurences
      int m_nMatchedHits;     ///< The number of times the primary has 0 pfo matches
      float m_completeness;   ///< The completeness of the match
    };

    typedef std::map<int, MatchingDetails> MatchingDetailsMap;
    typedef std::map<SimpleMCPrimary, SimpleMatchedPfoList>
      MCPrimaryMatchingMap; // SimpleMCPrimary has a defined operator<

    typedef std::map<art::Ptr<recob::PFParticle>, HitVector> PFParticleToMatchedHits;
    typedef std::map<art::Ptr<simb::MCParticle>, PFParticleToMatchedHits> MCParticleMatchingMap;

    /**
     *  @brief  Performing matching between true and reconstructed particles
     *
     *  @param  recoParticlesToHits the mapping from reconstructed particles to hits
     *  @param  trueParticlesToHits the mapping from true particles to hits
     *  @param  hitsToTrueParticles the mapping from hits to true particles
     *  @param  mcParticleMatchingMap the output matches between all reconstructed and true particles
     */
    void GetMCParticleMatchingMap(const PFParticlesToHits& recoParticlesToHits,
                                  const MCParticlesToHits& trueParticlesToHits,
                                  const HitsToMCParticles& hitsToTrueParticles,
                                  MCParticleMatchingMap& mcParticleMatchingMap) const;

    /**
     *  @brief  Extract details of each mc primary (ordered by number of true hits)
     *
     *  @param  evt the event
     *  @param  mcParticlesToHits the mc primary to hits map
     *  @param  hitsToMCParticles the hits to mc particles map
     *  @param  mcParticleMatchingMap the mc to particle to pf particle matching map (to record number of matched pf particles)
     *  @param  simpleMCPrimaryList to receive the populated simple mc primary list
     */
    void GetSimpleMCPrimaryList(const art::Event& evt,
                                const MCParticlesToHits& mcParticlesToHits,
                                const HitsToMCParticles& hitsToMCParticles,
                                const MCParticleMatchingMap& mcParticleMatchingMap,
                                SimpleMCPrimaryList& simpleMCPrimaryList) const;

    /**
     *  @brief  Obtain a sorted list of matched pfos for each mc primary
     *
     *  @param  simpleMCPrimaryList the simple mc primary list
     *  @param  mcToFullPfoMatchingMap the mc to full pfo matching map
     *  @param  pfoToHitListMap the pfo to hit list map
     *  @param  mcPrimaryMatchingMap to receive the populated mc primary matching map
     */
    void GetMCPrimaryMatchingMap(const SimpleMCPrimaryList& simpleMCPrimaryList,
                                 const MCParticleMatchingMap& mcParticleMatchingMap,
                                 const PFParticlesToHits& pfParticlesToHits,
                                 MCPrimaryMatchingMap& mcPrimaryMatchingMap) const;

    /**
     *  @brief  Whether a mc particle is neutrino induced
     *
     *  @param  pMCParticle address of the mc particle
     *  @param  artMCParticlesToMCTruth the mapping from mc particles to mc truth
     *
     *  @return boolean
     */
    bool IsNeutrinoInduced(const art::Ptr<simb::MCParticle> pMCParticle,
                           const MCParticlesToMCTruth& artMCParticlesToMCTruth) const;

    /**
     *  @brief  Obtain a vector of mc truth
     *
     *  @param  evt the event
     *  @param  mcNeutrinoVector to receive the populated vector of mc truth
     */
    void GetMCTruth(const art::Event& evt, MCTruthVector& mcTruthVector) const;

    /**
     *  @brief  Obtain a vector of reco neutrinos
     *
     *  @param  evt the event
     *  @param  recoNeutrinoVector to receive the populated vector of reco neutrinos
     */
    void GetRecoNeutrinos(const art::Event& evt, PFParticleVector& recoNeutrinoVector) const;

    /**
     *  @brief  Print all the raw matching output to screen
     *
     *  @param  mcTruthVector the mc truth vector
     *  @param  recoNeutrinoVector the reco neutrino vector
     *  @param  mcPrimaryMatchingMap the input/raw mc primary matching map
     */
    void PrintAllOutput(const MCTruthVector& mcTruthVector,
                        const PFParticleVector& recoNeutrinoVector,
                        const MCPrimaryMatchingMap& mcPrimaryMatchingMap) const;

    /**
     *  @brief  Apply a well-defined matching procedure to the comprehensive matches in the provided mc primary matching map
     *
     *  @param  mcPrimaryMatchingMap the input/raw mc primary matching map
     *  @param  matchingDetailsMap the matching details map, to be populated
     */
    void PerformMatching(const MCPrimaryMatchingMap& mcPrimaryMatchingMap,
                         MatchingDetailsMap& matchingDetailsMap) const;

    typedef std::set<int> IntSet;

    /**
     *  @brief  Get the strongest pfo match (most matched hits) between an available mc primary and an available pfo
     *
     *  @param  mcPrimaryMatchingMap the input/raw mc primary matching map
     *  @param  usedMCIds the list of mc primary ids with an existing match
     *  @param  usedPfoIds the list of pfo ids with an existing match
     *  @param  matchingDetailsMap the matching details map, to be populated
     */
    bool GetStrongestPfoMatch(const MCPrimaryMatchingMap& mcPrimaryMatchingMap,
                              IntSet& usedMCIds,
                              IntSet& usedPfoIds,
                              MatchingDetailsMap& matchingDetailsMap) const;

    /**
     *  @brief  Get the best matches for any pfos left-over after the strong matching procedure
     *
     *  @param  mcPrimaryMatchingMap the input/raw mc primary matching map
     *  @param  usedPfoIds the list of pfo ids with an existing match
     *  @param  matchingDetailsMap the matching details map, to be populated
     */
    void GetRemainingPfoMatches(const MCPrimaryMatchingMap& mcPrimaryMatchingMap,
                                const IntSet& usedPfoIds,
                                MatchingDetailsMap& matchingDetailsMap) const;

    /**
     *  @brief  Print the results of the matching procedure
     *
     *  @param  mcPrimaryMatchingMap the input/raw mc primary matching map
     *  @param  matchingDetailsMap the matching details map
     */
    void PrintMatchingOutput(const MCPrimaryMatchingMap& mcPrimaryMatchingMap,
                             const MatchingDetailsMap& matchingDetailsMap) const;

    /**
     *  @brief  Whether a provided mc primary passes selection, based on number of "good" hits
     *
     *  @param  simpleMCPrimary the simple mc primary
     *
     *  @return boolean
     */
    bool IsGoodMCPrimary(const SimpleMCPrimary& simpleMCPrimary) const;

    /**
     *  @brief  Whether a provided mc primary has a match, of any quality (use simple matched pfo list and information in matching details map)
     *
     *  @param  simpleMCPrimary the simple mc primary
     *  @param  simpleMatchedPfoList the list of simple matched pfos
     *  @param  matchingDetailsMap the matching details map
     *
     *  @return boolean
     */
    bool HasMatch(const SimpleMCPrimary& simpleMCPrimary,
                  const SimpleMatchedPfoList& simpleMatchedPfoList,
                  const MatchingDetailsMap& matchingDetailsMap) const;

    /**
     *  @brief  Whether a provided mc primary and pfo are deemed to be a good match
     *
     *  @param  simpleMCPrimary the simple mc primary
     *  @param  simpleMatchedPfo the simple matched pfo
     *
     *  @return boolean
     */
    bool IsGoodMatch(const SimpleMCPrimary& simpleMCPrimary,
                     const SimpleMatchedPfo& simpleMatchedPfo) const;

    /**
     *  @brief  Count the number of hits, in a provided vector, of a specified view
     *
     *  @param  view the view
     *  @param  hitVector the hit vector
     *
     *  @return the number of hits of the specified view
     */
    unsigned int CountHitsByType(const geo::View_t view, const HitVector& hitVector) const;

    /**
     *  @brief  Sort simple mc primaries by number of mc hits
     *
     *  @param  lhs the left-hand side
     *  @param  rhs the right-hand side
     *
     *  @return boolean
     */
    static bool SortSimpleMCPrimaries(const SimpleMCPrimary& lhs, const SimpleMCPrimary& rhs);

    /**
     *  @brief  Sort simple matched pfos by number of matched hits
     *
     *  @param  lhs the left-hand side
     *  @param  rhs the right-hand side
     *
     *  @return boolean
     */
    static bool SortSimpleMatchedPfos(const SimpleMatchedPfo& lhs, const SimpleMatchedPfo& rhs);

    std::string m_hitfinderLabel;   ///< The name/label of the hit producer module
    std::string m_particleLabel;    ///< The name/label of the particle producer module
    std::string m_geantModuleLabel; ///< The name/label of the geant module
    std::string m_backtrackerLabel; ///< The name/label of the back-tracker module

    bool m_printAllToScreen;      ///< Whether to print all/raw matching details to screen
    bool m_printMatchingToScreen; ///< Whether to print matching output to screen

    bool
      m_neutrinoInducedOnly; ///< Whether to consider only mc particles that were neutrino induced

    int
      m_matchingMinPrimaryHits; ///< The minimum number of good mc primary hits used in matching scheme
    int
      m_matchingMinHitsForGoodView; ///< The minimum number of good mc primary hits in given view to declare view to be good
    int m_matchingMinPrimaryGoodViews; ///< The minimum number of good views for a mc primary

    bool
      m_useSmallPrimaries; ///< Whether to consider matches to mc primaries with fewer than m_matchingMinPrimaryHits
    int m_matchingMinSharedHits;     ///< The minimum number of shared hits used in matching scheme
    float m_matchingMinCompleteness; ///< The minimum particle completeness to declare a match
    float m_matchingMinPurity;       ///< The minimum particle purity to declare a match
  };

  DEFINE_ART_MODULE(PFParticleValidation)

} // namespace lar_pandora

//------------------------------------------------------------------------------------------------------------------------------------------

#include "art/Framework/Principal/Event.h"

#include "fhiclcpp/ParameterSet.h"

#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"

#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/PFParticle.h"

#include <algorithm>
#include <iostream>

namespace lar_pandora {

  PFParticleValidation::PFParticleValidation(fhicl::ParameterSet const& pset)
    : art::EDAnalyzer(pset)
  {
    this->reconfigure(pset);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  PFParticleValidation::~PFParticleValidation() {}

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::reconfigure(fhicl::ParameterSet const& pset)
  {
    m_particleLabel = pset.get<std::string>("PFParticleModule", "pandoraNu");
    m_hitfinderLabel = pset.get<std::string>("HitFinderModule", "gaushit");
    m_geantModuleLabel = pset.get<std::string>("GeantModule", "largeant");
    m_backtrackerLabel = pset.get<std::string>("BackTrackerModule", "gaushitTruthMatch");
    m_printAllToScreen = pset.get<bool>("PrintAllToScreen", true);
    m_printMatchingToScreen = pset.get<bool>("PrintMatchingToScreen", true);
    m_neutrinoInducedOnly = pset.get<bool>("NeutrinoInducedOnly", true);
    m_matchingMinPrimaryHits = pset.get<int>("MatchingMinPrimaryHits", 15);
    m_matchingMinHitsForGoodView = pset.get<int>("MatchingMinHitsForGoodView", 5);
    m_matchingMinPrimaryGoodViews = pset.get<int>("MatchingMinPrimaryGoodViews", 2);
    m_useSmallPrimaries = pset.get<bool>("UseSmallPrimaries", true);
    m_matchingMinSharedHits = pset.get<int>("MatchingMinSharedHits", 5);
    m_matchingMinCompleteness = pset.get<float>("MatchingMinCompleteness", 0.1f);
    m_matchingMinPurity = pset.get<float>("MatchingMinPurity", 0.5f);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::beginJob() {}

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::endJob() {}

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::analyze(const art::Event& evt)
  {
    HitVector hitVector;
    LArPandoraHelper::CollectHits(evt, m_hitfinderLabel, hitVector);

    PFParticlesToHits pfParticlesToHits;
    HitsToPFParticles hitsToPfParticles;
    LArPandoraHelper::BuildPFParticleHitMaps(
      evt, m_particleLabel, pfParticlesToHits, hitsToPfParticles, LArPandoraHelper::kAddDaughters);

    MCParticlesToHits mcParticlesToHits;
    HitsToMCParticles hitsToMCParticles;

    LArPandoraHelper::BuildMCParticleHitMaps(evt,
                                             m_geantModuleLabel,
                                             hitVector,
                                             mcParticlesToHits,
                                             hitsToMCParticles,
                                             LArPandoraHelper::kAddDaughters);

    if (hitsToMCParticles.empty()) {
      if (m_backtrackerLabel.empty())
        throw cet::exception("LArPandora") << " PFParticleValidation::analyze - no sim channels "
                                              "found, backtracker module must be set in FHiCL "
                                           << std::endl;

      LArPandoraHelper::BuildMCParticleHitMaps(evt,
                                               m_geantModuleLabel,
                                               m_hitfinderLabel,
                                               m_backtrackerLabel,
                                               mcParticlesToHits,
                                               hitsToMCParticles,
                                               LArPandoraHelper::kAddDaughters);
    }

    MCParticleMatchingMap mcParticleMatchingMap;
    this->GetMCParticleMatchingMap(
      pfParticlesToHits, mcParticlesToHits, hitsToMCParticles, mcParticleMatchingMap);

    SimpleMCPrimaryList simpleMCPrimaryList;
    this->GetSimpleMCPrimaryList(
      evt, mcParticlesToHits, hitsToMCParticles, mcParticleMatchingMap, simpleMCPrimaryList);

    MCPrimaryMatchingMap mcPrimaryMatchingMap;
    this->GetMCPrimaryMatchingMap(
      simpleMCPrimaryList, mcParticleMatchingMap, pfParticlesToHits, mcPrimaryMatchingMap);

    MCTruthVector mcTruthVector;
    this->GetMCTruth(evt, mcTruthVector);

    PFParticleVector recoNeutrinoVector;
    this->GetRecoNeutrinos(evt, recoNeutrinoVector);

    if (m_printAllToScreen)
      this->PrintAllOutput(mcTruthVector, recoNeutrinoVector, mcPrimaryMatchingMap);

    if (m_printMatchingToScreen) {
      MatchingDetailsMap matchingDetailsMap;
      this->PerformMatching(mcPrimaryMatchingMap, matchingDetailsMap);
      this->PrintMatchingOutput(mcPrimaryMatchingMap, matchingDetailsMap);
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::GetMCParticleMatchingMap(
    const PFParticlesToHits& pfParticlesToHits,
    const MCParticlesToHits& mcParticlesToHits,
    const HitsToMCParticles& hitsToMCParticles,
    MCParticleMatchingMap& mcParticleMatchingMap) const
  {
    // Create a placeholder entry for all mc particles with >0 hits
    for (const MCParticlesToHits::value_type& mcParticleToHitsEntry : mcParticlesToHits) {
      if (!mcParticleToHitsEntry.second.empty())
        (void)mcParticleMatchingMap.insert(MCParticleMatchingMap::value_type(
          mcParticleToHitsEntry.first, PFParticleToMatchedHits()));
    }

    // Store true to reco matching details
    for (const PFParticlesToHits::value_type& recoParticleToHits : pfParticlesToHits) {
      const art::Ptr<recob::PFParticle> pRecoParticle(recoParticleToHits.first);
      const HitVector& hitVector(recoParticleToHits.second);

      for (const art::Ptr<recob::Hit> pHit : hitVector) {
        HitsToMCParticles::const_iterator mcParticleIter = hitsToMCParticles.find(pHit);

        if (hitsToMCParticles.end() == mcParticleIter) continue;

        const art::Ptr<simb::MCParticle> pTrueParticle = mcParticleIter->second;
        mcParticleMatchingMap[pTrueParticle][pRecoParticle].push_back(pHit);
      }
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::GetSimpleMCPrimaryList(
    const art::Event& evt,
    const MCParticlesToHits& mcParticlesToHits,
    const HitsToMCParticles& hitsToMCParticles,
    const MCParticleMatchingMap& mcParticleMatchingMap,
    SimpleMCPrimaryList& simpleMCPrimaryList) const
  {
    MCTruthToMCParticles artMCTruthToMCParticles;
    MCParticlesToMCTruth artMCParticlesToMCTruth;
    LArPandoraHelper::CollectMCParticles(
      evt, m_geantModuleLabel, artMCTruthToMCParticles, artMCParticlesToMCTruth);

    for (const MCParticlesToHits::value_type& mapEntry : mcParticlesToHits) {
      const art::Ptr<simb::MCParticle> pMCPrimary(mapEntry.first);

      if (m_neutrinoInducedOnly && !this->IsNeutrinoInduced(pMCPrimary, artMCParticlesToMCTruth))
        continue;

      SimpleMCPrimary simpleMCPrimary;
      // ATTN simpleMCPrimary.m_id assigned later, after sorting
      simpleMCPrimary.m_pAddress = pMCPrimary.get();
      simpleMCPrimary.m_pdgCode = pMCPrimary->PdgCode();
      simpleMCPrimary.m_energy = pMCPrimary->E();

      MCParticlesToHits::const_iterator trueHitsIter = mcParticlesToHits.find(pMCPrimary);

      if (mcParticlesToHits.end() != trueHitsIter) {
        const HitVector& hitVector(trueHitsIter->second);
        simpleMCPrimary.m_nMCHitsTotal = hitVector.size();
        simpleMCPrimary.m_nMCHitsU = this->CountHitsByType(geo::kU, hitVector);
        simpleMCPrimary.m_nMCHitsV = this->CountHitsByType(geo::kV, hitVector);
        simpleMCPrimary.m_nMCHitsW = this->CountHitsByType(geo::kW, hitVector);
      }

      MCParticleMatchingMap::const_iterator matchedPfoIter = mcParticleMatchingMap.find(pMCPrimary);

      if (mcParticleMatchingMap.end() != matchedPfoIter)
        simpleMCPrimary.m_nMatchedPfos = matchedPfoIter->second.size();

      simpleMCPrimaryList.push_back(simpleMCPrimary);
    }

    std::sort(simpleMCPrimaryList.begin(),
              simpleMCPrimaryList.end(),
              PFParticleValidation::SortSimpleMCPrimaries);

    int mcPrimaryId(0);
    for (SimpleMCPrimary& simpleMCPrimary : simpleMCPrimaryList)
      simpleMCPrimary.m_id = mcPrimaryId++;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool PFParticleValidation::IsNeutrinoInduced(
    const art::Ptr<simb::MCParticle> pMCParticle,
    const MCParticlesToMCTruth& artMCParticlesToMCTruth) const
  {
    MCParticlesToMCTruth::const_iterator iter = artMCParticlesToMCTruth.find(pMCParticle);

    if (artMCParticlesToMCTruth.end() == iter) return false;

    const art::Ptr<simb::MCTruth> pMCTruth = iter->second;
    return (simb::kBeamNeutrino == pMCTruth->Origin());
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::GetMCPrimaryMatchingMap(
    const SimpleMCPrimaryList& simpleMCPrimaryList,
    const MCParticleMatchingMap& mcParticleMatchingMap,
    const PFParticlesToHits& pfParticlesToHits,
    MCPrimaryMatchingMap& mcPrimaryMatchingMap) const
  {
    for (const SimpleMCPrimary& simpleMCPrimary : simpleMCPrimaryList) {
      SimpleMatchedPfoList simpleMatchedPfoList;
      MCParticleMatchingMap::const_iterator matchedPfoIter = mcParticleMatchingMap.end();

      // ATTN Nasty workaround I
      for (MCParticleMatchingMap::const_iterator iter = mcParticleMatchingMap.begin(),
                                                 iterEnd = mcParticleMatchingMap.end();
           iter != iterEnd;
           ++iter) {
        if (simpleMCPrimary.m_pAddress == iter->first.get()) {
          matchedPfoIter = iter;
          break;
        };
      }

      if (mcParticleMatchingMap.end() != matchedPfoIter) {
        for (const PFParticleToMatchedHits::value_type& contribution : matchedPfoIter->second) {
          const art::Ptr<recob::PFParticle> pMatchedPfo(contribution.first);
          const HitVector& matchedHitVector(contribution.second);

          SimpleMatchedPfo simpleMatchedPfo;
          simpleMatchedPfo.m_pAddress = pMatchedPfo.get();
          simpleMatchedPfo.m_id = pMatchedPfo->Self();

          // ATTN Assume pfos have either zero or one parents. Ignore parent neutrino.
          PFParticlesToHits::const_iterator parentPfoIter = pfParticlesToHits.end();

          // ATTN Nasty workaround II, bad place for another loop.
          for (PFParticlesToHits::const_iterator iter = pfParticlesToHits.begin(),
                                                 iterEnd = pfParticlesToHits.end();
               iter != iterEnd;
               ++iter) {
            if (pMatchedPfo->Parent() == iter->first->Self()) {
              parentPfoIter = iter;
              break;
            };
          }

          if ((pfParticlesToHits.end() != parentPfoIter) &&
              !LArPandoraHelper::IsNeutrino(parentPfoIter->first))
            simpleMatchedPfo.m_parentId = parentPfoIter->first->Self();

          simpleMatchedPfo.m_pdgCode = pMatchedPfo->PdgCode();
          simpleMatchedPfo.m_nMatchedHitsTotal = matchedHitVector.size();
          simpleMatchedPfo.m_nMatchedHitsU = this->CountHitsByType(geo::kU, matchedHitVector);
          simpleMatchedPfo.m_nMatchedHitsV = this->CountHitsByType(geo::kV, matchedHitVector);
          simpleMatchedPfo.m_nMatchedHitsW = this->CountHitsByType(geo::kW, matchedHitVector);

          PFParticlesToHits::const_iterator pfoHitsIter = pfParticlesToHits.find(pMatchedPfo);

          if (pfParticlesToHits.end() == pfoHitsIter)
            throw cet::exception("LArPandora")
              << " PFParticleValidation::analyze --- Presence of PFParticle in map mandatory.";

          const HitVector& pfoHitVector(pfoHitsIter->second);

          simpleMatchedPfo.m_nPfoHitsTotal = pfoHitVector.size();
          simpleMatchedPfo.m_nPfoHitsU = this->CountHitsByType(geo::kU, pfoHitVector);
          simpleMatchedPfo.m_nPfoHitsV = this->CountHitsByType(geo::kV, pfoHitVector);
          simpleMatchedPfo.m_nPfoHitsW = this->CountHitsByType(geo::kW, pfoHitVector);

          simpleMatchedPfoList.push_back(simpleMatchedPfo);
        }
      }

      // Store the ordered vectors of matched pfo details
      std::sort(simpleMatchedPfoList.begin(),
                simpleMatchedPfoList.end(),
                PFParticleValidation::SortSimpleMatchedPfos);

      if (!mcPrimaryMatchingMap
             .insert(MCPrimaryMatchingMap::value_type(simpleMCPrimary, simpleMatchedPfoList))
             .second)
        throw cet::exception("LArPandora")
          << " PFParticleValidation::analyze --- Double-counting MC primaries.";
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::GetMCTruth(const art::Event& evt, MCTruthVector& mcTruthVector) const
  {
    MCTruthToMCParticles artMCTruthToMCParticles;
    MCParticlesToMCTruth artMCParticlesToMCTruth;
    LArPandoraHelper::CollectMCParticles(
      evt, m_geantModuleLabel, artMCTruthToMCParticles, artMCParticlesToMCTruth);

    for (const auto& mapEntry : artMCTruthToMCParticles) {
      const art::Ptr<simb::MCTruth> truth = mapEntry.first;

      if (!truth->NeutrinoSet()) continue;

      if (mcTruthVector.end() == std::find(mcTruthVector.begin(), mcTruthVector.end(), truth))
        mcTruthVector.push_back(truth);
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::GetRecoNeutrinos(const art::Event& evt,
                                              PFParticleVector& recoNeutrinoVector) const
  {
    PFParticleVector allPFParticles;
    LArPandoraHelper::CollectPFParticles(evt, m_particleLabel, allPFParticles);
    LArPandoraHelper::SelectNeutrinoPFParticles(allPFParticles, recoNeutrinoVector);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::PrintAllOutput(const MCTruthVector& mcTruthVector,
                                            const PFParticleVector& recoNeutrinoVector,
                                            const MCPrimaryMatchingMap& mcPrimaryMatchingMap) const
  {
    std::cout << "---RAW-MATCHING-OUTPUT-----------------------------------------------------------"
                 "---------------"
              << std::endl;

    for (const art::Ptr<simb::MCTruth> pMCTruth : mcTruthVector) {
      std::cout << "MCNeutrino, PDG " << pMCTruth->GetNeutrino().Nu().PdgCode()
                << ", InteractionType " << pMCTruth->GetNeutrino().InteractionType() << std::endl;
    }

    for (const art::Ptr<recob::PFParticle> pPfo : recoNeutrinoVector) {
      std::cout << "RecoNeutrino, PDG " << pPfo->PdgCode() << ", nDaughters "
                << pPfo->NumDaughters() << std::endl;
    }

    for (const MCPrimaryMatchingMap::value_type& mapValue : mcPrimaryMatchingMap) {
      const SimpleMCPrimary& simpleMCPrimary(mapValue.first);

      std::cout << std::endl
                << "Primary " << simpleMCPrimary.m_id << ", PDG " << simpleMCPrimary.m_pdgCode
                << ", nMCHits " << simpleMCPrimary.m_nMCHitsTotal << " ("
                << simpleMCPrimary.m_nMCHitsU << ", " << simpleMCPrimary.m_nMCHitsV << ", "
                << simpleMCPrimary.m_nMCHitsW << ")" << std::endl;

      for (const SimpleMatchedPfo& simpleMatchedPfo : mapValue.second) {
        std::cout << "-MatchedPfo " << simpleMatchedPfo.m_id;

        if (simpleMatchedPfo.m_parentId >= 0)
          std::cout << ", ParentPfo " << simpleMatchedPfo.m_parentId;

        std::cout << ", PDG " << simpleMatchedPfo.m_pdgCode << ", nMatchedHits "
                  << simpleMatchedPfo.m_nMatchedHitsTotal << " ("
                  << simpleMatchedPfo.m_nMatchedHitsU << ", " << simpleMatchedPfo.m_nMatchedHitsV
                  << ", " << simpleMatchedPfo.m_nMatchedHitsW << ")"
                  << ", nPfoHits " << simpleMatchedPfo.m_nPfoHitsTotal << " ("
                  << simpleMatchedPfo.m_nPfoHitsU << ", " << simpleMatchedPfo.m_nPfoHitsV << ", "
                  << simpleMatchedPfo.m_nPfoHitsW << ")" << std::endl;
      }
    }

    std::cout << "---------------------------------------------------------------------------------"
                 "---------------"
              << std::endl;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::PerformMatching(const MCPrimaryMatchingMap& mcPrimaryMatchingMap,
                                             MatchingDetailsMap& matchingDetailsMap) const
  {
    // Get best matches, one-by-one, until no more strong matches possible
    IntSet usedMCIds, usedPfoIds;
    while (GetStrongestPfoMatch(mcPrimaryMatchingMap, usedMCIds, usedPfoIds, matchingDetailsMap)) {}

    // Assign any remaining pfos to primaries, based on number of matched hits
    GetRemainingPfoMatches(mcPrimaryMatchingMap, usedPfoIds, matchingDetailsMap);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool PFParticleValidation::GetStrongestPfoMatch(const MCPrimaryMatchingMap& mcPrimaryMatchingMap,
                                                  IntSet& usedMCIds,
                                                  IntSet& usedPfoIds,
                                                  MatchingDetailsMap& matchingDetailsMap) const
  {
    int bestPfoMatchId(-1);
    MatchingDetails bestMatchingDetails;

    for (const MCPrimaryMatchingMap::value_type& mapValue : mcPrimaryMatchingMap) {
      const SimpleMCPrimary& simpleMCPrimary(mapValue.first);

      if (!m_useSmallPrimaries && !this->IsGoodMCPrimary(simpleMCPrimary)) continue;

      if (usedMCIds.count(simpleMCPrimary.m_id)) continue;

      for (const SimpleMatchedPfo& simpleMatchedPfo : mapValue.second) {
        if (usedPfoIds.count(simpleMatchedPfo.m_id)) continue;

        if (!this->IsGoodMatch(simpleMCPrimary, simpleMatchedPfo)) continue;

        if (simpleMatchedPfo.m_nMatchedHitsTotal > bestMatchingDetails.m_nMatchedHits) {
          bestPfoMatchId = simpleMatchedPfo.m_id;
          bestMatchingDetails.m_matchedPrimaryId = simpleMCPrimary.m_id;
          bestMatchingDetails.m_nMatchedHits = simpleMatchedPfo.m_nMatchedHitsTotal;
          bestMatchingDetails.m_completeness =
            static_cast<float>(simpleMatchedPfo.m_nMatchedHitsTotal) /
            static_cast<float>(simpleMCPrimary.m_nMCHitsTotal);
        }
      }
    }

    if (bestPfoMatchId > -1) {
      matchingDetailsMap[bestPfoMatchId] = bestMatchingDetails;
      usedMCIds.insert(bestMatchingDetails.m_matchedPrimaryId);
      usedPfoIds.insert(bestPfoMatchId);
      return true;
    }

    return false;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::GetRemainingPfoMatches(
    const MCPrimaryMatchingMap& mcPrimaryMatchingMap,
    const IntSet& usedPfoIds,
    MatchingDetailsMap& matchingDetailsMap) const
  {
    for (const MCPrimaryMatchingMap::value_type& mapValue : mcPrimaryMatchingMap) {
      const SimpleMCPrimary& simpleMCPrimary(mapValue.first);

      if (!m_useSmallPrimaries && !this->IsGoodMCPrimary(simpleMCPrimary)) continue;

      for (const SimpleMatchedPfo& simpleMatchedPfo : mapValue.second) {
        if (usedPfoIds.count(simpleMatchedPfo.m_id)) continue;

        MatchingDetails& matchingDetails(matchingDetailsMap[simpleMatchedPfo.m_id]);

        if (simpleMatchedPfo.m_nMatchedHitsTotal > matchingDetails.m_nMatchedHits) {
          matchingDetails.m_matchedPrimaryId = simpleMCPrimary.m_id;
          matchingDetails.m_nMatchedHits = simpleMatchedPfo.m_nMatchedHitsTotal;
          matchingDetails.m_completeness =
            static_cast<float>(simpleMatchedPfo.m_nMatchedHitsTotal) /
            static_cast<float>(simpleMCPrimary.m_nMCHitsTotal);
        }
      }
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void PFParticleValidation::PrintMatchingOutput(const MCPrimaryMatchingMap& mcPrimaryMatchingMap,
                                                 const MatchingDetailsMap& matchingDetailsMap) const
  {
    std::cout << "---PROCESSED-MATCHING-OUTPUT-----------------------------------------------------"
                 "---------------"
              << std::endl;
    std::cout << "MinPrimaryGoodHits " << m_matchingMinPrimaryHits << ", MinHitsForGoodView "
              << m_matchingMinHitsForGoodView << ", MinPrimaryGoodViews "
              << m_matchingMinPrimaryGoodViews << std::endl;
    std::cout << "UseSmallPrimaries " << m_useSmallPrimaries << ", MinSharedHits "
              << m_matchingMinSharedHits << ", MinCompleteness " << m_matchingMinCompleteness
              << ", MinPurity " << m_matchingMinPurity << std::endl;

    bool isCorrect(true), isCalculable(false);

    for (const MCPrimaryMatchingMap::value_type& mapValue : mcPrimaryMatchingMap) {
      const SimpleMCPrimary& simpleMCPrimary(mapValue.first);
      const bool hasMatch(this->HasMatch(simpleMCPrimary, mapValue.second, matchingDetailsMap));
      const bool isTargetPrimary(this->IsGoodMCPrimary(simpleMCPrimary) &&
                                 (2112 != simpleMCPrimary.m_pdgCode));

      if (!hasMatch && !isTargetPrimary) continue;

      std::cout << std::endl
                << (!isTargetPrimary ? "(Non target) " : "") << "Primary " << simpleMCPrimary.m_id
                << ", PDG " << simpleMCPrimary.m_pdgCode << ", nMCHits "
                << simpleMCPrimary.m_nMCHitsTotal << " (" << simpleMCPrimary.m_nMCHitsU << ", "
                << simpleMCPrimary.m_nMCHitsV << ", " << simpleMCPrimary.m_nMCHitsW << ")"
                << std::endl;

      if (2112 != simpleMCPrimary.m_pdgCode) isCalculable = true;

      unsigned int nMatches(0);

      for (const SimpleMatchedPfo& simpleMatchedPfo : mapValue.second) {
        if (matchingDetailsMap.count(simpleMatchedPfo.m_id) &&
            (simpleMCPrimary.m_id ==
             matchingDetailsMap.at(simpleMatchedPfo.m_id).m_matchedPrimaryId)) {
          const bool isGoodMatch(this->IsGoodMatch(simpleMCPrimary, simpleMatchedPfo));

          if (isGoodMatch) ++nMatches;
          std::cout << "-" << (!isGoodMatch ? "(Below threshold) " : "") << "MatchedPfo "
                    << simpleMatchedPfo.m_id;

          if (simpleMatchedPfo.m_parentId >= 0)
            std::cout << ", ParentPfo " << simpleMatchedPfo.m_parentId;

          std::cout << ", PDG " << simpleMatchedPfo.m_pdgCode << ", nMatchedHits "
                    << simpleMatchedPfo.m_nMatchedHitsTotal << " ("
                    << simpleMatchedPfo.m_nMatchedHitsU << ", " << simpleMatchedPfo.m_nMatchedHitsV
                    << ", " << simpleMatchedPfo.m_nMatchedHitsW << ")"
                    << ", nPfoHits " << simpleMatchedPfo.m_nPfoHitsTotal << " ("
                    << simpleMatchedPfo.m_nPfoHitsU << ", " << simpleMatchedPfo.m_nPfoHitsV << ", "
                    << simpleMatchedPfo.m_nPfoHitsW << ")" << std::endl;
        }
      }

      if (isTargetPrimary && (1 != nMatches)) isCorrect = false;
    }

    std::cout << std::endl << "Is correct? " << (isCorrect && isCalculable) << std::endl;
    std::cout << "---------------------------------------------------------------------------------"
                 "---------------"
              << std::endl;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool PFParticleValidation::IsGoodMCPrimary(const SimpleMCPrimary& simpleMCPrimary) const
  {
    if (simpleMCPrimary.m_nMCHitsTotal < m_matchingMinPrimaryHits) return false;

    int nGoodViews(0);
    if (simpleMCPrimary.m_nMCHitsU >= m_matchingMinHitsForGoodView) ++nGoodViews;
    if (simpleMCPrimary.m_nMCHitsV >= m_matchingMinHitsForGoodView) ++nGoodViews;
    if (simpleMCPrimary.m_nMCHitsW >= m_matchingMinHitsForGoodView) ++nGoodViews;

    if (nGoodViews < m_matchingMinPrimaryGoodViews) return false;

    return true;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool PFParticleValidation::HasMatch(const SimpleMCPrimary& simpleMCPrimary,
                                      const SimpleMatchedPfoList& simpleMatchedPfoList,
                                      const MatchingDetailsMap& matchingDetailsMap) const
  {
    for (const SimpleMatchedPfo& simpleMatchedPfo : simpleMatchedPfoList) {
      if (matchingDetailsMap.count(simpleMatchedPfo.m_id) &&
          (simpleMCPrimary.m_id == matchingDetailsMap.at(simpleMatchedPfo.m_id).m_matchedPrimaryId))
        return true;
    }

    return false;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool PFParticleValidation::IsGoodMatch(const SimpleMCPrimary& simpleMCPrimary,
                                         const SimpleMatchedPfo& simpleMatchedPfo) const
  {
    const float purity((simpleMatchedPfo.m_nPfoHitsTotal > 0) ?
                         static_cast<float>(simpleMatchedPfo.m_nMatchedHitsTotal) /
                           static_cast<float>(simpleMatchedPfo.m_nPfoHitsTotal) :
                         0.f);
    const float completeness((simpleMCPrimary.m_nMCHitsTotal > 0) ?
                               static_cast<float>(simpleMatchedPfo.m_nMatchedHitsTotal) /
                                 static_cast<float>(simpleMCPrimary.m_nMCHitsTotal) :
                               0.f);

    return ((simpleMatchedPfo.m_nMatchedHitsTotal >= m_matchingMinSharedHits) &&
            (purity >= m_matchingMinPurity) && (completeness >= m_matchingMinCompleteness));
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  unsigned int PFParticleValidation::CountHitsByType(const geo::View_t view,
                                                     const HitVector& hitVector) const
  {
    unsigned int nHitsOfSpecifiedType(0);

    for (const art::Ptr<recob::Hit> pHit : hitVector) {
      if (view == pHit->View()) ++nHitsOfSpecifiedType;
    }

    return nHitsOfSpecifiedType;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool PFParticleValidation::SortSimpleMCPrimaries(const SimpleMCPrimary& lhs,
                                                   const SimpleMCPrimary& rhs)
  {
    if (lhs.m_nMCHitsTotal != rhs.m_nMCHitsTotal) return (lhs.m_nMCHitsTotal > rhs.m_nMCHitsTotal);

    return (lhs.m_energy > rhs.m_energy);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool PFParticleValidation::SortSimpleMatchedPfos(const SimpleMatchedPfo& lhs,
                                                   const SimpleMatchedPfo& rhs)
  {
    if (lhs.m_nMatchedHitsTotal != rhs.m_nMatchedHitsTotal)
      return (lhs.m_nMatchedHitsTotal > rhs.m_nMatchedHitsTotal);

    if (lhs.m_nPfoHitsTotal != rhs.m_nPfoHitsTotal)
      return (lhs.m_nPfoHitsTotal > rhs.m_nPfoHitsTotal);

    return (lhs.m_id < rhs.m_id);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------
  //------------------------------------------------------------------------------------------------------------------------------------------

  PFParticleValidation::SimpleMCPrimary::SimpleMCPrimary()
    : m_id(-1)
    , m_pdgCode(0)
    , m_nMCHitsTotal(0)
    , m_nMCHitsU(0)
    , m_nMCHitsV(0)
    , m_nMCHitsW(0)
    , m_energy(0.f)
    , m_nMatchedPfos(0)
    , m_pAddress(nullptr)
  {}

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool PFParticleValidation::SimpleMCPrimary::operator<(const SimpleMCPrimary& rhs) const
  {
    if (this == &rhs) return false;

    return (m_id < rhs.m_id);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------
  //------------------------------------------------------------------------------------------------------------------------------------------

  PFParticleValidation::SimpleMatchedPfo::SimpleMatchedPfo()
    : m_id(-1)
    , m_parentId(-1)
    , m_pdgCode(0)
    , m_nPfoHitsTotal(0)
    , m_nPfoHitsU(0)
    , m_nPfoHitsV(0)
    , m_nPfoHitsW(0)
    , m_nMatchedHitsTotal(0)
    , m_nMatchedHitsU(0)
    , m_nMatchedHitsV(0)
    , m_nMatchedHitsW(0)
    , m_pAddress(nullptr)
  {}

  //------------------------------------------------------------------------------------------------------------------------------------------
  //------------------------------------------------------------------------------------------------------------------------------------------

  PFParticleValidation::MatchingDetails::MatchingDetails()
    : m_matchedPrimaryId(-1), m_nMatchedHits(0), m_completeness(0.f)
  {}

} //namespace lar_pandora
