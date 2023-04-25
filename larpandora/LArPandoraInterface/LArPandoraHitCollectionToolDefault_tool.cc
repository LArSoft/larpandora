/**
 *  @file  larpandora/LArPandoraInterface/LArPandoraHitCollectionToolDefault_tool.cc
 *
 *  @brief Implement default hit collection tool (_tool.cc)
 *
 */

#include "art/Utilities/ToolMacros.h"

#include "larpandora/LArPandoraInterface/LArPandoraHitCollectionToolDefault.h"

namespace lar_pandora {

  LArPandoraHitCollectionToolDefault::LArPandoraHitCollectionToolDefault(
    const fhicl::ParameterSet& pset)
  {}

  void LArPandoraHitCollectionToolDefault::CollectHits(const art::Event& evt,
                                                       const std::string& label,
                                                       HitVector& hitVector)
  {
    LArPandoraHelper::CollectHits(evt, label, hitVector);
  }

} // namespace lar_pandora

DEFINE_ART_CLASS_TOOL(lar_pandora::LArPandoraHitCollectionToolDefault)
