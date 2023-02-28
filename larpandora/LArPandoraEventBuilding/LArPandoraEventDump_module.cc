/**
 *  @file   larpandora/LArPandoraEventBuilding/LArPandoraEventDump.cc
 *
 *  @brief  module for lar pandora event dump
 */

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "fhiclcpp/ParameterSet.h"

#include "lardata/Utilities/AssociationUtil.h"

#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/PCAxis.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/PFParticleMetadata.h"
#include "lardataobj/RecoBase/Seed.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Slice.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Vertex.h"

#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

namespace lar_pandora {

  class LArPandoraEventDump : public art::EDAnalyzer {
  public:
    explicit LArPandoraEventDump(fhicl::ParameterSet const& pset);

    LArPandoraEventDump(LArPandoraEventDump const&) = delete;
    LArPandoraEventDump(LArPandoraEventDump&&) = delete;
    LArPandoraEventDump& operator=(LArPandoraEventDump const&) = delete;
    LArPandoraEventDump& operator=(LArPandoraEventDump&&) = delete;

    void analyze(art::Event const& evt) override;

  private:
    template <class T>
    using Collection = art::Handle<std::vector<T>>;

    template <class T>
    using Association = art::FindManyP<T>;

    /**
     *  @brief  Class holding the handle for all of the data types from Pandora
     */
    class PandoraData {
    public:
      /**
         *  @brief  Default constructor
         *
         *  @param  evt the art event
         *  @param  pandoraLabel the pandora producer label
         *  @param  trackLabel the track producer label (optional)
         *  @param  showerLabel the shower producer label (optional)
         */
      PandoraData(const art::Event& evt,
                  const std::string& pandoraLabel,
                  const std::string& trackLabel = "",
                  const std::string& showerLabel = "");

      /**
         *  @brief  Default destructor
         */
      ~PandoraData();

      // Collections
      Collection<recob::PFParticle> m_pfParticleCollection; ///< The PFParticle handle
      Collection<larpandoraobj::PFParticleMetadata>
        m_pfParticleMetadataCollection;                     ///< The PFParticleMetadata handle
      Collection<recob::Cluster> m_clusterCollection;       ///< The Cluster handle
      Collection<recob::SpacePoint> m_spacePointCollection; ///< The SpacePoint handle
      Collection<recob::Vertex> m_vertexCollection;         ///< The Vertex handle
      Collection<recob::Track> m_trackCollection;           ///< The Track handle
      Collection<recob::Shower> m_showerCollection;         ///< The Shower handle
      Collection<recob::PCAxis> m_pcAxisCollection;         ///< The PCAxis handle
      Collection<recob::Slice> m_sliceCollection;           ///< The Slice handle

      // Associations
      Association<larpandoraobj::PFParticleMetadata>*
        m_pPFParticleToMetadataAssociation; ///< The PFParticle to metadata association
      Association<recob::Cluster>*
        m_pPFParticleToClusterAssociation; ///< The PFParticle to cluster association
      Association<recob::SpacePoint>*
        m_pPFParticleToSpacePointAssociation; ///< The PFParticle to space point association
      Association<recob::Vertex>*
        m_pPFParticleToVertexAssociation; ///< The PFParticle to vertex association
      Association<recob::Track>*
        m_pPFParticleToTrackAssociation; ///< The PFParticle to track association
      Association<recob::Shower>*
        m_pPFParticleToShowerAssociation; ///< The PFParticle to shower association
      Association<recob::Slice>*
        m_pPFParticleToSliceAssociation; ///< The PFParticle to slice association

      Association<recob::Hit>* m_pClusterToHitAssociation;    ///< The Cluster to hit association
      Association<recob::Hit>* m_pSpacePointToHitAssociation; ///< The SpacePoint to hit association
      Association<recob::Hit>* m_pTrackToHitAssociation;      ///< The Track to hit association
      Association<recob::Hit>* m_pShowerToHitAssociation;     ///< The Shower to hit association
      Association<recob::Hit>* m_pSliceToHitAssociation;      ///< The Slice to hit association

