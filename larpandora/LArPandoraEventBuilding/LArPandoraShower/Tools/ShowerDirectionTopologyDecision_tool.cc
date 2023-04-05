//############################################################################
//### Name:        ShowerDirectionTopologyDecisionTool                     ###
//### Author:      Dom Barker                                              ###
//### Date:        13.05.19                                                ###
//### Description: Decision tool to pick direction based on angular        ###
//###              seperation of two candidate direction vectors           ###
//############################################################################

//Framework Includes
#include "art/Utilities/ToolMacros.h"

//LArSoft Includes
#include "larpandora/LArPandoraEventBuilding/LArPandoraShower/Tools/IShowerTool.h"

#include "Math/VectorUtil.h"

namespace ShowerRecoTools {

  class ShowerDirectionTopologyDecisionTool : public IShowerTool {

  public:
    ShowerDirectionTopologyDecisionTool(const fhicl::ParameterSet& pset);

    //Generic Direction Finder
    int CalculateElement(const art::Ptr<recob::PFParticle>& pfparticle,
                         art::Event& Event,
                         reco::shower::ShowerElementHolder& ShowerEleHolder) override;

  private:
    int fVerbose;
    float fAngleCut;
    std::string fFirstDirectionInputLabel;
    std::string fSecondDirectionInputLabel;
    std::string fShowerDirectionOutputLabel;
  };

  ShowerDirectionTopologyDecisionTool::ShowerDirectionTopologyDecisionTool(
    const fhicl::ParameterSet& pset)
    : IShowerTool(pset.get<fhicl::ParameterSet>("BaseTools"))
    , fVerbose(pset.get<int>("Verbose"))
    , fAngleCut(pset.get<float>("AngleCut"))
    , fFirstDirectionInputLabel(pset.get<std::string>("FirstDirectionInputLabel"))
    , fSecondDirectionInputLabel(pset.get<std::string>("SecondDirectionInputLabel"))
    , fShowerDirectionOutputLabel(pset.get<std::string>("ShowerDirectionOutputLabel"))
  {}

  int ShowerDirectionTopologyDecisionTool::CalculateElement(
    const art::Ptr<recob::PFParticle>& pfparticle,
    art::Event& Event,
    reco::shower::ShowerElementHolder& ShowerEleHolder)
  {

    //Check the relevent products
    if (!ShowerEleHolder.CheckElement(fFirstDirectionInputLabel)) {
      if (fVerbose)
        mf::LogError("ShowerDirectionTopologyDecision")
          << "fFirstDirectionInputLabel is is not set. Stopping.";
      return 1;
    }
    if (!ShowerEleHolder.CheckElement(fSecondDirectionInputLabel)) {
      if (fVerbose)
        mf::LogError("ShowerDirectionTopologyDecision")
          << "fSecondDirectionInputLabel is is not set. Stopping.";
      return 1;
    }

    //Get the relevent products
    geo::Vector_t FirstShowerDirection;
    geo::Vector_t FirstShowerDirectionError;
    ShowerEleHolder.GetElementAndError(
      fFirstDirectionInputLabel, FirstShowerDirection, FirstShowerDirectionError);

    geo::Vector_t SecondShowerDirection;
    geo::Vector_t SecondShowerDirectionError;
    ShowerEleHolder.GetElementAndError(
      fSecondDirectionInputLabel, SecondShowerDirection, SecondShowerDirectionError);

    //Use the first tool if directions agree within the chosen angle
    if (ROOT::Math::VectorUtil::Angle(FirstShowerDirection, SecondShowerDirection) < fAngleCut) {
      ShowerEleHolder.SetElement(
        FirstShowerDirection, FirstShowerDirectionError, fShowerDirectionOutputLabel);
    }
    else {
      ShowerEleHolder.SetElement(
        SecondShowerDirection, SecondShowerDirectionError, fShowerDirectionOutputLabel);
    }
    return 0;
  }
}

DEFINE_ART_CLASS_TOOL(ShowerRecoTools::ShowerDirectionTopologyDecisionTool)
