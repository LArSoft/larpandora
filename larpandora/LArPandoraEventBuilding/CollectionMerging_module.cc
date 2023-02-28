/**
 *  @file   larpandora/LArPandoraEventBuilding/CollectionMerging_module.cc
 *
 *  @brief  module for lar pandora collection merging
 */

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "fhiclcpp/ParameterSet.h"

#include <memory>

namespace lar_pandora {

  class CollectionMerging : public art::EDProducer {
  public:
    explicit CollectionMerging(fhicl::ParameterSet const& pset);

    CollectionMerging(CollectionMerging const&) = delete;
    CollectionMerging(CollectionMerging&&) = delete;
    CollectionMerging& operator=(CollectionMerging const&) = delete;
    CollectionMerging& operator=(CollectionMerging&&) = delete;

    void produce(art::Event& evt) override;

  private:
    std::string
      m_AllHitsCRProducerLabel; ///< Label of the pandora instance that ran CR reco on all hits
    std::string
      m_AllHitsCRTrackProducerLabel; ///< Label of the track producer using the pandora instance that ran CR reco on all hits
    std::string
      m_AllHitsCRShowerProducerLabel; ///< Label of the shower producer using the pandora instance that ran CR reco on all hits

    std::string
      m_CRRemHitsCRProducerLabel; ///< Label of the pandora instance that ran CR reco on CR removed hits
    std::string
      m_CRRemHitsCRTrackProducerLabel; ///< Label of the track producer using the pandora instance that ran CR reco on CR removed hits
    std::string
      m_CRRemHitsCRShowerProducerLabel; ///< Label of the shower producer using the pandora instance that ran CR reco on CR removed hits

    std::string
      m_CRRemHitsNuProducerLabel; ///< Label of the pandora instance that ran Nu reco on CR removed hits
    std::string
      m_CRRemHitsNuTrackProducerLabel; ///< Label of the track producer using the pandora instance that ran Nu reco on CR removed hits
    std::string
      m_CRRemHitsNuShowerProducerLabel; ///< Label of the shower producer using the pandora instance that ran Nu reco on CR removed hits

    std::string m_AllHitProducerLabel;   ///< Label of the primary hit producer
    std::string m_CRRemHitProducerLabel; ///< Label of the CR removed hit producer

    std::string m_ClearCRTagProducerLabel; ///< Label of the unabiguous CR tag producer
    std::string m_NuIdCRTagProducerLabel;  ///< Label of the neutrino-ID CR tag producer

    bool
      m_ShouldProduceNeutrinos; ///< If we should produce collections related to neutrino top-level PFParticles
    bool
      m_ShouldProduceT0s; ///< If we should produce T0s (relevant when stitching over multiple drift volumes)
  };

  DEFINE_ART_MODULE(CollectionMerging)

} // namespace lar_pandora

//------------------------------------------------------------------------------------------------------------------------------------------
// implementation follows

#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/PCAxis.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Vertex.h"

#include "larpandora/LArPandoraEventBuilding/LArPandoraEvent.h"

namespace lar_pandora {