      Association<recob::PCAxis>*
        m_pShowerToPCAxisAssociation; ///< The Shower to PCAxis association

    private:
      /**
         *  @brief  Load a collection from the event
         *
         *  @param  evt the art event
         *  @param  label the producer label
         *  @param  collection the output collection
         */
      template <class T>
      void LoadCollection(const art::Event& evt,
                          const std::string& label,
                          Collection<T>& collection);

      /**
         *  @brief  Load an association from the event
         *
         *  @param  evt the art event
         *  @param  label the producer label
         *  @param  collection the input collection (from which object are associated)
         *  @param  pAssociation the output association
         */
      template <class T, class U>
      void LoadAssociation(const art::Event& evt,
                           const std::string& label,
                           const Collection<T>& collection,
                           Association<U>*& pAssociation);
    };

    // -------------------------------------------------------------------------------------------------------------------------------------

    /**
     *  @brief  Print the metadata about the event
     *
     *  @param  evt the art event
     */
    void PrintEventMetadata(const art::Event& evt) const;

    /**
     *  @brief  Print a summary of the event similar to the standard event dump
     *
     *  @param  data the pandora collections and associations
     */
    void PrintEventSummary(const PandoraData& data) const;

    /**
     *  @brief  Print the full PFParticle Hierarchy
     *
     *  @param  data the pandora collections and associations
     */
    void PrintPFParticleHierarchy(const PandoraData& data) const;

    /**
     *  @brief  Build the map from PFParticle ID to PFParticle from the input data
     *
     *  @param  data the pandora collections and associations
     *  @param  pfParticleMap the output PFParticle map
     */
    void BuildPFParticleMap(const PandoraData& data, PFParticleMap& pfParticleMap) const;

    /**
     *  @brief  Print a given PFParticle
     *
     *  @param  particle the particle to print
     *  @param  pfParticleMap the input mapping from PFParticle ID to PFParticle
     *  @param  data the pandora collections and associations
     *  @param  depth the number of characters to indent
     */
    void PrintParticle(const art::Ptr<recob::PFParticle>& particle,
                       const PFParticleMap& pfParticleMap,
                       const PandoraData& data,
                       const unsigned int depth) const;

    /**
     *  @brief  Print a given Hit
     *
     *  @param  hit the hit to print
     *  @param  depth the number of characters to indent
     */
    void PrintHit(const art::Ptr<recob::Hit>& hit, const unsigned int depth) const;

    /**
     *  @brief  Print a given Slice
     *
     *  @param  slice the slice to print
     *  @param  data the pandora collections and associations
     *  @param  depth the number of characters to indent
     */
    void PrintSlice(const art::Ptr<recob::Slice>& slice,
                    const PandoraData& data,
                    const unsigned int depth) const;

    /**
     *  @brief  Print a given Cluster
     *
     *  @param  cluster the cluster to print
     *  @param  data the pandora collections and associations
     *  @param  depth the number of characters to indent
     */
    void PrintCluster(const art::Ptr<recob::Cluster>& cluster,
                      const PandoraData& data,
                      const unsigned int depth) const;

    /**
     *  @brief  Print a given Vertex
     *
     *  @param  vertex the vertex to print
     *  @param  depth the number of characters to indent
     */
    void PrintVertex(const art::Ptr<recob::Vertex>& vertex, const unsigned int depth) const;

    /**
     *  @brief  Print a given SpacePoint
     *
     *  @param  spacePoint the spacePoint to print
     *  @param  data the pandora collections and associations
     *  @param  depth the number of characters to indent
     */
    void PrintSpacePoint(const art::Ptr<recob::SpacePoint>& spacePoint,
                         const PandoraData& data,
                         const unsigned int depth) const;

