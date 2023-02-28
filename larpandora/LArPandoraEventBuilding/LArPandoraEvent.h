/**
 *  @file   larpandora/LArPandoraEventBuilding/LArPandoraEvent.h
 *
 *  @brief  A description of all outputs from an instance of pandora with functionality to filter and merge multiple output
 */

#ifndef LAR_PANDORA_EVENT_H
#define LAR_PANDORA_EVENT_H 1

#include "art/Persistency/Common/PtrMaker.h"

#include "lardata/Utilities/AssociationUtil.h"

#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/PCAxis.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/PFParticleMetadata.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Vertex.h"

#include "lardataobj/AnalysisBase/T0.h"

#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

#include <algorithm>
#include <map>
#include <memory>

namespace lar_pandora {

  typedef std::vector<art::Ptr<larpandoraobj::PFParticleMetadata>> MetadataVector;
  typedef std::map<art::Ptr<recob::PFParticle>,
                   std::vector<art::Ptr<larpandoraobj::PFParticleMetadata>>>
    PFParticlesToMetadata;

  typedef std::vector<art::Ptr<recob::PCAxis>> PCAxisVector;
  typedef std::map<art::Ptr<recob::PFParticle>, std::vector<art::Ptr<recob::PCAxis>>>
    PFParticlesToPCAxes;
  typedef std::map<art::Ptr<recob::PFParticle>, std::vector<art::Ptr<recob::PFParticle>>>
    PFParticlesToPFParticles;
  typedef std::map<art::Ptr<recob::Shower>, std::vector<art::Ptr<recob::PCAxis>>> ShowersToPCAxes;
  typedef std::map<art::Ptr<recob::SpacePoint>, std::vector<art::Ptr<recob::Hit>>>
    SpacePointsToHitVector;

  /**
 *  @brief LArPandoraEvent class
 */
  class LArPandoraEvent {
  public:
    /**
     *  @brief  Class to handle the required producer labels
     */
    class Labels {
    public:
      /**
         *  @brief  Label type enumeration
         */
      enum LabelType {
        PFParticleLabel,
        SpacePointLabel,
        ClusterLabel,
        VertexLabel,
        TrackLabel,
        ShowerLabel,
        T0Label,
        PFParticleMetadataLabel,
        PCAxisLabel,
        HitLabel,
        PFParticleToSpacePointLabel,
        PFParticleToClusterLabel,
        PFParticleToVertexLabel,
        PFParticleToTrackLabel,
        PFParticleToShowerLabel,
        PFParticleToT0Label,
        PFParticleToMetadataLabel,
        PFParticleToPCAxisLabel,
        SpacePointToHitLabel,
        ClusterToHitLabel,
        TrackToHitLabel,
        ShowerToHitLabel,
        ShowerToPCAxisLabel
      };

      /**
         *  @brief  Minimal parametrised constructor.
         *          Sets all collection labels to be the same as the PFParticle producer label
         */
      Labels(const std::string& pfParticleProducerLabel, const std::string& hitProducerLabel);

      /**
         *  @brief  Track / Shower parametrised constructor.
         *          Sets all collection labels to be the same as the PFParticle producer label,
         *          except those relating to track and shower production, which are supplied.
         */
      Labels(const std::string& pfParticleProducerLabel,
             const std::string& trackProducerLabel,
             const std::string& showerProducerLabel,
             const std::string& hitProducerLabel);

      const std::string& GetLabel(const LabelType& type) const;

      void SetSpacePointProducerLabel(const std::string& label);
      void SetClusterProducerLabel(const std::string& label);
      void SetVertexProducerLabel(const std::string& label);
      void SetTrackProducerLabel(const std::string& label);
      void SetShowerProducerLabel(const std::string& label);
      void SetT0ProducerLabel(const std::string& label);
      void SetMetadataProducerLabel(const std::string& label);
      void SetPCAxisProducerLabel(const std::string& label);

