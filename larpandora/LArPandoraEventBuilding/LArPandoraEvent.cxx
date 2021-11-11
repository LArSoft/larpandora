/**
 *  @file   larpandora/LArPandoraEventBuilding/LArPandoraEvent.cxx
 *
 *  @brief  A description of all outputs from an instance of pandora with functionality to filter outputs
 */

#include "larpandora/LArPandoraEventBuilding/LArPandoraEvent.h"

#include "lardataobj/RecoBase/TrackHitMeta.h"

namespace lar_pandora
{

LArPandoraEvent::LArPandoraEvent(art::EDProducer *pProducer, art::Event *pEvent, const Labels &inputLabels, const bool shouldProduceT0s) :
    m_pProducer(pProducer),
    m_pEvent(pEvent), 
    m_labels(inputLabels),
    m_shouldProduceT0s(shouldProduceT0s)
{
    this->GetCollections();
}

//------------------------------------------------------------------------------------------------------------------------------------------

LArPandoraEvent::LArPandoraEvent(const LArPandoraEvent &event, const PFParticleVector &selectedPFParticles) :
    m_pProducer(event.m_pProducer),
    m_pEvent(event.m_pEvent),
    m_labels(event.m_labels),
    m_shouldProduceT0s(event.m_shouldProduceT0s),
    m_hits(event.m_hits)
{
    m_pfParticles = selectedPFParticles;
 
    // Only collect objects associated to a selected particles
    for (const auto &part : selectedPFParticles)
    {
        this->CollectAssociated(part, event.m_pfParticleSpacePointMap, m_spacePoints);
        this->CollectAssociated(part, event.m_pfParticleClusterMap, m_clusters);
        this->CollectAssociated(part, event.m_pfParticleVertexMap, m_vertices);
        this->CollectAssociated(part, event.m_pfParticleSliceMap, m_slices);
        this->CollectAssociated(part, event.m_pfParticleTrackMap, m_tracks);
        this->CollectAssociated(part, event.m_pfParticleShowerMap, m_showers);
        this->CollectAssociated(part, event.m_pfParticlePCAxisMap, m_pcAxes);
        this->CollectAssociated(part, event.m_pfParticleMetadataMap, m_metadata);

        if (m_shouldProduceT0s)
            this->CollectAssociated(part, event.m_pfParticleT0Map, m_t0s);
    }

    // Filter the association maps from the input event to only include objects associated to the selected particles
    this->GetFilteredAssociationMap(m_pfParticles, m_spacePoints, event.m_pfParticleSpacePointMap, m_pfParticleSpacePointMap);
    this->GetFilteredAssociationMap(m_pfParticles, m_clusters, event.m_pfParticleClusterMap, m_pfParticleClusterMap);
    this->GetFilteredAssociationMap(m_pfParticles, m_vertices, event.m_pfParticleVertexMap, m_pfParticleVertexMap);
    this->GetFilteredAssociationMap(m_pfParticles, m_slices, event.m_pfParticleSliceMap, m_pfParticleSliceMap);
    this->GetFilteredAssociationMap(m_pfParticles, m_tracks, event.m_pfParticleTrackMap, m_pfParticleTrackMap);
    this->GetFilteredAssociationMap(m_pfParticles, m_showers, event.m_pfParticleShowerMap, m_pfParticleShowerMap);
    this->GetFilteredAssociationMap(m_pfParticles, m_pcAxes, event.m_pfParticlePCAxisMap, m_pfParticlePCAxisMap);
    this->GetFilteredAssociationMap(m_pfParticles, m_metadata, event.m_pfParticleMetadataMap, m_pfParticleMetadataMap);
    this->GetFilteredAssociationMap(m_spacePoints, event.m_hits, event.m_spacePointHitMap, m_spacePointHitMap); 
    this->GetFilteredAssociationMap(m_clusters, event.m_hits, event.m_clusterHitMap, m_clusterHitMap);
    this->GetFilteredAssociationMap(m_slices, event.m_hits, event.m_sliceHitMap, m_sliceHitMap);
    this->GetFilteredAssociationMap(m_tracks, event.m_hits, event.m_trackHitMap, m_trackHitMap);
    this->GetFilteredAssociationMap(m_showers, event.m_hits, event.m_showerHitMap, m_showerHitMap);
    this->GetFilteredAssociationMap(m_showers, m_pcAxes, event.m_showerPCAxisMap, m_showerPCAxisMap);
        
    if (m_shouldProduceT0s)
        this->GetFilteredAssociationMap(m_pfParticles, m_t0s, event.m_pfParticleT0Map, m_pfParticleT0Map);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LArPandoraEvent::WriteToEvent() const
{
    this->WriteCollection(m_pfParticles);
    this->WriteCollection(m_spacePoints);
    this->WriteCollection(m_clusters);
    this->WriteCollection(m_vertices);
    this->WriteCollection(m_slices);
    this->WriteCollection(m_tracks);
    this->WriteCollection(m_showers);
    this->WriteCollection(m_pcAxes);
    this->WriteCollection(m_metadata);

    this->WriteAssociation(m_pfParticleSpacePointMap, m_pfParticles, m_spacePoints);
    this->WriteAssociation(m_pfParticleClusterMap, m_pfParticles, m_clusters);
    this->WriteAssociation(m_pfParticleVertexMap, m_pfParticles, m_vertices);
    this->WriteAssociation(m_pfParticleSliceMap, m_pfParticles, m_slices);
    this->WriteAssociation(m_pfParticleTrackMap, m_pfParticles, m_tracks);
    this->WriteAssociation(m_pfParticleShowerMap, m_pfParticles, m_showers);
    this->WriteAssociation(m_pfParticlePCAxisMap, m_pfParticles, m_pcAxes);
    this->WriteAssociation(m_pfParticleMetadataMap, m_pfParticles, m_metadata);
    this->WriteAssociation(m_spacePointHitMap, m_spacePoints, m_hits, false);
    this->WriteAssociation(m_clusterHitMap, m_clusters, m_hits, false);
    this->WriteAssociation(m_sliceHitMap, m_slices, m_hits, false);
    this->WriteAssociation(m_trackHitMap, m_tracks, m_hits, false);
    this->WriteAssociation(m_showerHitMap, m_showers, m_hits, false);
    this->WriteAssociation(m_showerPCAxisMap, m_showers, m_pcAxes);

    if (m_shouldProduceT0s)
    {
        this->WriteCollection(m_t0s);
        this->WriteAssociation(m_pfParticleT0Map, m_pfParticles, m_t0s);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LArPandoraEvent::GetCollections()
{
    this->GetCollection(Labels::PFParticleLabel, m_pfParticles);
    this->GetCollection(Labels::SpacePointLabel, m_spacePoints); 
    this->GetCollection(Labels::ClusterLabel, m_clusters); 
    this->GetCollection(Labels::VertexLabel, m_vertices); 
    this->GetCollection(Labels::SliceLabel, m_slices); 
    this->GetCollection(Labels::TrackLabel, m_tracks); 
    this->GetCollection(Labels::ShowerLabel, m_showers); 
    this->GetCollection(Labels::PCAxisLabel, m_pcAxes); 
    this->GetCollection(Labels::PFParticleMetadataLabel, m_metadata); 
    this->GetCollection(Labels::HitLabel, m_hits); 

    this->GetAssociationMap(m_pfParticles, Labels::PFParticleToSpacePointLabel, m_pfParticleSpacePointMap);
    this->GetAssociationMap(m_pfParticles, Labels::PFParticleToClusterLabel, m_pfParticleClusterMap);
    this->GetAssociationMap(m_pfParticles, Labels::PFParticleToVertexLabel, m_pfParticleVertexMap);
    this->GetAssociationMap(m_pfParticles, Labels::PFParticleToSliceLabel, m_pfParticleSliceMap);
    this->GetAssociationMap(m_pfParticles, Labels::PFParticleToTrackLabel, m_pfParticleTrackMap);
    this->GetAssociationMap(m_pfParticles, Labels::PFParticleToShowerLabel, m_pfParticleShowerMap);
    this->GetAssociationMap(m_pfParticles, Labels::PFParticleToPCAxisLabel, m_pfParticlePCAxisMap);
    this->GetAssociationMap(m_pfParticles, Labels::PFParticleToMetadataLabel, m_pfParticleMetadataMap);
    this->GetAssociationMap(m_spacePoints, Labels::SpacePointToHitLabel, m_spacePointHitMap);
    this->GetAssociationMap(m_clusters, Labels::ClusterToHitLabel, m_clusterHitMap);
    this->GetAssociationMap(m_slices, Labels::SliceToHitLabel, m_sliceHitMap);
    this->GetAssociationMap(m_tracks, Labels::TrackToHitLabel, m_trackHitMap);
    this->GetAssociationMap(m_showers, Labels::ShowerToHitLabel, m_showerHitMap);
    this->GetAssociationMap(m_showers, Labels::ShowerToPCAxisLabel, m_showerPCAxisMap);

    if (m_shouldProduceT0s)
    {
        this->GetCollection(Labels::T0Label, m_t0s); 
        this->GetAssociationMap(m_pfParticles, Labels::PFParticleToT0Label, m_pfParticleT0Map);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

LArPandoraEvent::Labels::Labels(const std::string &pfParticleProducerLabel, const std::string &hitProducerLabel)
{
    m_labels.emplace(PFParticleLabel, pfParticleProducerLabel);
    m_labels.emplace(SpacePointLabel, pfParticleProducerLabel);
    m_labels.emplace(ClusterLabel, pfParticleProducerLabel);
    m_labels.emplace(VertexLabel, pfParticleProducerLabel);
    m_labels.emplace(SliceLabel, pfParticleProducerLabel);
    m_labels.emplace(TrackLabel, pfParticleProducerLabel);
    m_labels.emplace(ShowerLabel, pfParticleProducerLabel);
    m_labels.emplace(T0Label, pfParticleProducerLabel);
    m_labels.emplace(PFParticleMetadataLabel, pfParticleProducerLabel);
    m_labels.emplace(PCAxisLabel, pfParticleProducerLabel);
    m_labels.emplace(HitLabel, hitProducerLabel);

    m_labels.emplace(PFParticleToSpacePointLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToClusterLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToVertexLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToSliceLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToTrackLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToShowerLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToT0Label, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToMetadataLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToPCAxisLabel, pfParticleProducerLabel);
    m_labels.emplace(SpacePointToHitLabel, pfParticleProducerLabel);
    m_labels.emplace(ClusterToHitLabel, pfParticleProducerLabel);
    m_labels.emplace(SliceToHitLabel, pfParticleProducerLabel);
    m_labels.emplace(TrackToHitLabel, pfParticleProducerLabel);
    m_labels.emplace(ShowerToHitLabel, pfParticleProducerLabel);
    m_labels.emplace(ShowerToPCAxisLabel, pfParticleProducerLabel);
}

//------------------------------------------------------------------------------------------------------------------------------------------

LArPandoraEvent::Labels::Labels(const std::string &pfParticleProducerLabel, const std::string &trackProducerLabel, const std::string &showerProducerLabel,
    const std::string &hitProducerLabel)
{
    m_labels.emplace(PFParticleLabel, pfParticleProducerLabel);
    m_labels.emplace(SpacePointLabel, pfParticleProducerLabel);
    m_labels.emplace(ClusterLabel, pfParticleProducerLabel);
    m_labels.emplace(VertexLabel, pfParticleProducerLabel);
    m_labels.emplace(SliceLabel, pfParticleProducerLabel);
    m_labels.emplace(TrackLabel, trackProducerLabel);
    m_labels.emplace(ShowerLabel, showerProducerLabel);
    m_labels.emplace(T0Label, pfParticleProducerLabel);
    m_labels.emplace(PFParticleMetadataLabel, pfParticleProducerLabel);
    m_labels.emplace(PCAxisLabel, showerProducerLabel);
    m_labels.emplace(HitLabel, hitProducerLabel);

    m_labels.emplace(PFParticleToSpacePointLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToClusterLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToVertexLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToSliceLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToTrackLabel, trackProducerLabel);
    m_labels.emplace(PFParticleToShowerLabel, showerProducerLabel);
    m_labels.emplace(PFParticleToT0Label, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToMetadataLabel, pfParticleProducerLabel);
    m_labels.emplace(PFParticleToPCAxisLabel, showerProducerLabel);
    m_labels.emplace(SpacePointToHitLabel, pfParticleProducerLabel);
    m_labels.emplace(ClusterToHitLabel, pfParticleProducerLabel);
    m_labels.emplace(SliceToHitLabel, pfParticleProducerLabel);
    m_labels.emplace(TrackToHitLabel, trackProducerLabel);
    m_labels.emplace(ShowerToHitLabel, showerProducerLabel);
    m_labels.emplace(ShowerToPCAxisLabel, showerProducerLabel);
}

//------------------------------------------------------------------------------------------------------------------------------------------

const std::string & LArPandoraEvent::Labels::GetLabel(const LabelType type) const
{
    if (m_labels.find(type) == m_labels.end())
        throw cet::exception("LArPandora") << " LArPandoraEvent::GetLabel -- Label map doesn't contain label of requested type" << std::endl;

    return m_labels.at(type);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LArPandoraEvent::Labels::SetLabel(const LabelType type, const std::string &label)
{
    m_labels[type] = label;
}

} // namespace lar_pandora
