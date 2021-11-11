/**
 *  @file   larpandora/LArPandoraEventBuilding/LArPandoraEvent.h
 *
 *  @brief  A description of all outputs from an instance of pandora with functionality to filter and merge multiple output
 */

#ifndef LAR_PANDORA_EVENT_H
#define LAR_PANDORA_EVENT_H 1

#include "art/Persistency/Common/PtrMaker.h"
#include "canvas/Persistency/Common/Assns.h"
#include "art/Framework/Principal/Event.h"

#include "lardata/Utilities/AssociationUtil.h"

#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/PFParticleMetadata.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/TrackHitMeta.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/PCAxis.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Slice.h"
#include "lardataobj/AnalysisBase/T0.h"

#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

#include <memory>
#include <algorithm>
#include <map>

namespace lar_pandora
{

/**
 *  @brief LArPandoraEvent class
 */
class LArPandoraEvent
{
public:
    /**
     *  @brief Shorthand for a collection of objects of type T
     */
    template <typename T>
    using Collection = std::vector< art::Ptr<T> >;

    template <typename R, typename D>
    using PairVector = std::vector< std::pair< art::Ptr<R>, D > >;

    /**
     *  @brief General purpose short-hand with optional D parameter
     */
    template <typename L, typename R, typename D>
    using Association = std::map< art::Ptr<L>, PairVector<R, D> >;

    // Collection typedef specializations
    typedef Collection<recob::Hit>                         HitCollection;
    typedef Collection<recob::PFParticle>                  PFParticleCollection;
    typedef Collection<recob::Cluster>                     ClusterCollection;
    typedef Collection<recob::SpacePoint>                  SpacePointCollection;
    typedef Collection<recob::Vertex>                      VertexCollection;
    typedef Collection<recob::Slice>                       SliceCollection;
    typedef Collection<recob::Track>                       TrackCollection;
    typedef Collection<recob::Shower>                      ShowerCollection;
    typedef Collection<recob::PCAxis>                      PCAxisCollection;
    typedef Collection<larpandoraobj::PFParticleMetadata>  PFParticleMetadataCollection;
    typedef Collection<anab::T0>                           T0Collection;

    // Association typedef specializations
    typedef Association<recob::PFParticle, recob::Cluster, void*>                     PFParticleToClusterAssoc;
    typedef Association<recob::PFParticle, recob::SpacePoint, void*>                  PFParticleToSpacePointAssoc;
    typedef Association<recob::PFParticle, recob::Vertex, void*>                      PFParticleToVertexAssoc;
    typedef Association<recob::PFParticle, recob::Slice, void*>                       PFParticleToSliceAssoc;
    typedef Association<recob::PFParticle, recob::Track, void*>                       PFParticleToTrackAssoc;
    typedef Association<recob::PFParticle, recob::Shower, void*>                      PFParticleToShowerAssoc;
    typedef Association<recob::PFParticle, recob::PCAxis, void*>                      PFParticleToPCAxisAssoc;
    typedef Association<recob::PFParticle, larpandoraobj::PFParticleMetadata, void*>  PFParticleToPFParticleMetadataAssoc;
    typedef Association<recob::PFParticle, anab::T0, void*>                           PFParticleToT0Assoc;

    typedef Association<recob::Cluster, recob::Hit, void*>                            ClusterToHitAssoc;
    typedef Association<recob::SpacePoint, recob::Hit, void*>                         SpacePointToHitAssoc;
    typedef Association<recob::Slice, recob::Hit, void*>                              SliceToHitAssoc;
    typedef Association<recob::Track, recob::Hit, recob::TrackHitMeta>                TrackToHitAssoc;
    typedef Association<recob::Shower, recob::Hit, void*>                             ShowerToHitAssoc;
    typedef Association<recob::Shower, recob::PCAxis, void*>                          ShowerToPCAxisAssoc;

    /**
     *  @brief  Class to handle the required producer labels 
     */
    class Labels
    {
    public:
        /**
         *  @brief  Label type enumeration
         */
        enum LabelType
        {
            PFParticleLabel,
            SpacePointLabel,
            ClusterLabel,
            VertexLabel,
            SliceLabel,
            TrackLabel,
            ShowerLabel,
            T0Label,
            PFParticleMetadataLabel,
            PCAxisLabel,
            HitLabel,
            PFParticleToSpacePointLabel,
            PFParticleToClusterLabel,
            PFParticleToVertexLabel,
            PFParticleToSliceLabel,
            PFParticleToTrackLabel,
            PFParticleToShowerLabel,
            PFParticleToT0Label,
            PFParticleToMetadataLabel,
            PFParticleToPCAxisLabel,
            SpacePointToHitLabel,
            ClusterToHitLabel,
            SliceToHitLabel,
            TrackToHitLabel,
            ShowerToHitLabel,
            ShowerToPCAxisLabel
        };