      void SetPFParticleToSpacePointProducerLabel(const std::string& label);
      void SetPFParticleToClusterProducerLabel(const std::string& label);
      void SetPFParticleToVertexProducerLabel(const std::string& label);
      void SetPFParticleToTrackProducerLabel(const std::string& label);
      void SetPFParticleToShowerProducerLabel(const std::string& label);
      void SetPFParticleToT0ProducerLabel(const std::string& label);
      void SetPFParticleToMetadataProducerLabel(const std::string& label);
      void SetPFParticleToPCAxisProducerLabel(const std::string& label);
      void SetSpacePointToHitProducerLabel(const std::string& label);
      void SetClusterToHitProducerLabel(const std::string& label);
      void SetTrackToHitProducerLabel(const std::string& label);
      void SetShowerToHitProducerLabel(const std::string& label);
      void SetShowerToPCAxisProducerLabel(const std::string& label);

    private:
      std::map<LabelType, std::string> m_labels; ///< Map holding the labels
    };

    /**
     *  @brief  Constructor from an art::Event
     *
     *  @param  pProducer pointer to the producer to write the output
     *  @param  pEvent pointer to the event to process
     *  @param  inputLabel labels for the producers of the input collections
     *  @param  shouldProduceT0s if T0s should be produced (usually only for multiple drift volume use cases)
     *  @param  shift amount by which to shift PFParticle IDs when merging
     */
    LArPandoraEvent(art::EDProducer* pProducer,
                    art::Event* pEvent,
                    const Labels& inputLabels,
                    const bool shouldProduceT0s = false,
                    const size_t shift = 100000);

    /**
     *  @brief  Construct by copying an existing LArPandoraEvent, replacing the collections and associations
     *          by any objects associated with a PFParticle in the selection supplied.
     *
     *  @param  event input event to copy and filter
     *  @param  pfParticleVector input vector of selected particles
     */
    LArPandoraEvent(const LArPandoraEvent& event, const PFParticleVector& selectedPFParticles);

    /**
     *  @brief  Produce a copy of the event keeping only the collections that are associated with a top-level particle whose Pdg code
     *          is a neutrino (non-neutrino) if shouldProduceNeutrinos is set to true (false)
     *
     *  @param  shouldProduceNeutrinos if the returned event should contain neutrinos (or non-neutrinos)
     */
    LArPandoraEvent FilterByPdgCode(const bool shouldProduceNeutrinos) const;

    /**
     *  @brief  Produce a copy of the event keeping only the collections that are associated with a top-level particle that is not
     *          tagged as a neutrino (non-neutrino) if shouldProduceNeutrinos is set to true (false)
     *
     *  @param  shouldProduceNeutrinos if the returned event should contain neutrinos (or non-neutrinos)
     *  @param  tagProducerLabel label for the producer of the CRTags
     */
    LArPandoraEvent FilterByCRTag(const bool shouldProduceNeutrinos,
                                  const std::string& tagProducerLabel) const;

    /**
     *  @brief  Write (put) the collections in this LArPandoraEvent to the art::Event
     */
    void WriteToEvent() const;

    /**
     *  @brief  Merge collections from two events into one
     */
    LArPandoraEvent Merge(const LArPandoraEvent& other) const;

  private:
    /**
     *  @brief pdg enumeration
     */
    enum Pdg { nue = 12, numu = 14, nutau = 16 };

    /**
     *  @brief  Get the collections and associations from m_pEvent with the required labels
     */
    void GetCollections();

    /**
     *  @brief  Gets a given collection from m_pEvent with the label supplied
     *
     *  @param  inputLabel a label for the producer of the collection required
     *  @param  outputHandle the output art Handle to required collection
     *  @param  outputCollection the required collection
     */
    template <typename T>
    void GetCollection(const Labels::LabelType& inputLabel,
                       art::Handle<std::vector<T>>& outputHandle,
                       std::vector<art::Ptr<T>>& outputCollection) const;

    /**
     *  @brief  Get the mapping between two collections using the specified label
     *
     *  @param  inputLabel a label for the producer of the association required
     *  @param  inputHandleT the input art Handle to the first collection
     *  @param  outputAssociationMap output mapping between the two data types supplied (T -> U)
     */
    template <typename T, typename U>
    void GetAssociationMap(
      const Labels::LabelType& inputLabel,
      art::Handle<std::vector<T>>& inputHandleT,
      std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& outputAssociationMap) const;

    /**
     *  @brief  Get the mapping from PFParticles to their daughters
     */
    void GetPFParticleHierarchy();

