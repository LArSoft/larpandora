/**
 *  @file   larpandora/LArPandoraEventBuilding/CollectionSplitting_module.cc
 *
 *  @brief  module for lar pandora collection splitting
 */

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "fhiclcpp/ParameterSet.h"

#include <memory>

namespace lar_pandora {

  class CollectionSplitting;

  class CollectionSplitting : public art::EDProducer {
  public:
    explicit CollectionSplitting(fhicl::ParameterSet const& pset);

    CollectionSplitting(CollectionSplitting const&) = delete;
    CollectionSplitting(CollectionSplitting&&) = delete;
    CollectionSplitting& operator=(CollectionSplitting const&) = delete;
    CollectionSplitting& operator=(CollectionSplitting&&) = delete;

    void produce(art::Event& e) override;

  private:
    std::string
      m_InputProducerLabel; ///< Label for the Pandora instance that produced the collections we want to split up
    std::string
      m_TrackProducerLabel; ///< Label for the track producer using the Pandora instance that produced the collections we want to split up
    std::string
      m_ShowerProducerLabel; ///< Label for the shower producer using the Pandora instance that produced the collections we want to split up
    std::string
      m_HitProducerLabel; ///< Label for the hit producer that was used as input to the Pandora instance specified
    bool
      m_ShouldProduceNeutrinos; ///< If we should produce collections related to neutrino top-level PFParticles
    bool
      m_ShouldProduceCosmics; ///< If we should produce collections related to cosmic top-level PFParticles
    bool
      m_ShouldProduceT0s; ///< If we should produce T0s (relevant when stitching over multiple drift volumes)
  };

  DEFINE_ART_MODULE(CollectionSplitting)

} // namespace lar_pandora

//------------------------------------------------------------------------------------------------------------------------------------------
// implementation follows

#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/PCAxis.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Vertex.h"

#include "larpandora/LArPandoraEventBuilding/LArPandoraEvent.h"

namespace lar_pandora {

  CollectionSplitting::CollectionSplitting(fhicl::ParameterSet const& pset)
    : EDProducer{pset}
    , m_InputProducerLabel(pset.get<std::string>("InputProducerLabel"))
    , m_TrackProducerLabel(pset.get<std::string>("TrackProducerLabel"))
    , m_ShowerProducerLabel(pset.get<std::string>("ShowerProducerLabel"))
    , m_HitProducerLabel(pset.get<std::string>("HitProducerLabel"))
    , m_ShouldProduceNeutrinos(pset.get<bool>("ShouldProduceNeutrinos", true))
    , m_ShouldProduceCosmics(pset.get<bool>("ShouldProduceCosmics", true))
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

  void CollectionSplitting::produce(art::Event& evt)
  {
    if (!m_ShouldProduceNeutrinos && !m_ShouldProduceCosmics)
      throw cet::exception("LArPandora")
        << " CollectionSplitting -- Must be configured to produce neutrinos or cosmics or both.";

    const lar_pandora::LArPandoraEvent::Labels labels(
      m_InputProducerLabel, m_TrackProducerLabel, m_ShowerProducerLabel, m_HitProducerLabel);
    const lar_pandora::LArPandoraEvent fullEvent(this, &evt, labels, m_ShouldProduceT0s);

    if (m_ShouldProduceNeutrinos && m_ShouldProduceCosmics) { fullEvent.WriteToEvent(); }
    else {
      const lar_pandora::LArPandoraEvent filteredEvent(
        fullEvent.FilterByPdgCode(m_ShouldProduceNeutrinos));
      filteredEvent.WriteToEvent();
    }
  }

} // namespace lar_pandora
