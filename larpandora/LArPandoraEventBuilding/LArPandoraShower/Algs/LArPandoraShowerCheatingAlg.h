#ifndef LArPandoraShowerCheatingAlg_hxx
#define LArPandoraShowerCheatingAlg_hxx

//LArSoft Includes
#include "larpandora/LArPandoraEventBuilding/LArPandoraShower/Algs/LArPandoraShowerAlg.h"
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileService.h"
#include "canvas/Utilities/InputTag.h"

namespace recob {
  class Hit;
  class PFParticle;
}
namespace simb {
  class MCParticle;
}

namespace detinfo {
  class DetectorClocksData;
}

namespace shower {
  class LArPandoraShowerCheatingAlg;
}

namespace reco::shower {
  class ShowerElementHolder;
}

#include <map>
#include <utility> // std::pair<>
#include <vector>

class shower::LArPandoraShowerCheatingAlg {
public:
  LArPandoraShowerCheatingAlg(const fhicl::ParameterSet& pset);

  std::map<int, const simb::MCParticle*> GetTrueParticleMap() const;
  std::map<int, std::vector<int>> GetTrueChain(
    std::map<int, const simb::MCParticle*>& trueParticles) const;
  void CheatDebugEVD(detinfo::DetectorClocksData const& clockData,
                     const simb::MCParticle* trueParticle,
                     art::Event const& Event,
                     reco::shower::ShowerElementHolder& ShowerEleHolder,
                     const art::Ptr<recob::PFParticle>& pfparticle) const;

  int TrueParticleID(detinfo::DetectorClocksData const& clockData,
                     const art::Ptr<recob::Hit>& hit) const;

  std::pair<int, double> TrueParticleIDFromTrueChain(
    detinfo::DetectorClocksData const& clockData,
    std::map<int, std::vector<int>> const& ShowersMothers,
    std::vector<art::Ptr<recob::Hit>> const& hits,
    int planeid) const;

private:
  shower::LArPandoraShowerAlg fLArPandoraShowerAlg;

  art::InputTag fHitModuleLabel;
  art::InputTag fPFParticleLabel;
  art::ServiceHandle<cheat::ParticleInventoryService> particleInventory;
  art::ServiceHandle<art::TFileService> tfs;

  std::string fShowerStartPositionInputLabel;
  std::string fShowerDirectionInputLabel;
  std::string fInitialTrackSpacePointsInputLabel;
};
#endif