    /**
     *  @brief  Filters primary PFParticles from the m_pfParticles
     *
     *  @param  primaryPFParticles output vector of all primary PFParticles in the input vector
     */
    void GetPrimaryPFParticles(PFParticleVector& primaryPFParticles) const;

    /**
     *  @brief  Filters PFParticles based on their Pdg from the inputPFParticles
     *
     *  @param  shouldProduceNeutrinos if the filtered particle vector should contain neutrinos (or non-neutrinos)
     *  @param  inputPFParticles input vector of PFParticles
     *  @param  filteredPFParticles output vector of filtered PFParticles
     */
    void GetFilteredParticlesByPdgCode(const bool shouldProduceNeutrinos,
                                       const PFParticleVector& inputPFParticles,
                                       PFParticleVector& outputPFParticles) const;

    /**
     *  @brief  Filters PFParticles based on their Pdg from the inputPFParticles
     *
     *  @param  shouldProduceNeutrinos if the filtered particle vector should contain neutrinos (or non-neutrinos)
     *  @param  tagProducerLabel the label for the producer of the CR tags
     *  @param  inputPFParticles input vector of PFParticles
     *  @param  filteredPFParticles output vector of filtered PFParticles
     */
    void GetFilteredParticlesByCRTag(const bool shouldProduceNeutrinos,
                                     const std::string& tagProducerLabel,
                                     const PFParticleVector& inputPFParticles,
                                     PFParticleVector& outputPFParticles) const;

    /**
     *  @brief  Filters the hierarchy map using a given vector of filteredParticles
     *
     *  @param  filteredParticles input PFParticles that have already been filtered
     *  @param  unfilteredPFParticleDaughterMap input hierarchy map to filter
     *  @param  outputPFParticleDaughterMap output filtered hierarchy map
     */
    void GetFilteredHierarchyMap(const PFParticleVector& filteredParticles,
                                 const PFParticlesToPFParticles& unfilteredPFParticleDaughterMap,
                                 PFParticlesToPFParticles& outputPFParticleDaughterMap) const;

    /**
     *  @brief  Produce a mapping between PFParticles and their ID
     *
     *  @param  idToPFParticleMap output mapping between PFParticles and their IDs
     */
    void GetIdToPFParticleMap(
      std::map<size_t, art::Ptr<recob::PFParticle>>& idToPFParticleMap) const;

    /**
     *  @brief  Get particles downstream of any particle in an input vector
     *
     *  @param  inputPFParticles input vector of PFParticles
     *  @param  downstreamPFParticles output vector of PFParticles downstream of those in the input vector
     */
    void GetDownstreamPFParticles(const PFParticleVector& inputPFParticles,
                                  PFParticleVector& downstreamPFParticles) const;

    /**
     *  @brief  Get particles downstream of a supplied particle
     *
     *  @param  part input PFParticles
     *  @param  downstreamPFParticles output vector of PFParticles downstream of part
     */
    void GetDownstreamPFParticles(const art::Ptr<recob::PFParticle>& part,
                                  PFParticleVector& downstreamPFParticles) const;

    /**
     *  @brief  Fills the PFParticleToOriginIdMap using an existing map from another LArPandoraEvent
     *
     *  @param  existingMap input map from PFParticles to origin IDs
     */
    void FillPFParticleToOriginIdMap(
      const std::map<art::Ptr<recob::PFParticle>, unsigned int>& existingMap);

    /**
     *  @brief  Collects all objects of type U associated to a given object of type T
     *
     *  @param  anObject an input object of type T with which we want to collect associated objects of type U
     *  @param  associationTtoU the general input association between objects of type U and T
     *  @param  associatedU output vector of objects of type U associated with anObject
     */
    template <typename T, typename U>
    void CollectAssociated(const art::Ptr<T>& anObject,
                           const std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& associationTtoU,
                           std::vector<art::Ptr<U>>& associatedU) const;

    /**
     *   @brief  Gets the mapping between two filtered collections
     *
     *   @param  collectionT a first filtered collection
     *   @param  collectionU a second filtered collection
     *   @param  inputAssociationTtoU mapping between the two unfiltered collections
     *   @param  outputAssociationTtoU mapping between the two filtered collections
     *
     *   @return mapping between the filtered collections
     */
    template <typename T, typename U>
    void GetFilteredAssociationMap(
      const std::vector<art::Ptr<T>>& collectionT,
      const std::vector<art::Ptr<U>>& collectionU,
      const std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& inputAssociationTtoU,
      std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& outputAssociationTtoU) const;