        /**
         *  @brief  Minimal parametrised constructor.
         *          Sets all collection labels to be the same as the PFParticle producer label 
         */
        Labels(const std::string &pfParticleProducerLabel, const std::string &hitProducerLabel);

        /**
         *  @brief  Track / Shower parametrised constructor.
         *          Sets all collection labels to be the same as the PFParticle producer label,
         *          except those relating to track and shower production, which are supplied.
         */
        Labels(const std::string &pfParticleProducerLabel, const std::string &trackProducerLabel, const std::string &showerProducerLabel,
            const std::string &hitProducerLabel);

        /**
         *  @brief  Get the label of a given type
         *
         *  @param  type the label type to retrieve
         *
         *  @return the label
         */
        const std::string & GetLabel(const LabelType type) const;

        /**
         *  @brief  Set the label of a given type
         *
         *  @param  type the label type to set
         *  @param  label the label to set
         */
        void SetLabel(const LabelType type, const std::string &label);

    private:
        std::map<LabelType, std::string> m_labels;   ///< Map holding the labels
    };

    /**
     *  @brief  Constructor from an art::Event
     *
     *  @param  pProducer pointer to the producer to write the output
     *  @param  pEvent pointer to the event to process
     *  @param  inputLabel labels for the producers of the input collections
     *  @param  shouldProduceT0s if T0s should be produced (usually only for multiple drift volume use cases)
     */
    LArPandoraEvent(art::EDProducer *pProducer, art::Event *pEvent, const Labels &inputLabels, const bool shouldProduceT0s = false);

    /**
     *  @brief  Construct by copying an existing LArPandoraEvent, replacing the collections and associations 
     *          by any objects associated with a PFParticle in the selection supplied.
     * 
     *  @param  event input event to copy and filter
     *  @param  pfParticleVector input vector of selected particles
     */
    LArPandoraEvent(const LArPandoraEvent &event, const PFParticleVector &selectedPFParticles);

    /**
     *  @brief  Write (put) the collections in this LArPandoraEvent to the art::Event
     */
    void WriteToEvent() const;

private:
    /**
     *  @brief  Get the collections and associations from m_pEvent with the required labels
     */
    void GetCollections();

    /**
     *  @brief  Gets a given collection from m_pEvent with the label supplied
     *
     *  @param  inputLabel a label for the producer of the collection required
     *  @param  outputCollection the required collection
     */
    template <typename T>
    void GetCollection(const Labels::LabelType &inputLabel, Collection<T> &outputCollection) const;

    /**
     *  @brief  Get the mapping between two collections with metadata using the specified label
     *
     *  @param  collectionL the collection from which the associations should be retrieved
     *  @param  inputLabel a label for the producer of the association required
     *  @param  outputAssociationMap output mapping between the two data types supplied (L -> R + D)
     */
    template <typename L, typename R, typename D>
    void GetAssociationMap(const Collection<L> &collectionL, const Labels::LabelType &inputLabel,
        Association<L, R, D> &outputAssociationMap) const;

    /**
     *  @brief  Get the mapping between two collections with metadata using the specified label
     *
     *  @param  collectionL the collection from which the associations should be retrieved
     *  @param  inputLabel a label for the producer of the association required
     *  @param  outputAssociationMap output mapping between the two data types supplied (L -> R no metadata)
     */
    template <typename L, typename R>
    void GetAssociationMap(const Collection<L> &collectionL, const Labels::LabelType &inputLabel,
        Association<L, R, void*> &outputAssociationMap) const;

    /**
     *  @brief  Collects all objects of type R with metadata D associated to a given object of type L
     *
     *  @param  anObject an input object of type L with which we want to collect associated objects of type R with metadata D
     *  @param  associationLtoR the general input association between objects of type L and R
     *  @param  associatedR output vector of objects of type R associated with anObject
     */
    template <typename L, typename R, typename D>
    void CollectAssociated(const art::Ptr<L> &anObject, const Association<L, R, D> &associationLtoR, Collection<R> &associatedR) const;