    /**
     *  @brief  Print a given Track
     *
     *  @param  track the track to print
     *  @param  data the pandora collections and associations
     *  @param  depth the number of characters to indent
     */
    void PrintTrack(const art::Ptr<recob::Track>& track,
                    const PandoraData& data,
                    const unsigned int depth) const;

    /**
     *  @brief  Print a given Shower
     *
     *  @param  shower the shower to print
     *  @param  data the pandora collections and associations
     *  @param  depth the number of characters to indent
     */
    void PrintShower(const art::Ptr<recob::Shower>& shower,
                     const PandoraData& data,
                     const unsigned int depth) const;

    /**
     *  @brief  Print a horizontal line
     *
     *  @param  depth the number of characters to indent
     */
    void PrintRule(const unsigned int depth) const;

    /**
     *  @brief  Print a title line
     *
     *  @param  name the title name
     *  @param  depth the number of characters to indent
     */
    void PrintTitle(const std::string& name, const unsigned int depth) const;

    /**
     *  @brief  Print a given property with the correct amount of whitespace
     *
     *  @param  name the property name
     *  @param  value the property value
     *  @param  depth the number of characters to indent
     */
    template <class T>
    void PrintProperty(const std::string& name, const T& value, const unsigned int depth) const;

    std::string m_verbosityLevel; ///< The level of verbosity to use
    std::string m_pandoraLabel;   ///< The label of the Pandora pattern recognition producer
    std::string m_trackLabel;     ///< The track producer label
    std::string m_showerLabel;    ///< The shower producer label
  };

  DEFINE_ART_MODULE(LArPandoraEventDump)

} // namespace lar_pandora

//------------------------------------------------------------------------------------------------------------------------------------------
// implementation follows

namespace lar_pandora {