    /**
     *  @brief  Write a given collection to the event
     *
     *  @param  collection the collection to write
     */
    template <typename T>
    void WriteCollection(const std::vector<art::Ptr<T>>& collection) const;

    /**
     *  @brief  Write a given association to the event
     *
     *  @param  associationMap the association to write from objects of type T -> U
     *  @param  collectionT the collection of type T that has been written
     *  @param  collectionU the collection of type U that has been written
     *  @param  thisProducesU will this producer produce collectionU of was it produced by a different module?
     */
    template <typename T, typename U>
    void WriteAssociation(const std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& associationMap,
                          const std::vector<art::Ptr<T>>& collectionT,
                          const std::vector<art::Ptr<U>>& collectionU,
                          const bool thisProducesU = true) const;

    /**
     *  @brief  Merge two PFParticle to origin ID maps ensuring no ID collisions
     *
     *  @param  mapToMerge the map to accept the merge
     *  @param  mapToAdd the map to append to the map to merge
     */
    void MergePFParticleToOriginIdMap(
      std::map<art::Ptr<recob::PFParticle>, unsigned int>& mapToMerge,
      const std::map<art::Ptr<recob::PFParticle>, unsigned int>& mapToAdd) const;

    /**
     *  @brief  Append a collection onto an other collection
     *
     *  @param  collectionToMerge the collection to accept
     *  @param  collection the collection to append
     */
    template <typename T>
    void MergeCollection(std::vector<art::Ptr<T>>& collectionToMerge,
                         const std::vector<art::Ptr<T>>& collection) const;

    /**
     *  @brief  Append an association to another association
     *
     *  @param  associationToMerge the association to accept
     *  @param  association the association to append
     */
    template <typename T, typename U>
    void MergeAssociation(std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& associationToMerge,
                          const std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& association) const;

    art::EDProducer*
      m_pProducer; ///<  The producer which should write the output collections and associations
    art::Event* m_pEvent; ///<  The event to consider
    Labels m_labels;      ///<  A set of labels describing the producers for each input collection

    std::map<art::Ptr<recob::PFParticle>, unsigned int>
      m_pfParticleToOriginIdMap; ///< Mapping between PFParticles, and an ID for the LArPandoraEvent from which they originated (to keep track of merges)

    bool
      m_shouldProduceT0s; ///<  If T0s should be produced (usually only true for use cases with multiple drift volumes)
    const size_t
      m_shift; ///<  Amount by which to shift PFParticle IDs when merging two reconstructions of the same event

    // Collections
    PFParticleVector m_pfParticles; ///<  The input collection of PFParticles
    SpacePointVector m_spacePoints; ///<  The input collection of SpacePoints
    ClusterVector m_clusters;       ///<  The input collection of Clusters
    VertexVector m_vertices;        ///<  The input collection of Vertices
    TrackVector m_tracks;           ///<  The input collection of Tracks
    ShowerVector m_showers;         ///<  The input collection of Showers
    T0Vector m_t0s;                 ///<  The input collection of T0s
    MetadataVector m_metadata;      ///<  The input collection of PFParticle metadata
    PCAxisVector m_pcAxes;          ///<  The input collection of PCAxes
    HitVector m_hits;               ///<  The input collection of Hits

    // Association maps
    PFParticlesToSpacePoints
      m_pfParticleSpacePointMap; ///<  The input associations: PFParticle -> SpacePoint
    PFParticlesToClusters
      m_pfParticleClusterMap;                    ///<  The input associations: PFParticle -> Cluster
    PFParticlesToVertices m_pfParticleVertexMap; ///<  The input associations: PFParticle -> Vertex
    PFParticlesToTracks m_pfParticleTrackMap;    ///<  The input associations: PFParticle -> Track
    PFParticlesToShowers m_pfParticleShowerMap;  ///<  The input associations: PFParticle -> Shower
    PFParticlesToT0s m_pfParticleT0Map;          ///<  The input associations: PFParticle -> T0
    PFParticlesToMetadata
      m_pfParticleMetadataMap;                 ///<  The input associations: PFParticle -> Metadata
    PFParticlesToPCAxes m_pfParticlePCAxisMap; ///<  The input associations: PFParticle -> PCAxis

