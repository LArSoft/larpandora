/**
 *  @file  larpandora/LArPandoraInterface/Tools/LArPandoraHitCollectionToolDefault_tool.cc
 * 
 *  @brief Define class for hit collection tools and implements DEFAULT base tool
 * 
 */

#include "art/Utilities/ToolMacros.h"

//#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"
#include "larpandora/LArPandoraInterface/LArPandoraHitCollectionTool.h"

namespace HitCollectionTools {

  class LArPandoraHitCollectionToolDefault : public HitCollectionTool
  {
  public:
    explicit LArPandoraHitCollectionToolDefault(const fhicl::ParameterSet& pset);
    void CollectHits(const art::Event& evt, const std::string& label, lar_pandora::HitVector& hitVector) override;
  };

  LArPandoraHitCollectionToolDefault::LArPandoraHitCollectionToolDefault(const fhicl::ParameterSet& pset){}

  void LArPandoraHitCollectionToolDefault::CollectHits(const art::Event& evt, const std::string& label, lar_pandora::HitVector& hitVector)
  {
    lar_pandora::LArPandoraHelper::CollectHits(evt, label, hitVector);
  }

} // namespace lar_pandora

DEFINE_ART_CLASS_TOOL(HitCollectionTools::LArPandoraHitCollectionToolDefault)