  CollectionMerging::CollectionMerging(fhicl::ParameterSet const& pset)
    : EDProducer{pset}
    , m_AllHitsCRProducerLabel(pset.get<std::string>("AllHitsCRProducerLabel"))
    , m_AllHitsCRTrackProducerLabel(pset.get<std::string>("AllHitsCRTrackProducerLabel"))
    , m_AllHitsCRShowerProducerLabel(pset.get<std::string>("AllHitsCRShowerProducerLabel"))
    , m_CRRemHitsCRProducerLabel(pset.get<std::string>("CRRemHitsCRProducerLabel"))
    , m_CRRemHitsCRTrackProducerLabel(pset.get<std::string>("CRRemHitsCRTrackProducerLabel"))
    , m_CRRemHitsCRShowerProducerLabel(pset.get<std::string>("CRRemHitsCRShowerProducerLabel"))
    , m_CRRemHitsNuProducerLabel(pset.get<std::string>("CRRemHitsNuProducerLabel"))
    , m_CRRemHitsNuTrackProducerLabel(pset.get<std::string>("CRRemHitsNuTrackProducerLabel"))
    , m_CRRemHitsNuShowerProducerLabel(pset.get<std::string>("CRRemHitsNuShowerProducerLabel"))
    , m_AllHitProducerLabel(pset.get<std::string>("AllHitProducerLabel"))
    , m_CRRemHitProducerLabel(pset.get<std::string>("CRRemHitProducerLabel"))
    , m_ClearCRTagProducerLabel(pset.get<std::string>("ClearCRTagProducerLabel"))
    , m_NuIdCRTagProducerLabel(pset.get<std::string>("NuIdCRTagProducerLabel"))
    , m_ShouldProduceNeutrinos(pset.get<bool>("ShouldProduceNeutrinos", true))
    , m_ShouldProduceT0s(pset.get<bool>("ShouldProduceT0s", false))
  {
    produces<std::vector<recob::PFParticle>>();
    produces<std::vector<recob::SpacePoint>>();
    produces<std::vector<recob::Cluster>>();
    produces<std::vector<recob::Vertex>>();
    produces<std::vector<recob::Track>>();
    produces<std::vector<recob::Shower>>();
    produces<std::vector<recob::PCAxis>>();
    produces<std::vector<larpandoraobj::PFParticleMetadata>>();

    produces<art::Assns<recob::PFParticle, recob::SpacePoint>>();
    produces<art::Assns<recob::PFParticle, recob::Cluster>>();
    produces<art::Assns<recob::PFParticle, recob::Vertex>>();
    produces<art::Assns<recob::PFParticle, recob::Track>>();
    produces<art::Assns<recob::PFParticle, recob::Shower>>();
    produces<art::Assns<recob::PFParticle, recob::PCAxis>>();
    produces<art::Assns<recob::PFParticle, larpandoraobj::PFParticleMetadata>>();
    produces<art::Assns<recob::Track, recob::Hit>>();
    produces<art::Assns<recob::Shower, recob::Hit>>();
    produces<art::Assns<recob::Shower, recob::PCAxis>>();
    produces<art::Assns<recob::SpacePoint, recob::Hit>>();
    produces<art::Assns<recob::Cluster, recob::Hit>>();

    if (m_ShouldProduceT0s) {
      produces<std::vector<anab::T0>>();
      produces<art::Assns<recob::PFParticle, anab::T0>>();
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void CollectionMerging::produce(art::Event& evt)
  {
    const lar_pandora::LArPandoraEvent::Labels allHitsCRLabels(m_AllHitsCRProducerLabel,
                                                               m_AllHitsCRTrackProducerLabel,
                                                               m_AllHitsCRShowerProducerLabel,
                                                               m_AllHitProducerLabel);
    const lar_pandora::LArPandoraEvent allHitsCREvent(
      this, &evt, allHitsCRLabels, m_ShouldProduceT0s);

    const lar_pandora::LArPandoraEvent::Labels crRemHitsCRLabels(m_CRRemHitsCRProducerLabel,
                                                                 m_CRRemHitsCRTrackProducerLabel,
                                                                 m_CRRemHitsCRShowerProducerLabel,
                                                                 m_CRRemHitProducerLabel);
    const lar_pandora::LArPandoraEvent crRemHitsCREvent(
      this, &evt, crRemHitsCRLabels, m_ShouldProduceT0s);

    const lar_pandora::LArPandoraEvent::Labels crRemHitsNuLabels(m_CRRemHitsNuProducerLabel,
                                                                 m_CRRemHitsNuTrackProducerLabel,
                                                                 m_CRRemHitsNuShowerProducerLabel,
                                                                 m_CRRemHitProducerLabel);
    const lar_pandora::LArPandoraEvent crRemHitsNuEvent(
      this, &evt, crRemHitsNuLabels, m_ShouldProduceT0s);

    if (m_ShouldProduceNeutrinos) {
      const lar_pandora::LArPandoraEvent filteredCRRemHitsNuEvent(
        crRemHitsNuEvent.FilterByCRTag(m_ShouldProduceNeutrinos, m_NuIdCRTagProducerLabel));
      filteredCRRemHitsNuEvent.WriteToEvent();
    }
    else {
      const lar_pandora::LArPandoraEvent filteredAllHitsCREvent(
        allHitsCREvent.FilterByCRTag(m_ShouldProduceNeutrinos, m_ClearCRTagProducerLabel));
      const lar_pandora::LArPandoraEvent filteredCRRemHitsCREvent(
        crRemHitsCREvent.FilterByCRTag(m_ShouldProduceNeutrinos, m_NuIdCRTagProducerLabel));
      const lar_pandora::LArPandoraEvent mergedEvent(
        filteredAllHitsCREvent.Merge(filteredCRRemHitsCREvent));
      mergedEvent.WriteToEvent();
    }
  }

} // namespace lar_pandora
