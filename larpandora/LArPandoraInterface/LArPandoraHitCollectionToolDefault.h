/**
 *  @file  larpandora/LArPandoraInterface/LArPandoraHitCollectionToolDefault.h
 *
 *  @brief Implement default hit collection tool (.h)
 *
 */

#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"
#include "larpandora/LArPandoraInterface/LArPandoraHitCollectionTool.h"

namespace lar_pandora {

  class LArPandoraHitCollectionToolDefault : public IHitCollectionTool {
  public:
    explicit LArPandoraHitCollectionToolDefault(const fhicl::ParameterSet& pset);
    void CollectHits(const art::Event& evt,
                     const std::string& label,
                     HitVector& hitVector) override;
  };

} // namespace lar_pandora