    /**
     *   @brief  Gets the filtered mapping from objets in collectionL to objects that also exist in collectionR using a "superset" input association
     *
     *   @param  collectionL a first filtered collection
     *   @param  collectionR a second filtered collection
     *   @param  inputAssociationLtoR mapping between the two unfiltered collections
     *   @param  outputAssociationLtoR mapping between the two filtered collections
     *
     *   @return mapping between the filtered collections
     */
    template <typename L, typename R, typename D>
    void GetFilteredAssociationMap(const Collection<L> &collectionL, const Collection<R> &collectionR,
        const Association<L, R, D> &inputAssociationLtoR, Association<L, R, D> &outputAssociationLtoR) const;

    /**
     *  @brief  Write a given collection to the event
     *
     *  @param  collection the collection to write
     */
    template <typename T>
    void WriteCollection(const Collection<T> &collection) const;

    /**
     *  @brief  Write a given association to the event
     *
     *  @param  associationMap the association to write from objects of type L -> R + D
     *  @param  collectionL the collection of type L that has been written
     *  @param  collectionR the collection of type R that has been written
     *  @param  thisProducesR will this producer produce collectionR of was it produced by a different module?
     */
    template <typename L, typename R, typename D>
    void WriteAssociation(const Association<L, R, D> &associationMap, const Collection<L> &collectionL, const Collection<R> &collectionR,
        const bool thisProducesR = true) const;

    /**
     *  @brief  Write a given association to the event
     *
     *  @param  associationMap the association to write from objects of type L -> R (no metadata)
     *  @param  collectionL the collection of type L that has been written
     *  @param  collectionR the collection of type R that has been written
     *  @param  thisProducesR will this producer produce collectionR of was it produced by a different module?
     */
    template <typename L, typename R>
    void WriteAssociation(const Association<L, R, void*> &associationMap, const Collection<L> &collectionL, const Collection<R> &collectionR,
        const bool thisProducesR = true) const;

    /**
     *  @brief  Get the index of an objet in a given collection
     *
     *  @param  object the object to search for
     *  @param  collection the collection to search through
     *
     *  @return the index of the object in the collection
     */
    template<typename T>
    size_t GetIndex(const art::Ptr<T> object, const Collection<T> &collection) const;

    art::EDProducer            *m_pProducer;                    ///<  The producer which should write the output collections and associations
    art::Event                 *m_pEvent;                       ///<  The event to consider
    Labels                      m_labels;                       ///<  A set of labels describing the producers for each input collection
    bool                        m_shouldProduceT0s;             ///<  If T0s should be produced (usually only true for use cases with multiple drift volumes)

    // Collections
    PFParticleCollection                m_pfParticles;             ///<  The input collection of PFParticles
    SpacePointCollection                m_spacePoints;             ///<  The input collection of SpacePoints
    ClusterCollection                   m_clusters;                ///<  The input collection of Clusters
    VertexCollection                    m_vertices;                ///<  The input collection of Vertices
    SliceCollection                     m_slices;                  ///<  The input collection of Slices
    TrackCollection                     m_tracks;                  ///<  The input collection of Tracks
    ShowerCollection                    m_showers;                 ///<  The input collection of Showers
    T0Collection                        m_t0s;                     ///<  The input collection of T0s
    PFParticleMetadataCollection        m_metadata;                ///<  The input collection of PFParticle metadata
    PCAxisCollection                    m_pcAxes;                  ///<  The input collection of PCAxes
    HitCollection                       m_hits;                    ///<  The input collection of Hits

