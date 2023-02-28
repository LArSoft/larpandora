/**
 *  @file  larpandora/LArPandoraInterface/Tools/LArPandoraHitCollectionTool.h
 * 
 *  @brief Define class for hit collection tools and implements base tool
 * 
 */
#ifndef LAR_PANDORA_HIT_COLLECTION_TOOL_H
#define LAR_PANDORA_HIT_COLLECTION_TOOL_H

#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

#include "art/Framework/Principal/Event.h"

namespace HitCollectionTools {

  class HitCollectionTool
  {
  public:
    virtual ~HitCollectionTool() noexcept = default;
    virtual void CollectHits(const art::Event& evt, const std::string& label, lar_pandora::HitVector& hitVector) = 0;
  };

} // namespace lar_pandora

#endif //  LAR_PANDORA_HITCOLLECTION_TOOL_H
