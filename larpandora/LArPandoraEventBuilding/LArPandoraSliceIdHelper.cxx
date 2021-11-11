/**
 *  @file  larpandora/LArPandoraEventBuilding/LArPandoraSliceIdHelper.cxx
 *
 *  @brief implementation of the slice id helper class
 *
 */

#include "larpandora/LArPandoraEventBuilding/LArPandoraSliceIdHelper.h"

#include "cetlib_except/exception.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Persistency/Common/FindManyP.h"

#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCParticle.h"

#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Cluster.h"

namespace lar_pandora
{

void LArPandoraSliceIdHelper::GetSliceMetadata(const SliceVector &slices, const art::Event &evt, const std::string &truthLabel,
    const std::string &mcParticleLabel, const std::string &hitLabel, const std::string &backtrackLabel, const std::string &pandoraLabel,
    SliceMetadataVector &sliceMetadata, simb::MCNeutrino &mcNeutrino)
{
    // Find the beam neutrino in MC
    const auto beamNuMCTruth(LArPandoraSliceIdHelper::GetBeamNeutrinoMCTruth(evt, truthLabel));
    mcNeutrino = beamNuMCTruth->GetNeutrino();

    // Collect all MC particles resulting from the beam neutrino
    MCParticleVector mcParticles;
    LArPandoraSliceIdHelper::CollectNeutrinoMCParticles(evt, truthLabel, mcParticleLabel, beamNuMCTruth, mcParticles);

    // Get the hits and determine which are neutrino induced
    HitVector hits;
    HitToBoolMap hitToIsNuInducedMap;
    LArPandoraSliceIdHelper::GetHitOrigins(evt, hitLabel, backtrackLabel, mcParticles, hits, hitToIsNuInducedMap);
    const unsigned int nNuHits(LArPandoraSliceIdHelper::CountNeutrinoHits(hits, hitToIsNuInducedMap));

    // Get the mapping from PFParticle to hits through clusters
    PFParticlesToHits pfParticleToHitsMap;
    LArPandoraSliceIdHelper::GetPFParticleToHitsMap(evt, pandoraLabel, pfParticleToHitsMap);

    // Calculate the metadata for each slice
    LArPandoraSliceIdHelper::GetSliceMetadata(slices, pfParticleToHitsMap, hitToIsNuInducedMap, nNuHits, sliceMetadata);
}

// -----------------------------------------------------------------------------------------------------------------------------------------

art::Ptr<simb::MCTruth> LArPandoraSliceIdHelper::GetBeamNeutrinoMCTruth(const art::Event &evt, const std::string &truthLabel)
{
    // Get the MCTruth handle
    art::Handle< std::vector<simb::MCTruth> > mcTruthHandle;
    evt.getByLabel(truthLabel, mcTruthHandle);
    
    if (!mcTruthHandle.isValid())
        throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetBeamNeutrinoMCTruth - invalid MCTruth handle" << std::endl;

    // Look for the truth block that is from the beam neutrino, and ensure we choose the one with the highest energy if there are multiple
    bool foundNeutrino(false);
    float maxNeutrinoEnergy(-std::numeric_limits<float>::max());
    art::Ptr<simb::MCTruth> beamNuMCTruth;   
    for (unsigned int i = 0; i < mcTruthHandle->size(); ++i)
    {
        const art::Ptr<simb::MCTruth> mcTruth(mcTruthHandle, i);
        
        if (mcTruth->Origin() != simb::kBeamNeutrino)
            continue;

        const float nuEnergy(mcTruth->GetNeutrino().Nu().E());
        if (nuEnergy < maxNeutrinoEnergy)
            continue;

        // Select this as the beam neutrino
        maxNeutrinoEnergy = nuEnergy;
        beamNuMCTruth = mcTruth;
        foundNeutrino = true;
    }

    if (!foundNeutrino)
        throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetBeamNeutrinoMCTruth - found no beam neutrino MCTruth blocks" << std::endl;

    return beamNuMCTruth;
}

// -----------------------------------------------------------------------------------------------------------------------------------------

void LArPandoraSliceIdHelper::CollectNeutrinoMCParticles(const art::Event &evt, const std::string &truthLabel,
    const std::string &mcParticleLabel, const art::Ptr<simb::MCTruth> &beamNuMCTruth, MCParticleVector &mcParticles)
{
    // Get the MCTruth handle
    art::Handle< std::vector<simb::MCTruth> > mcTruthHandle;
    evt.getByLabel(truthLabel, mcTruthHandle);
    
    if (!mcTruthHandle.isValid())
        throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::CollectNeutrinoMCParticles - invalid MCTruth handle" << std::endl;

    // Find MCParticles that are associated to the beam neutrino MCTruth block
    art::FindManyP<simb::MCParticle> truthToMCParticleAssns(mcTruthHandle, evt, mcParticleLabel);
    mcParticles = truthToMCParticleAssns.at(beamNuMCTruth.key()); // ATTN will throw if association from beamNuMCTruth doesn't exist. We want this!
}

// -----------------------------------------------------------------------------------------------------------------------------------------

void LArPandoraSliceIdHelper::GetHitOrigins(const art::Event &evt, const std::string &hitLabel, const std::string &backtrackLabel,
    const MCParticleVector &mcParticles, HitVector &hits, HitToBoolMap &hitToIsNuInducedMap)
{
    // Collect the hits from the event
    art::Handle< std::vector<recob::Hit> > hitHandle;
    evt.getByLabel(hitLabel, hitHandle);

    if (!hitHandle.isValid())
        throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetHitOrigins - invalid hit handle" << std::endl;

    art::FindManyP<simb::MCParticle> hitToMCParticleAssns(hitHandle, evt, backtrackLabel);

    // Find the hits that are associated to a neutrino induced MCParticle using the Hit->MCParticle associations form the backtracker
    for (unsigned int i = 0; i < hitHandle->size(); ++i)
    {
        const art::Ptr<recob::Hit> hit(hitHandle, i);
        hits.push_back(hit);

        const auto &particles(hitToMCParticleAssns.at(hit.key()));

        bool foundNuParticle(false);
        for (const auto &part : particles)
        {
            // If the MCParticles isn't in the list of neutrino particles
            if (std::find(mcParticles.begin(), mcParticles.end(), part) == mcParticles.end())
                continue;

            foundNuParticle = true;
            break;
        }
        
        if (!hitToIsNuInducedMap.emplace(hit, foundNuParticle).second)
            throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetHitOrigins - repeated hits in input collection" << std::endl;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------

unsigned int LArPandoraSliceIdHelper::CountNeutrinoHits(const HitVector &hits, const HitToBoolMap &hitToIsNuInducedMap)
{
    unsigned int nNuHits(0);
    for (const auto &hit : hits)
    {
        const auto it(hitToIsNuInducedMap.find(hit));

        if (it == hitToIsNuInducedMap.end())
            throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::CountNeutrinoHits - can't find hit in hitToIsNuInducedMap" << std::endl;

        nNuHits += it->second ? 1 : 0;
    }

    return nNuHits;
}

// -----------------------------------------------------------------------------------------------------------------------------------------

void LArPandoraSliceIdHelper::GetPFParticleToHitsMap(const art::Event &evt, const std::string &pandoraLabel, PFParticlesToHits &pfParticleToHitsMap)
{
    // Get the PFParticles
    art::Handle< std::vector<recob::PFParticle> > pfParticleHandle;
    evt.getByLabel(pandoraLabel, pfParticleHandle);
    
    if (!pfParticleHandle.isValid())
        throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetPFParticleToHitsMap - invalid PFParticle handle" << std::endl;
    
    // Get the Clusters
    art::Handle< std::vector<recob::Cluster> > clusterHandle;
    evt.getByLabel(pandoraLabel, clusterHandle);
    
    if (!clusterHandle.isValid())
        throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetPFParticleToHitsMap - invalid cluster handle" << std::endl;

    // Get the associations between PFParticles -> Clusters -> Hits
    art::FindManyP<recob::Cluster> pfParticleToClusterAssns(pfParticleHandle, evt, pandoraLabel);
    art::FindManyP<recob::Hit> clusterToHitAssns(clusterHandle, evt, pandoraLabel);

    // Get the hits associated to each PFParticles
    for (unsigned int iPart = 0; iPart < pfParticleHandle->size(); ++iPart)
    {
        const art::Ptr<recob::PFParticle> part(pfParticleHandle, iPart);
        HitVector hits;

        for (const auto &cluster : pfParticleToClusterAssns.at(part.key()))
        {
            for (const auto &hit : clusterToHitAssns.at(cluster.key()))
            {
                if (std::find(hits.begin(), hits.end(), hit) != hits.end())
                    throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetPFParticleToHitsMap - double counted hits!" << std::endl;

                hits.push_back(hit);
            }
        }
        
        if (!pfParticleToHitsMap.emplace(part, hits).second)
            throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetPFParticleToHitsMap - repeated input PFParticles" << std::endl;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------

void LArPandoraSliceIdHelper::GetReconstructedHitsInSlice(const Slice &slice, const PFParticlesToHits &pfParticleToHitsMap, HitVector &hits)
{
    // ATTN here we use the PFParticles from both hypotheses to collect the hits. Hits will not be double counted
    LArPandoraSliceIdHelper::CollectHits(slice.GetTargetHypothesis(), pfParticleToHitsMap, hits);
    LArPandoraSliceIdHelper::CollectHits(slice.GetCosmicRayHypothesis(), pfParticleToHitsMap, hits);
}

// -----------------------------------------------------------------------------------------------------------------------------------------
    
void LArPandoraSliceIdHelper::CollectHits(const PFParticleVector &pfParticles, const PFParticlesToHits &pfParticleToHitsMap, HitVector &hits)
{
    for (const auto &part : pfParticles)
    {
        const auto it(pfParticleToHitsMap.find(part));
        if (it == pfParticleToHitsMap.end())
            throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::CollectHits - can't find any hits associated to input PFParticle" << std::endl;

        for (const auto &hit : it->second)
        {
            // ATTN here we ensure that we don't double count hits, even if the input PFParticles are from different Pandora instances
            if (std::find(hits.begin(), hits.end(), hit) == hits.end())
                hits.push_back(hit);
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------

void LArPandoraSliceIdHelper::GetSliceMetadata(const SliceVector &slices, const PFParticlesToHits &pfParticleToHitsMap,
    const HitToBoolMap &hitToIsNuInducedMap, const unsigned int nNuHits, SliceMetadataVector &sliceMetadata)
{
    if (!sliceMetadata.empty())
        throw cet::exception("LArPandora") << " LArPandoraSliceIdHelper::GetSliceMetadata - non empty input metadata vector" << std::endl;

    if (slices.empty())
        return;

    unsigned int mostCompleteSliceIndex(0);
    unsigned int maxNuHits(0);
    
    for (unsigned int sliceIndex = 0; sliceIndex < slices.size(); ++sliceIndex)
    {
        const Slice &slice(slices.at(sliceIndex));
        HitVector hits;
        LArPandoraSliceIdHelper::GetReconstructedHitsInSlice(slice, pfParticleToHitsMap, hits);

        const unsigned int nHitsInSlice(hits.size());
        const unsigned int nNuHitsInSlice(LArPandoraSliceIdHelper::CountNeutrinoHits(hits, hitToIsNuInducedMap));

        if (nNuHitsInSlice > maxNuHits)
        {
            mostCompleteSliceIndex = sliceIndex;
            maxNuHits = nNuHitsInSlice;
        }

        SliceMetadata metadata;
        metadata.m_nHits = nHitsInSlice;
        metadata.m_purity = ((nHitsInSlice == 0) ? -1.f : static_cast<float>(nNuHitsInSlice) / static_cast<float>(nHitsInSlice));
        metadata.m_completeness = ((nNuHits == 0) ? -1.f : static_cast<float>(nNuHitsInSlice) / static_cast<float>(nNuHits));
        metadata.m_isMostComplete = false;

        sliceMetadata.push_back(metadata);
    }

    sliceMetadata.at(mostCompleteSliceIndex).m_isMostComplete = true;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------
        
LArPandoraSliceIdHelper::SliceMetadata::SliceMetadata() :
    m_purity(-std::numeric_limits<float>::max()),
    m_completeness(-std::numeric_limits<float>::max()),
    m_nHits(std::numeric_limits<unsigned int>::max()),
    m_isMostComplete(false)
{
}

} // namespace lar_pandora
