//############################################################################
//### Name:        ShowerTrackTrajPointDirection                           ###
//### Author:      Dominic Barker                                          ###
//### Date:        13.05.19                                                ###
//### Description: Tool for finding the shower direction using the         ###
//###              first trajectory of the initial track                   ###
//############################################################################

//Framework Includes
#include "art/Utilities/ToolMacros.h"

//LArSoft Includes
#include "larpandora/LArPandoraEventBuilding/LArPandoraShower/Tools/IShowerTool.h"

#include "lardataobj/RecoBase/Track.h"

namespace ShowerRecoTools {

  class ShowerTrackTrajPointDirection : IShowerTool {

  public:
    ShowerTrackTrajPointDirection(const fhicl::ParameterSet& pset);

    //Calculate the direction from the inital track
    int CalculateElement(const art::Ptr<recob::PFParticle>& pfparticle,
                         art::Event& Event,
                         reco::shower::ShowerElementHolder& ShowerEleHolder) override;

  private:
    //fcl
    int fVerbose;
    bool fUsePandoraVertex; //Direction from point defined as
    //(Position of traj point - Vertex) rather than
    //(Position of traj point - Track Start Point).
    bool fUsePositonInfo; //Don't use the direction At point rather than definition
    //above.
    //((Position of traj point + 1) - (Position of traj point).
    int fTrajPoint; //Trajectory point to get the direction from.

    std::string fInitialTrackInputLabel;
    std::string fShowerStartPositionInputLabel;
    std::string fShowerDirectionOutputLabel;
  };

  ShowerTrackTrajPointDirection::ShowerTrackTrajPointDirection(const fhicl::ParameterSet& pset)
    : IShowerTool(pset.get<fhicl::ParameterSet>("BaseTools"))
    , fVerbose(pset.get<int>("Verbose"))
    , fUsePandoraVertex(pset.get<bool>("UsePandoraVertex"))
    , fUsePositonInfo(pset.get<bool>("UsePositonInfo"))
    , fTrajPoint(pset.get<int>("TrajPoint"))
    , fInitialTrackInputLabel(pset.get<std::string>("InitialTrackInputLabel"))
    , fShowerStartPositionInputLabel(pset.get<std::string>("ShowerStartPositionInputLabel"))
    , fShowerDirectionOutputLabel(pset.get<std::string>("ShowerDirectionOutputLabel"))
  {}

  int ShowerTrackTrajPointDirection::CalculateElement(
    const art::Ptr<recob::PFParticle>& pfparticle,
    art::Event& Event,
    reco::shower::ShowerElementHolder& ShowerEleHolder)
  {

    //Check the Track has been defined
    if (!ShowerEleHolder.CheckElement(fInitialTrackInputLabel)) {
      if (fVerbose)
        mf::LogError("ShowerTrackTrajPointDirection") << "Initial track not set" << std::endl;
      return 1;
    }
    recob::Track InitialTrack;
    ShowerEleHolder.GetElement(fInitialTrackInputLabel, InitialTrack);

    if ((int)InitialTrack.NumberTrajectoryPoints() - 1 < fTrajPoint) {
      if (fVerbose)
        mf::LogError("ShowerTrackTrajPointDirection")
          << "Less that fTrajPoint trajectory points, bailing." << std::endl;
      fTrajPoint = InitialTrack.NumberTrajectoryPoints() - 1;
    }

    //ignore bogus info.
    auto flags = InitialTrack.FlagsAtPoint(fTrajPoint);
    if (flags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) {
      if (fVerbose)
        mf::LogError("ShowerTrackTrajPointDirection")
          << "Bogus trajectory point bailing." << std::endl;
      return 1;
    }

    geo::Vector_t Direction;
    //Get the difference between the point and the start position.
    if (fUsePositonInfo) {
      //Get the start position.
      geo::Point_t StartPosition;
      if (fUsePandoraVertex) {
        //Check the Track has been defined
        if (!ShowerEleHolder.CheckElement(fShowerStartPositionInputLabel)) {
          if (fVerbose)
            mf::LogError("ShowerTrackTrajPointDirection")
              << "Shower start position not set" << std::endl;
          return 1;
        }
        ShowerEleHolder.GetElement(fShowerStartPositionInputLabel, StartPosition);
      }
      else {
        StartPosition = InitialTrack.Start();
      }
      //Get the specific trajectory point and look and and the direction from the start position
      geo::Point_t TrajPosition = InitialTrack.LocationAtPoint(fTrajPoint);
      Direction = (TrajPosition - StartPosition).Unit();
    }
    else {
      //Use the direction of the trajection at tat point;
      Direction = InitialTrack.DirectionAtPoint(fTrajPoint);
    }

    geo::Vector_t DirectionErr = {-999, -999, -999};
    ShowerEleHolder.SetElement(Direction, DirectionErr, fShowerDirectionOutputLabel);
    return 0;
  }
}

DEFINE_ART_CLASS_TOOL(ShowerRecoTools::ShowerTrackTrajPointDirection)