    // Association maps
    PFParticleToSpacePointAssoc         m_pfParticleSpacePointMap; ///<  The input associations: PFParticle -> SpacePoint
    PFParticleToClusterAssoc            m_pfParticleClusterMap;    ///<  The input associations: PFParticle -> Cluster
    PFParticleToVertexAssoc             m_pfParticleVertexMap;     ///<  The input associations: PFParticle -> Vertex
    PFParticleToSliceAssoc              m_pfParticleSliceMap;      ///<  The input associations: PFParticle -> Slice
    PFParticleToTrackAssoc              m_pfParticleTrackMap;      ///<  The input associations: PFParticle -> Track
    PFParticleToShowerAssoc             m_pfParticleShowerMap;     ///<  The input associations: PFParticle -> Shower
    PFParticleToT0Assoc                 m_pfParticleT0Map;         ///<  The input associations: PFParticle -> T0
    PFParticleToPFParticleMetadataAssoc m_pfParticleMetadataMap;   ///<  The input associations: PFParticle -> Metadata
    PFParticleToPCAxisAssoc             m_pfParticlePCAxisMap;     ///<  The input associations: PFParticle -> PCAxis
    SpacePointToHitAssoc                m_spacePointHitMap;        ///<  The input associations: SpacePoint -> Hit
    ClusterToHitAssoc                   m_clusterHitMap;           ///<  The input associations: Cluster -> Hit
    SliceToHitAssoc                     m_sliceHitMap;             ///<  The input associations: Slice -> Hit
    TrackToHitAssoc                     m_trackHitMap;             ///<  The input associations: Track -> Hit
    ShowerToHitAssoc                    m_showerHitMap;            ///<  The input associations: Shower -> Hit
    ShowerToPCAxisAssoc                 m_showerPCAxisMap;         ///<  The input associations: PCAxis -> Shower
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline void LArPandoraEvent::GetCollection(const Labels::LabelType &inputLabel, Collection<T> &outputCollection) const
{
    const auto &handle(m_pEvent->getValidHandle<std::vector<T> >(m_labels.GetLabel(inputLabel)));

    for (unsigned int i = 0; i != handle->size(); i++)
        outputCollection.emplace_back(handle, i);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename L, typename R, typename D>
inline void LArPandoraEvent::GetAssociationMap(const Collection<L> &collectionL, const Labels::LabelType &inputLabel,
    Association<L, R, D> &outputAssociationMap) const
{
    const auto &assocHandle(m_pEvent->getValidHandle<art::Assns<L, R, D> >(m_labels.GetLabel(inputLabel)));
   
    // Check that there are no associaions from objects not in collectionL
    for (const auto &entry : *assocHandle)
    {
        auto it(std::find(collectionL.begin(), collectionL.end(), entry.first));
        if (it == collectionL.end())
            throw cet::exception("LArPandora") << " LArPandoraEvent::GetAssociationMap -- Found object in association that isn't in the supplied collection" << std::endl;
    }
    
    // Ensure there is an entry for every object of type L
    for (const auto &objectL : collectionL)
        outputAssociationMap[objectL];

    // Fill the association map
    for (const auto &entry : *assocHandle)
        outputAssociationMap.at(entry.first).emplace_back(entry.second, *entry.data);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename L, typename R>
inline void LArPandoraEvent::GetAssociationMap(const Collection<L> &collectionL, const Labels::LabelType &inputLabel,
    Association<L, R, void*> &outputAssociationMap) const
{
    const auto &assocHandle(m_pEvent->getValidHandle<art::Assns<L, R> >(m_labels.GetLabel(inputLabel)));
    
    // Check that there are no associaions from objects not in collectionL
    for (const auto &entry : *assocHandle)
    {
        auto it(std::find(collectionL.begin(), collectionL.end(), entry.first));
        if (it == collectionL.end())
            throw cet::exception("LArPandora") << " LArPandoraEvent::GetAssociationMap -- Found object in association that isn't in the supplied collection" << std::endl;
    }
    
    // Ensure there is an entry for every object of type L
    for (const auto &objectL : collectionL)
        outputAssociationMap[objectL];

    // Fill the association map
    for (const auto &entry : *assocHandle)
        outputAssociationMap.at(entry.first).emplace_back(entry.second, nullptr);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename L, typename R, typename D>
inline void LArPandoraEvent::CollectAssociated(const art::Ptr<L> &anObject, const Association<L, R, D> &associationLtoR,
    Collection<R> &associatedR) const
{
    if (associationLtoR.find(anObject) == associationLtoR.end())
        throw cet::exception("LArPandora") << " LArPandoraEvent::CollectAssociated -- Can not find association for object supplied." << std::endl;

    for (const auto &entry : associationLtoR.at(anObject))
    {
        // Ensure we don't repeat objects in the output collection
        if (std::find(associatedR.begin(), associatedR.end(), entry.first) == associatedR.end())
            associatedR.push_back(entry.first);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename L, typename R, typename D>
inline void LArPandoraEvent::GetFilteredAssociationMap(const Collection<L> &collectionL, const Collection<R> &collectionR,
    const Association<L, R, D> &inputAssociationLtoR, Association<L, R, D> &outputAssociationLtoR) const
{
    for (const auto &objectL : collectionL) 
    {
        if (inputAssociationLtoR.find(objectL) == inputAssociationLtoR.end())
            throw cet::exception("LArPandora") << " LArPandoraEvent::GetFilteredAssociationMap -- Can not find association for object in supplied collection." << std::endl;
        
        if (outputAssociationLtoR.find(objectL) != outputAssociationLtoR.end())
            throw cet::exception("LArPandora") << " LArPandoraEvent::GetFilteredAssociationMap -- Repeated objects in input collectionL" << std::endl;
        
        for (const auto &entry : inputAssociationLtoR.at(objectL)) 
        {
            if (std::find(collectionR.begin(), collectionR.end(), entry.first) == collectionR.end())
                continue;
       
            outputAssociationLtoR[objectL].push_back(entry);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline void LArPandoraEvent::WriteCollection(const Collection<T> &collection) const
{
    std::unique_ptr<std::vector<T> > output(new std::vector<T>);

    for (const auto &object : collection)
        output->push_back(*object);

    m_pEvent->put(std::move(output));
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename L, typename R, typename D>
inline void LArPandoraEvent::WriteAssociation(const Association<L, R, D> &associationMap, const Collection<L> &collectionL,
    const Collection<R> &collectionR, const bool thisProducesR) const
{
    // The output assocation to populate
    std::unique_ptr<art::Assns<L, R, D> > outputAssn(new art::Assns<L, R, D>);

    // NB. The art::Ptrs in the stored collections refer to the producer that originally created the objects (e.g. Pandora pat-rec). To make
    // correct associations, we need to make new art::Ptrs to refer to the *copies* of the objects made by this producer. This is done using
    // the PtrMaker utility.
    const art::PtrMaker<L> makePtrL(*m_pEvent);

    for (auto it = associationMap.begin(); it != associationMap.end(); ++it) 
    {
        const auto indexL(this->GetIndex(it->first, collectionL));
        const auto outputPtrL(makePtrL(indexL));
    
        for (const auto &entry : it->second)
        {
            const auto &objectR(entry.first);
            const auto &objectD(entry.second);

            if (thisProducesR)
            {
                const art::PtrMaker<R> makePtrR(*m_pEvent);
                const auto indexR(this->GetIndex(objectR, collectionR));
                outputAssn->addSingle(outputPtrL, makePtrR(indexR), objectD);
            }
            else
            {
               outputAssn->addSingle(outputPtrL, objectR, objectD);
            }
        }
    }

    m_pEvent->put(std::move(outputAssn));
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename L, typename R>
inline void LArPandoraEvent::WriteAssociation(const Association<L, R, void*> &associationMap, const Collection<L> &collectionL,
    const Collection<R> &collectionR, const bool thisProducesR) const
{
    // The output assocation to populate
    std::unique_ptr<art::Assns<L, R> > outputAssn(new art::Assns<L, R>);

    // NB. The art::Ptrs in the stored collections refer to the producer that originally created the objects (e.g. Pandora pat-rec). To make
    // correct associations, we need to make new art::Ptrs to refer to the *copies* of the objects made by this producer. This is done using
    // the PtrMaker utility.
    const art::PtrMaker<L> makePtrL(*m_pEvent);

    for (auto it = associationMap.begin(); it != associationMap.end(); ++it) 
    {
        const auto indexL(this->GetIndex(it->first, collectionL));
        const auto outputPtrL(makePtrL(indexL));
    
        for (const auto &entry : it->second)
        {
            const auto &objectR(entry.first);

            if (thisProducesR)
            {
                const art::PtrMaker<R> makePtrR(*m_pEvent);
                const auto indexR(this->GetIndex(objectR, collectionR));
                outputAssn->addSingle(outputPtrL, makePtrR(indexR));
            }
            else
            {
               outputAssn->addSingle(outputPtrL, objectR);
            }
        }
    }

    m_pEvent->put(std::move(outputAssn));
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline size_t LArPandoraEvent::GetIndex(const art::Ptr<T> object, const Collection<T> &collection) const
{
    const auto it(std::find(collection.begin(), collection.end(), object));
    if (it == collection.end()) 
        throw cet::exception("LArPandora") << " LArPandoraEvent::GetIndex -- Can't find input object in the supplied collection." << std::endl;
        
    return static_cast<size_t>(std::distance(collection.begin(), it));
}

} // namespace lar_pandora

#endif // #ifndef LAR_PANDORA_EVENT_H
