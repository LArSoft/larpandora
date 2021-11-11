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

namespace lar_pandora
{

class CollectionSplitting;


class CollectionSplitting : public art::EDProducer
{
public:
    explicit CollectionSplitting(fhicl::ParameterSet const & pset);

    CollectionSplitting(CollectionSplitting const &) = delete;
    CollectionSplitting(CollectionSplitting &&) = delete;
    CollectionSplitting & operator = (CollectionSplitting const &) = delete;
    CollectionSplitting & operator = (CollectionSplitting &&) = delete;

    void produce(art::Event & e) override;

private:
    std::string     m_inputProducerLabel;           ///< Label for the Pandora instance that produced the collections we want to split up
    std::string     m_trackProducerLabel;           ///< Label for the track producer using the Pandora instance that produced the collections we want to split up
    std::string     m_showerProducerLabel;          ///< Label for the shower producer using the Pandora instance that produced the collections we want to split up
    std::string     m_hitProducerLabel;             ///< Label for the hit producer that was used as input to the Pandora instance specified
    bool            m_shouldProduceT0s;             ///< If we should produce T0s (relevant when stitching over multiple drift volumes)
};

DEFINE_ART_MODULE(CollectionSplitting)

} // namespace lar_pandora

//------------------------------------------------------------------------------------------------------------------------------------------
// implementation follows

#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/PCAxis.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Slice.h"
#include "lardataobj/RecoBase/TrackHitMeta.h"

#include "larpandora/LArPandoraEventBuilding/LArPandoraEvent.h"

namespace lar_pandora
{

CollectionSplitting::CollectionSplitting(fhicl::ParameterSet const &pset) :
    EDProducer{pset},
    m_inputProducerLabel(pset.get<std::string>("InputProducerLabel")),
    m_trackProducerLabel(pset.get<std::string>("TrackProducerLabel")),
    m_showerProducerLabel(pset.get<std::string>("ShowerProducerLabel")),
    m_hitProducerLabel(pset.get<std::string>("HitProducerLabel")),
    m_shouldProduceT0s(pset.get<bool>("ShouldProduceT0s", false))
{
    produces< std::vector<recob::PFParticle> >();
    produces< std::vector<recob::SpacePoint> >();
    produces< std::vector<recob::Cluster> >();
    produces< std::vector<recob::Vertex> >();
    produces< std::vector<recob::Slice> >();
    produces< std::vector<recob::Track> >();
    produces< std::vector<recob::Shower> >();
    produces< std::vector<recob::PCAxis> >();
    produces< std::vector<larpandoraobj::PFParticleMetadata> >();

    produces< art::Assns<recob::PFParticle, recob::SpacePoint> >();
    produces< art::Assns<recob::PFParticle, recob::Cluster> >();
    produces< art::Assns<recob::PFParticle, recob::Vertex> >();
    produces< art::Assns<recob::PFParticle, recob::Slice> >();
    produces< art::Assns<recob::PFParticle, recob::Track> >();
    produces< art::Assns<recob::PFParticle, recob::Shower> >();
    produces< art::Assns<recob::PFParticle, recob::PCAxis> >();
    produces< art::Assns<recob::PFParticle, larpandoraobj::PFParticleMetadata> >();
    produces< art::Assns<recob::Track, recob::Hit, recob::TrackHitMeta> >();
    produces< art::Assns<recob::Shower, recob::Hit> >();
    produces< art::Assns<recob::Shower, recob::PCAxis> >();
    produces< art::Assns<recob::SpacePoint, recob::Hit> >();
    produces< art::Assns<recob::Cluster, recob::Hit> >();
    produces< art::Assns<recob::Slice, recob::Hit> >();

    if (m_shouldProduceT0s)
    {
        produces< std::vector<anab::T0> >();
        produces< art::Assns<recob::PFParticle, anab::T0> >();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CollectionSplitting::produce(art::Event &evt)
{
    const lar_pandora::LArPandoraEvent::Labels labels(m_inputProducerLabel, m_trackProducerLabel, m_showerProducerLabel, m_hitProducerLabel); 
    const lar_pandora::LArPandoraEvent pandoraEvent(this, &evt, labels, m_shouldProduceT0s);

    pandoraEvent.WriteToEvent();
}

} // namespace lar_pandora