    SpacePointsToHitVector m_spacePointHitMap; ///<  The input associations: SpacePoint -> Hit
    ClustersToHits m_clusterHitMap;            ///<  The input associations: Cluster -> Hit
    TracksToHits m_trackHitMap;                ///<  The input associations: Track -> Hit
    ShowersToHits m_showerHitMap;              ///<  The input associations: Shower -> Hit

    ShowersToPCAxes m_showerPCAxisMap; ///<  The input associations: PCAxis -> Shower

    PFParticlesToPFParticles
      m_pfParticleDaughterMap; ///<  The mapping from parent to daughter PFParticles
  };

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline void LArPandoraEvent::GetCollection(const Labels::LabelType& inputLabel,
                                             art::Handle<std::vector<T>>& outputHandle,
                                             std::vector<art::Ptr<T>>& outputCollection) const
  {
    m_pEvent->getByLabel(m_labels.GetLabel(inputLabel), outputHandle);

    for (unsigned int i = 0; i != outputHandle->size(); i++) {
      art::Ptr<T> object(outputHandle, i);
      outputCollection.push_back(object);
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, typename U>
  inline void LArPandoraEvent::GetAssociationMap(
    const Labels::LabelType& inputLabel,
    art::Handle<std::vector<T>>& inputHandleT,
    std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& outputAssociationMap) const
  {
    art::FindManyP<U> assoc(inputHandleT, (*m_pEvent), m_labels.GetLabel(inputLabel));

    for (unsigned int iT = 0; iT < inputHandleT->size(); iT++) {
      art::Ptr<T> objectT(inputHandleT, iT);

      if (outputAssociationMap.find(objectT) == outputAssociationMap.end()) {
        std::vector<art::Ptr<U>> emptyVect;
        outputAssociationMap.insert(
          typename std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>::value_type(objectT, emptyVect));
      }

      for (art::Ptr<U> objectU : assoc.at(objectT.key()))
        outputAssociationMap[objectT].push_back(objectU);
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, typename U>
  inline void LArPandoraEvent::CollectAssociated(
    const art::Ptr<T>& anObject,
    const std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& associationTtoU,
    std::vector<art::Ptr<U>>& associatedU) const
  {
    if (associationTtoU.find(anObject) == associationTtoU.end())
      throw cet::exception("LArPandora")
        << " LArPandoraEvent::CollectAssociated -- Can not find association for object supplied."
        << std::endl;

    std::vector<art::Ptr<U>> associatedObjects = associationTtoU.at(anObject);
    associatedU.insert(associatedU.end(), associatedObjects.begin(), associatedObjects.end());
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, typename U>
  inline void LArPandoraEvent::GetFilteredAssociationMap(
    const std::vector<art::Ptr<T>>& collectionT,
    const std::vector<art::Ptr<U>>& collectionU,
    const std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& inputAssociationTtoU,
    std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& outputAssociationTtoU) const
  {
    for (art::Ptr<T> objectT : collectionT) {
      std::vector<art::Ptr<U>> emptyVector;
      if (!outputAssociationTtoU
             .insert(typename std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>::value_type(
               objectT, emptyVector))
             .second)
        throw cet::exception("LArPandora")
          << " LArPandoraEvent::GetFilteredAssociationMap -- Can not have multiple association map "
             "entries for a single object."
          << std::endl;

      for (art::Ptr<U> objectU : inputAssociationTtoU.at(objectT)) {
        typename std::vector<art::Ptr<U>>::const_iterator associatedObjectIter =
          std::find(collectionU.begin(), collectionU.end(), objectU);
        if (associatedObjectIter == collectionU.end()) continue;

        outputAssociationTtoU[objectT].push_back(objectU);
      }
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline void LArPandoraEvent::WriteCollection(const std::vector<art::Ptr<T>>& collection) const
  {
    std::unique_ptr<std::vector<T>> output(new std::vector<T>);

    for (art::Ptr<T> object : collection)
      output->push_back(*object);

    m_pEvent->put(std::move(output));
  }

  template <>
  inline void LArPandoraEvent::WriteCollection(
    const std::vector<art::Ptr<recob::PFParticle>>& collection) const
  {
    std::unique_ptr<std::vector<recob::PFParticle>> output(new std::vector<recob::PFParticle>);

    for (art::Ptr<recob::PFParticle> part : collection) {

      if (part->Self() >= m_shift)
        throw cet::exception("LArPandora")
          << " LArPandoraEvent::WriteCollection -- PFParticle ID exceeds shift value of " << m_shift
          << ". Can't merge the collections!" << std::endl;

      if (m_pfParticleToOriginIdMap.find(part) == m_pfParticleToOriginIdMap.end())
        throw cet::exception("LArPandora")
          << " LArPandoraEvent::WriteCollection -- Can't find supplied PFParticle in the "
             "PFParticle to origin ID map."
          << std::endl;

      const size_t offset(m_shift * m_pfParticleToOriginIdMap.at(part));
      const size_t adjustedSelf(part->Self() + offset);

      size_t adjustedParent = part->Parent();
      if (part->Parent() != recob::PFParticle::kPFParticlePrimary) adjustedParent += offset;

      const std::vector<size_t>& daughters(part->Daughters());
      std::vector<size_t> adjustedDaughters;
      for (unsigned int d = 0; d < daughters.size(); d++)
        adjustedDaughters.push_back(daughters[d] + offset);

      recob::PFParticle adjustedPart(
        part->PdgCode(), adjustedSelf, adjustedParent, adjustedDaughters);
      output->push_back(adjustedPart);
    }

    m_pEvent->put(std::move(output));
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, typename U>
  inline void LArPandoraEvent::WriteAssociation(
    const std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& associationMap,
    const std::vector<art::Ptr<T>>& collectionT,
    const std::vector<art::Ptr<U>>& collectionU,
    const bool thisProducesU) const
  {
    const art::PtrMaker<T> makePtrT(*m_pEvent);
    std::unique_ptr<art::Assns<T, U>> outputAssn(new art::Assns<T, U>);

    for (typename std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>::const_iterator it =
           associationMap.begin();
         it != associationMap.end();
         ++it) {
      typename std::vector<art::Ptr<T>>::const_iterator itT =
        std::find(collectionT.begin(), collectionT.end(), it->first);
      if (itT == collectionT.end())
        throw cet::exception("LArPandora") << " LArPandoraEvent::WriteAssociation -- association "
                                              "map contains object not in collectionT."
                                           << std::endl;

      art::Ptr<T> newObjectT(makePtrT(std::distance(collectionT.begin(), itT)));

      for (const art::Ptr<U>& objectU : it->second) {
        if (thisProducesU) {
          const art::PtrMaker<U> makePtrU(*m_pEvent);

          typename std::vector<art::Ptr<U>>::const_iterator itU =
            std::find(collectionU.begin(), collectionU.end(), objectU);
          if (itU == collectionU.end())
            throw cet::exception("LArPandora")
              << " LArPandoraEvent::WriteAssociation -- association map contains object not in "
                 "collectionU."
              << std::endl;

          art::Ptr<U> newObjectU(makePtrU(std::distance(collectionU.begin(), itU)));
          util::CreateAssn(*m_pProducer, *m_pEvent, newObjectU, newObjectT, *outputAssn);
        }
        else {
          util::CreateAssn(*m_pProducer, *m_pEvent, objectU, newObjectT, *outputAssn);
        }
      }
    }

    m_pEvent->put(std::move(outputAssn));
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T>
  inline void LArPandoraEvent::MergeCollection(std::vector<art::Ptr<T>>& collectionToMerge,
                                               const std::vector<art::Ptr<T>>& collection) const
  {
    collectionToMerge.insert(collectionToMerge.end(), collection.begin(), collection.end());
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <typename T, typename U>
  inline void LArPandoraEvent::MergeAssociation(
    std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& associationToMerge,
    const std::map<art::Ptr<T>, std::vector<art::Ptr<U>>>& association) const
  {
    associationToMerge.insert(association.begin(), association.end());
  }

} // namespace lar_pandora

#endif // #ifndef LAR_PANDORA_EVENT_H
