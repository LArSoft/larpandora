/**
 *  @file   larpandora/LArPandoraEventBuilding/LArPandoraSimpleBeamParticleId_tool.cc
 *
 *  @brief  implementation of the lar pandora simple beam particle id tool
 */

#include "art/Utilities/ToolMacros.h"
#include "fhiclcpp/ParameterSet.h"

#include "larpandora/LArPandoraEventBuilding/SliceIdBaseTool.h"
#include "larpandora/LArPandoraEventBuilding/Slice.h"

namespace lar_pandora
{

/**
 *  @brief  Simple beam particle ID tool that selects the most likely beam particle slice using the scores from Pandora
 */
class SimpleBeamParticleId : SliceIdBaseTool
{
public:
    /**
     *  @brief  Default constructor
     *
     *  @param  pset FHiCL parameter set
     */
    SimpleBeamParticleId(fhicl::ParameterSet const &pset);

    /**
     *  @brief  Classify slices as neutrino or cosmic
     *
     *  @param  slices the input vector of slices to classify
     *  @param  evt the art event
     */
    void ClassifySlices(SliceVector &slices, const art::Event &evt) override;

private:
    float m_minBDTScore; ///< The minimum BDT score to select a slice as a beam particle

};

DEFINE_ART_CLASS_TOOL(SimpleBeamParticleId)

} // namespace lar_pandora

//------------------------------------------------------------------------------------------------------------------------------------------
// implementation follows

namespace lar_pandora
{
 
SimpleBeamParticleId::SimpleBeamParticleId(fhicl::ParameterSet const &pset) :
    m_minBDTScore(pset.get<float>("MinBDTScore"))
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SimpleBeamParticleId::ClassifySlices(SliceVector &slices, const art::Event &/*evt*/) 
{
    for (Slice &slice : slices)
    {
        if (slice.GetTopologicalScore() > m_minBDTScore)
            slice.TagAsTarget();
    }
}

} // namespace lar_pandora
