/**
 *  @file   larpandora/LArPandoraEventBuilding/SliceIdBaseTool.h
 *
 *  @brief  header for the lar pandora slice ID base tool
 */

#ifndef LAR_PANDORA_SLICE_ID_BASE_TOOL_H
#define LAR_PANDORA_SLICE_ID_BASE_TOOL_H 1

#include "art/Framework/Principal/Event.h"

#include "larpandora/LArPandoraEventBuilding/Slice.h"
#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

namespace lar_pandora
{

/**
 *  @brief  Abstract base class for a slice ID tool
 */
class SliceIdBaseTool
{
public:
    virtual ~SliceIdBaseTool() noexcept = default;

    /**
     *  @brief  The tools interface function. Here the derived tool will classify the input slices
     *
     *  @param  slices the input vector of slices to classify
     *  @param  evt the art event
     */
    virtual void ClassifySlices(SliceVector &slices, const art::Event &evt) = 0;
};

} // namespace lar_pandora

#endif // #ifndef LAR_PANDORA_SLICE_ID_BASE_TOOL_H