  LArPandoraEventDump::LArPandoraEventDump(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
    , m_pandoraLabel(pset.get<std::string>("PandoraLabel"))
    , m_trackLabel(pset.get<std::string>("TrackLabel", ""))
    , m_showerLabel(pset.get<std::string>("ShowerLabel", ""))
  {
    m_verbosityLevel = pset.get<std::string>("VerbosityLevel");
    std::transform(
      m_verbosityLevel.begin(), m_verbosityLevel.end(), m_verbosityLevel.begin(), ::tolower);

    if (m_verbosityLevel != "brief" && m_verbosityLevel != "summary" &&
        m_verbosityLevel != "detailed" && m_verbosityLevel != "extreme") {
      throw cet::exception("LArPandoraEventDump")
        << "Unknown verbosity level: " << m_verbosityLevel << std::endl;
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::analyze(art::Event const& evt)
  {
    // Load the Pandora owned collections from the event
    PandoraData data(evt, m_pandoraLabel, m_trackLabel, m_showerLabel);

    this->PrintEventMetadata(evt);
    this->PrintEventSummary(data);

    if (m_verbosityLevel != "brief") this->PrintPFParticleHierarchy(data);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintEventMetadata(const art::Event& evt) const
  {
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "run    : " << evt.run() << std::endl;
    std::cout << "subRun : " << evt.subRun() << std::endl;
    std::cout << "event  : " << evt.event() << std::endl;
    std::cout << std::endl;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintEventSummary(const PandoraData& data) const
  {
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Collection sizes" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    std::cout << "PFParticle         : " << data.m_pfParticleCollection->size() << std::endl;
    std::cout << "PFParticleMetadata : " << data.m_pfParticleMetadataCollection->size()
              << std::endl;
    std::cout << "Cluster            : " << data.m_clusterCollection->size() << std::endl;
    std::cout << "SpacePoint         : " << data.m_spacePointCollection->size() << std::endl;
    std::cout << "Vertex             : " << data.m_vertexCollection->size() << std::endl;
    std::cout << "Track              : " << data.m_trackCollection->size() << std::endl;
    std::cout << "Shower             : " << data.m_showerCollection->size() << std::endl;
    std::cout << "PCAxis             : " << data.m_pcAxisCollection->size() << std::endl;
    std::cout << "Slice              : " << data.m_sliceCollection->size() << std::endl;
    std::cout << std::endl;

    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Association sizes" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    if (data.m_pPFParticleToMetadataAssociation)
      std::cout << "PFParticle -> Metadata   : " << data.m_pPFParticleToMetadataAssociation->size()
                << std::endl;

    if (data.m_pPFParticleToClusterAssociation)
      std::cout << "PFParticle -> Cluster    : " << data.m_pPFParticleToClusterAssociation->size()
                << std::endl;

    if (data.m_pPFParticleToSpacePointAssociation)
      std::cout << "PFParticle -> SpacePoint : "
                << data.m_pPFParticleToSpacePointAssociation->size() << std::endl;

    if (data.m_pPFParticleToVertexAssociation)
      std::cout << "PFParticle -> Vertex     : " << data.m_pPFParticleToVertexAssociation->size()
                << std::endl;

    if (data.m_pPFParticleToTrackAssociation)
      std::cout << "PFParticle -> Track      : " << data.m_pPFParticleToTrackAssociation->size()
                << std::endl;

    if (data.m_pPFParticleToShowerAssociation)
      std::cout << "PFParticle -> Shower     : " << data.m_pPFParticleToShowerAssociation->size()
                << std::endl;

    if (data.m_pPFParticleToSliceAssociation)
      std::cout << "PFParticle -> Slice      : " << data.m_pPFParticleToSliceAssociation->size()
                << std::endl;

    if (data.m_pClusterToHitAssociation)
      std::cout << "Cluster    -> Hit        : " << data.m_pClusterToHitAssociation->size()
                << std::endl;

    if (data.m_pSpacePointToHitAssociation)
      std::cout << "SpacePoint -> Hit        : " << data.m_pSpacePointToHitAssociation->size()
                << std::endl;

    if (data.m_pTrackToHitAssociation)
      std::cout << "Track      -> Hit        : " << data.m_pTrackToHitAssociation->size()
                << std::endl;

    if (data.m_pShowerToHitAssociation)
      std::cout << "Shower     -> Hit        : " << data.m_pShowerToHitAssociation->size()
                << std::endl;

    if (data.m_pShowerToPCAxisAssociation)
      std::cout << "Shower     -> PCAxis     : " << data.m_pShowerToPCAxisAssociation->size()
                << std::endl;

    if (data.m_pSliceToHitAssociation)
      std::cout << "Slice      -> Hit        : " << data.m_pSliceToHitAssociation->size()
                << std::endl;

    std::cout << std::endl;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintPFParticleHierarchy(const PandoraData& data) const
  {
    // Get the mapping from PFParticle ID to PFParticle
    PFParticleMap pfParticleMap;
    this->BuildPFParticleMap(data, pfParticleMap);

    // Print all primary PFParticles
    for (unsigned int i = 0; i < data.m_pfParticleCollection->size(); ++i) {
      const art::Ptr<recob::PFParticle> particle(data.m_pfParticleCollection, i);

      if (!particle->IsPrimary()) continue;

      this->PrintParticle(particle, pfParticleMap, data, 0);
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::BuildPFParticleMap(const PandoraData& data,
                                               PFParticleMap& pfParticleMap) const
  {
    for (unsigned int i = 0; i < data.m_pfParticleCollection->size(); ++i) {
      const art::Ptr<recob::PFParticle> particle(data.m_pfParticleCollection, i);
      pfParticleMap[particle->Self()] = particle;
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintParticle(const art::Ptr<recob::PFParticle>& particle,
                                          const PFParticleMap& pfParticleMap,
                                          const PandoraData& data,
                                          const unsigned int depth) const
  {
    this->PrintRule(depth);
    this->PrintTitle("PFParticle", depth);
    this->PrintRule(depth);

    // Print the PFParticle details
    this->PrintProperty("Key", particle.key(), depth);
    this->PrintProperty("Id", particle->Self(), depth);
    this->PrintProperty("PDG", particle->PdgCode(), depth);
    this->PrintProperty("IsPrimary", particle->IsPrimary(), depth);

    if (!particle->IsPrimary()) this->PrintProperty("Parent", particle->Parent(), depth);

    // Print the metadata
    if (data.m_pPFParticleToMetadataAssociation) {
      const auto& metadata(data.m_pPFParticleToMetadataAssociation->at(particle.key()));
      this->PrintProperty("# Metadata", metadata.size(), depth);

      for (const auto& metadatum : metadata) {
        const auto& propertiesMap(metadatum->GetPropertiesMap());
        this->PrintProperty("# Properties", propertiesMap.size(), depth + 2);

        for (const auto& propertiesMapEntry : propertiesMap)
          this->PrintProperty(propertiesMapEntry.first, propertiesMapEntry.second, depth + 4);
      }
    }

    // Print the slices
    if (data.m_pPFParticleToSliceAssociation) {
      const auto& slices(data.m_pPFParticleToSliceAssociation->at(particle.key()));
      this->PrintProperty("# Slices", slices.size(), depth);

      if (m_verbosityLevel != "summary") {
        for (const auto& slice : slices)
          this->PrintSlice(slice, data, depth + 2);
      }
    }

    // Print the clusters
    if (data.m_pPFParticleToClusterAssociation) {
      const auto& clusters(data.m_pPFParticleToClusterAssociation->at(particle.key()));
      this->PrintProperty("# Clusters", clusters.size(), depth);

      if (m_verbosityLevel != "summary") {
        for (const auto& cluster : clusters)
          this->PrintCluster(cluster, data, depth + 2);
      }
    }

    // Print the space points
    if (data.m_pPFParticleToSpacePointAssociation) {
      const auto& spacePoints(data.m_pPFParticleToSpacePointAssociation->at(particle.key()));
      this->PrintProperty("# SpacePoints", spacePoints.size(), depth);

      if (m_verbosityLevel != "summary") {
        for (const auto& spacePoint : spacePoints)
          this->PrintSpacePoint(spacePoint, data, depth + 2);
      }
    }

    // Print the vertices
    if (data.m_pPFParticleToVertexAssociation) {
      const auto& vertices(data.m_pPFParticleToVertexAssociation->at(particle.key()));
      this->PrintProperty("# Vertices", vertices.size(), depth);

      if (m_verbosityLevel != "summary") {
        for (const auto& vertex : vertices)
          this->PrintVertex(vertex, depth + 2);
      }
    }

    // Print the tracks
    if (data.m_pPFParticleToTrackAssociation) {
      const auto& tracks(data.m_pPFParticleToTrackAssociation->at(particle.key()));
      this->PrintProperty("# Tracks", tracks.size(), depth);

      if (m_verbosityLevel != "summary") {
        for (const auto& track : tracks)
          this->PrintTrack(track, data, depth + 2);
      }
    }

    // Print the showers
    if (data.m_pPFParticleToShowerAssociation) {
      const auto& showers(data.m_pPFParticleToShowerAssociation->at(particle.key()));
      this->PrintProperty("# Showers", showers.size(), depth);

      if (m_verbosityLevel != "summary") {
        for (const auto& shower : showers)
          this->PrintShower(shower, data, depth + 2);
      }
    }

    // Print the daughters
    this->PrintProperty("# Daughters", particle->NumDaughters(), depth);
    this->PrintRule(depth);

    for (auto& daughterId : particle->Daughters()) {
      const auto daughterIter(pfParticleMap.find(daughterId));

      if (daughterIter == pfParticleMap.end())
        throw cet::exception("LArPandoraEventDump")
          << "Couldn't find daughter of PFParticle in the PFParticle map";

      const auto& daughter(daughterIter->second);
      this->PrintParticle(daughter, pfParticleMap, data, depth + 4);
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintHit(const art::Ptr<recob::Hit>& hit,
                                     const unsigned int depth) const
  {
    this->PrintTitle("Hit", depth);
    this->PrintProperty("Key", hit.key(), depth + 2);
    this->PrintProperty("Channel", hit->Channel(), depth + 2);
    this->PrintProperty("View", hit->View(), depth + 2);
    this->PrintProperty("Peak time", hit->PeakTime(), depth + 2);
    this->PrintProperty("RMS", hit->RMS(), depth + 2);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintSlice(const art::Ptr<recob::Slice>& slice,
                                       const PandoraData& data,
                                       const unsigned int depth) const
  {
    this->PrintTitle("Slice", depth);
    this->PrintProperty("Key", slice.key(), depth + 2);
    this->PrintProperty("ID", slice->ID(), depth + 2);

    if (!data.m_pSliceToHitAssociation) return;

    const auto& hits(data.m_pSliceToHitAssociation->at(slice.key()));
    this->PrintProperty("# Hits", hits.size(), depth + 2);

    if (m_verbosityLevel != "extreme") return;

    // Print each associated hit
    for (const auto& hit : hits)
      this->PrintHit(hit, depth + 4);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintCluster(const art::Ptr<recob::Cluster>& cluster,
                                         const PandoraData& data,
                                         const unsigned int depth) const
  {
    this->PrintTitle("Cluster", depth);
    this->PrintProperty("Key", cluster.key(), depth + 2);
    this->PrintProperty("ID", cluster->ID(), depth + 2);
    this->PrintProperty("View", cluster->View(), depth + 2);

    if (!data.m_pClusterToHitAssociation) return;

    const auto& hits(data.m_pClusterToHitAssociation->at(cluster.key()));
    this->PrintProperty("# Hits", hits.size(), depth + 2);

    if (m_verbosityLevel == "detailed") return;

    // Print each associated hit
    for (const auto& hit : hits)
      this->PrintHit(hit, depth + 4);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintSpacePoint(const art::Ptr<recob::SpacePoint>& spacePoint,
                                            const PandoraData& data,
                                            const unsigned int depth) const
  {
    this->PrintTitle("SpacePoint", depth);
    this->PrintProperty("Key", spacePoint.key(), depth + 2);
    this->PrintProperty("ID", spacePoint->ID(), depth + 2);
    const auto& position(spacePoint->XYZ());
    this->PrintProperty("X", position[0], depth + 2);
    this->PrintProperty("Y", position[1], depth + 2);
    this->PrintProperty("Z", position[2], depth + 2);

    if (!data.m_pSpacePointToHitAssociation) return;

    const auto& hits(data.m_pSpacePointToHitAssociation->at(spacePoint.key()));
    this->PrintProperty("# Hits", hits.size(), depth + 2);

    if (m_verbosityLevel == "detailed") return;

    // Print each associated hit
    for (const auto& hit : hits)
      this->PrintHit(hit, depth + 4);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintVertex(const art::Ptr<recob::Vertex>& vertex,
                                        const unsigned int depth) const
  {
    this->PrintTitle("Vertex", depth);
    this->PrintProperty("Key", vertex.key(), depth + 2);
    this->PrintProperty("ID", vertex->ID(), depth + 2);
    const auto& position(vertex->position());
    this->PrintProperty("X", position.X(), depth + 2);
    this->PrintProperty("Y", position.Y(), depth + 2);
    this->PrintProperty("Z", position.Z(), depth + 2);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintTrack(const art::Ptr<recob::Track>& track,
                                       const PandoraData& data,
                                       const unsigned int depth) const
  {
    this->PrintTitle("Track", depth);
    this->PrintProperty("Key", track.key(), depth + 2);
    this->PrintProperty("# Trajectory points", track->NumberTrajectoryPoints(), depth + 2);
    this->PrintProperty("Length", track->Length(), depth + 2);

    if (!data.m_pTrackToHitAssociation) return;

    const auto& hits(data.m_pTrackToHitAssociation->at(track.key()));
    this->PrintProperty("# Hits", hits.size(), depth + 2);

    if (m_verbosityLevel == "detailed") return;

    // Print each associated hit
    for (const auto& hit : hits)
      this->PrintHit(hit, depth + 4);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintShower(const art::Ptr<recob::Shower>& shower,
                                        const PandoraData& data,
                                        const unsigned int depth) const
  {
    this->PrintTitle("Shower", depth);
    this->PrintProperty("Key", shower.key(), depth + 2);
    this->PrintProperty("ID", shower->ID(), depth + 2);
    this->PrintProperty("StartX", shower->ShowerStart().X(), depth + 2);
    this->PrintProperty("StartY", shower->ShowerStart().Y(), depth + 2);
    this->PrintProperty("StartZ", shower->ShowerStart().Z(), depth + 2);
    this->PrintProperty("Length", shower->Length(), depth + 2);
    this->PrintProperty("OpenAngle", shower->OpenAngle(), depth + 2);

    if (data.m_pShowerToPCAxisAssociation) {
      const auto& pcAxes(data.m_pShowerToPCAxisAssociation->at(shower.key()));
      this->PrintProperty("# PCAxes", pcAxes.size(), depth + 2);

      for (const auto& pcAxis : pcAxes) {
        this->PrintTitle("PCAxis", depth + 4);
        this->PrintProperty("Key", pcAxis.key(), depth + 6);
        this->PrintProperty("ID", pcAxis->getID(), depth + 6);
        this->PrintProperty("# Hits used", pcAxis->getNumHitsUsed(), depth + 6);
      }
    }

    if (!data.m_pShowerToHitAssociation) return;

    const auto& hits(data.m_pShowerToHitAssociation->at(shower.key()));
    this->PrintProperty("# Hits", hits.size(), depth + 2);

    if (m_verbosityLevel == "detailed") return;

    // Print each associated hit
    for (const auto& hit : hits)
      this->PrintHit(hit, depth + 4);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintRule(const unsigned int depth) const
  {
    const unsigned int nDashes(std::max(0, 120 - static_cast<int>(depth)));

    std::cout << std::string(depth, ' ') << std::string(nDashes, '-') << std::endl;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void LArPandoraEventDump::PrintTitle(const std::string& name, const unsigned int depth) const
  {
    std::cout << std::string(depth, ' ') << name << std::endl;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <class T>
  void LArPandoraEventDump::PrintProperty(const std::string& name,
                                          const T& value,
                                          const unsigned int depth) const
  {
    // The separation between the property name and property value
    const unsigned int separation(std::max(0, 32 - static_cast<int>(depth)));

    std::cout << std::string(depth, ' ') << std::setw(separation) << std::left << ("- " + name)
              << value << std::endl;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------
  //------------------------------------------------------------------------------------------------------------------------------------------

  LArPandoraEventDump::PandoraData::PandoraData(const art::Event& evt,
                                                const std::string& pandoraLabel,
                                                const std::string& trackLabel,
                                                const std::string& showerLabel)
    : m_pPFParticleToMetadataAssociation(nullptr)
    , m_pPFParticleToClusterAssociation(nullptr)
    , m_pPFParticleToSpacePointAssociation(nullptr)
    , m_pPFParticleToVertexAssociation(nullptr)
    , m_pPFParticleToTrackAssociation(nullptr)
    , m_pPFParticleToShowerAssociation(nullptr)
    , m_pPFParticleToSliceAssociation(nullptr)
    , m_pClusterToHitAssociation(nullptr)
    , m_pSpacePointToHitAssociation(nullptr)
    , m_pTrackToHitAssociation(nullptr)
    , m_pShowerToHitAssociation(nullptr)
    , m_pSliceToHitAssociation(nullptr)
    , m_pShowerToPCAxisAssociation(nullptr)
  {

    // Load the collections
    this->LoadCollection(evt, pandoraLabel, m_pfParticleCollection);
    this->LoadCollection(evt, pandoraLabel, m_pfParticleMetadataCollection);
    this->LoadCollection(evt, pandoraLabel, m_clusterCollection);
    this->LoadCollection(evt, pandoraLabel, m_spacePointCollection);
    this->LoadCollection(evt, pandoraLabel, m_vertexCollection);
    this->LoadCollection(evt, trackLabel, m_trackCollection);
    this->LoadCollection(evt, showerLabel, m_showerCollection);
    this->LoadCollection(evt, showerLabel, m_pcAxisCollection);
    this->LoadCollection(evt, pandoraLabel, m_sliceCollection);

    // Load the associations
    this->LoadAssociation(
      evt, pandoraLabel, m_pfParticleCollection, m_pPFParticleToMetadataAssociation);
    this->LoadAssociation(
      evt, pandoraLabel, m_pfParticleCollection, m_pPFParticleToClusterAssociation);
    this->LoadAssociation(
      evt, pandoraLabel, m_pfParticleCollection, m_pPFParticleToSpacePointAssociation);
    this->LoadAssociation(
      evt, pandoraLabel, m_pfParticleCollection, m_pPFParticleToVertexAssociation);
    this->LoadAssociation(
      evt, pandoraLabel, m_pfParticleCollection, m_pPFParticleToSliceAssociation);
    this->LoadAssociation(evt, pandoraLabel, m_clusterCollection, m_pClusterToHitAssociation);
    this->LoadAssociation(evt, pandoraLabel, m_spacePointCollection, m_pSpacePointToHitAssociation);
    this->LoadAssociation(evt, pandoraLabel, m_sliceCollection, m_pSliceToHitAssociation);

    this->LoadAssociation(evt, trackLabel, m_pfParticleCollection, m_pPFParticleToTrackAssociation);
    this->LoadAssociation(evt, trackLabel, m_trackCollection, m_pTrackToHitAssociation);

    this->LoadAssociation(
      evt, showerLabel, m_pfParticleCollection, m_pPFParticleToShowerAssociation);
    this->LoadAssociation(evt, showerLabel, m_showerCollection, m_pShowerToHitAssociation);
    this->LoadAssociation(evt, showerLabel, m_showerCollection, m_pShowerToPCAxisAssociation);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  LArPandoraEventDump::PandoraData::~PandoraData()
  {
    // Clean up all heap memory
    delete m_pPFParticleToMetadataAssociation;
    delete m_pPFParticleToClusterAssociation;
    delete m_pPFParticleToSpacePointAssociation;
    delete m_pPFParticleToVertexAssociation;
    delete m_pPFParticleToTrackAssociation;
    delete m_pPFParticleToShowerAssociation;
    delete m_pPFParticleToSliceAssociation;
    delete m_pClusterToHitAssociation;
    delete m_pSpacePointToHitAssociation;
    delete m_pTrackToHitAssociation;
    delete m_pShowerToHitAssociation;
    delete m_pShowerToPCAxisAssociation;
    delete m_pSliceToHitAssociation;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <class T>
  void LArPandoraEventDump::PandoraData::LoadCollection(const art::Event& evt,
                                                        const std::string& label,
                                                        Collection<T>& collection)
  {
    if (label.empty()) return;

    evt.getByLabel(label, collection);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  template <class T, class U>
  void LArPandoraEventDump::PandoraData::LoadAssociation(const art::Event& evt,
                                                         const std::string& label,
                                                         const Collection<T>& collection,
                                                         Association<U>*& pAssociation)
  {
    if (label.empty()) return;

    if (pAssociation)
      throw cet::exception("LArPandoraEventDump")
        << "Association supplied type has already been loaded!";

    pAssociation = new Association<U>(collection, evt, label);
  }

} // namespace lar_pandora
