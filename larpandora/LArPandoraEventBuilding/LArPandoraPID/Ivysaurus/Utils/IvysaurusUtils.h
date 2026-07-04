/**
 *  @file   larpandora/LArPandoraEventBuilding/LArPandoraPID/Ivysaurus/Utils/IvysaurusUtils.h
 *
 *  @brief  Helpers used by the Ivysaurus PID
 */
#ifndef IVYSAURUS_UTILS_H
#define IVYSAURUS_UTILS_H
// ART
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "canvas/Persistency/Common/Ptr.h" 
#include "canvas/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 
#include "canvas/Persistency/Common/FindManyP.h"
// LArSoft
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "larsim/Utils/TruthMatchUtils.h"
// ROOT
#include "TVector3.h"
//C++
#include <string>
#include <vector>

namespace IvysaurusUtils
{
    enum PandoraView {TPC_VIEW_U, TPC_VIEW_V, TPC_VIEW_W};

    /**
     *  @brief  Project a 3D coordinate into an input Pandora 2D view
     *
     *  @param inputPosition3D the input 3D coordinate
     *  @param pandoraView the input Pandora 2D view
     *
     *  @return the projected 2D position
     */        
    const TVector3 ProjectIntoPandoraView(const TVector3 &inputPosition3D, const PandoraView pandoraView);

    /**
     *  @brief  Obtain the Pandora view of a LArSoft hit
     *
     *  @param hit the input LArSoft hit
     *
     *  @return the Pandora view
     */ 
    const PandoraView GetPandora2DView(const art::Ptr<recob::Hit> &hit);

    void ObtainPandoraHitPositionAndWidth(const art::Event &evt, const art::Ptr<recob::Hit> hit, 
        const PandoraView hitType, TVector3 &pandoraHitPosition, float &width);

    // Function to obtain the Pandora U coordinate from LArSoft Y/Z coordinates
    float YZToU(const float yCoord, const float zCoord);

    // Function to obtain the Pandora V coordinate from LArSoft Y/Z coordinates
    float YZToV(const float yCoord, const float zCoord);

    // Function to obtain the Pandora W coordinate from LArSoft Y/Z coordinates
    float YZToW(const float yCoord, const float zCoord);

    double CompletenessFromTrueParticleID(detinfo::DetectorClocksData const& clockData, const std::vector<art::Ptr<recob::Hit>> &selectedHits, 
        const std::vector<art::Ptr<recob::Hit>> &eventHits, const int trackID);

    double HitPurityFromTrueParticleID(detinfo::DetectorClocksData const& clockData, const std::vector<art::Ptr<recob::Hit>> &selectedHits,
        const int trackID);

    float IntegrateGaussian(const float limitA, const float limitB, const float mean, const float std, const float stepSize);

    float TrapeziumRule(const float lowerLimit, const float upperLimit, const float mean, const float std);

    bool GetInitialDirection(const art::Event &evt, const TVector3 &pfpVertex, const std::vector<art::Ptr<recob::SpacePoint>> &spacepoints, 
        const std::string &recoModuleLabel, TVector3 &direction);
}

#endif // IVYSAURUS_UTILS_H
