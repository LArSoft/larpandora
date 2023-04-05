//############################################################################
//### Name:        ShowerTrackColinearTrajPointDirection                   ###
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

//ROOT
#include "Math/VectorUtil.h"

using ROOT::Math::VectorUtil::Angle;

namespace ShowerRecoTools {

  class ShowerTrackColinearTrajPointDirection : IShowerTool {

  public:
    ShowerTrackColinearTrajPointDirection(const fhicl::ParameterSet& pset);

    //Calculate the direction from the inital track.
    int CalculateElement(const art::Ptr<recob::PFParticle>& pfparticle,
                         art::Event& Event,
                         reco::shower::ShowerElementHolder& ShowerEleHolder) override;

  private:
    //fcl
    int fVerbose;
    bool fUsePandoraVertex; //Direction from point defined as
    //(Position of traj point - Vertex) rather than
    //(Position of traj point - Track Start Point).
    bool fAllowDynamicSliding; //Rather than evualte the angle from the start use
    //the previous trajectory point position.
    bool fUsePositionInfo; //Don't use the DirectionAtPoint rather than
    //definition above.
    //((Position of traj point + 1)-(Position of traj point)
    bool fUseStartPos; //Rather the using the angles between the directions
    //from start position to the trajectory points
    //use the angle between the the points themselves
    float fAngleCut;

    std::string fInitialTrackInputLabel;
    std::string fShowerStartPositionInputLabel;
    std::string fShowerDirectionOutputLabel;
  };

  ShowerTrackColinearTrajPointDirection::ShowerTrackColinearTrajPointDirection(
    const fhicl::ParameterSet& pset)
    : IShowerTool(pset.get<fhicl::ParameterSet>("BaseTools"))
    , fVerbose(pset.get<int>("Verbose"))
    , fUsePandoraVertex(pset.get<bool>("UsePandoraVertex"))
    , fAllowDynamicSliding(pset.get<bool>("AllowDynamicSliding"))
    , fUsePositionInfo(pset.get<bool>("UsePositionInfo"))
    , fUseStartPos(pset.get<bool>("UseStartPos"))
    , fAngleCut(pset.get<float>("AngleCut"))
    , fInitialTrackInputLabel(pset.get<std::string>("InitialTrackInputLabel"))
    , fShowerStartPositionInputLabel(pset.get<std::string>("ShowerStartPositionInputLabel"))
    , fShowerDirectionOutputLabel(pset.get<std::string>("ShowerDirectionOutputLabel"))

  {}

  int ShowerTrackColinearTrajPointDirection::CalculateElement(
    const art::Ptr<recob::PFParticle>& pfparticle,
    art::Event& Event,
    reco::shower::ShowerElementHolder& ShowerEleHolder)
  {

    //Check the Track has been defined
    if (!ShowerEleHolder.CheckElement(fInitialTrackInputLabel)) {
      if (fVerbose)
        mf::LogError("ShowerTrackColinearTrajPointDirection")
          << "Initial track not set" << std::endl;
      return 1;
    }
    recob::Track InitialTrack;
    ShowerEleHolder.GetElement(fInitialTrackInputLabel, InitialTrack);

    //Smartly choose the which trajectory point to look at by ignoring the smush of hits at the vertex.
    if (InitialTrack.NumberTrajectoryPoints() == 1) {
      if (fVerbose)
        mf::LogError("ShowerTrackColinearTrajPointDirection")
          << "Not Enough trajectory points." << std::endl;
      return 1;
    }

    //Trajectory point which the direction is calcualted for.
    int trajpoint = 0;
    geo::Vector_t Direction;

    if (fUsePositionInfo) {
      //Get the start position.
      geo::Point_t StartPosition;

      if (fUsePandoraVertex) {
        //Check the Track has been defined
        if (!ShowerEleHolder.CheckElement(fShowerStartPositionInputLabel)) {
          if (fVerbose)
            mf::LogError("ShowerTrackColinearTrajPointDirection")
              << "Shower start position not set" << std::endl;
          return 1;
        }
        ShowerEleHolder.GetElement(fShowerStartPositionInputLabel, StartPosition);
      }
      else {
        StartPosition = InitialTrack.Start();
      }

      //Loop over the trajectory points and find two corresponding trajectory points where the angle between themselves (or themsleves and the start position) is less the fMinAngle.
      for (unsigned int traj = 0; traj < InitialTrack.NumberTrajectoryPoints() - 2; ++traj) {
        ++trajpoint;

        //ignore bogus info.
        auto trajflags = InitialTrack.FlagsAtPoint(trajpoint);
        if (trajflags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) { continue; }

        bool bail = false;

        //ignore bogus info.
        auto flags = InitialTrack.FlagsAtPoint(traj);
        if (flags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) { continue; }

        //find the next non bogus traj point.
        int nexttraj = traj + 1;
        auto nextflags = InitialTrack.FlagsAtPoint(nexttraj);
        while (nextflags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) {
          if (nexttraj == (int)InitialTrack.NumberTrajectoryPoints() - 2) {
            bail = true;
            break;
          }
          ++nexttraj;
          nextflags = InitialTrack.FlagsAtPoint(nexttraj);
        }

        //find the next next non bogus traj point.
        int nextnexttraj = nexttraj + 1;
        auto nextnextflags = InitialTrack.FlagsAtPoint(nextnexttraj);
        while (nextnextflags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) {
          if (nexttraj == (int)InitialTrack.NumberTrajectoryPoints() - 1) {
            bail = true;
            break;
          }
          ++nextnexttraj;
          nextnextflags = InitialTrack.FlagsAtPoint(nextnexttraj);
        }

        if (bail) {
          if (fVerbose)
            mf::LogError("ShowerTrackColinearTrajPointDirection")
              << "Trajectory point not set as rest of the traj points are bogus." << std::endl;
          break;
        }

        //Get the directions.
        geo::Vector_t TrajPosition = InitialTrack.LocationAtPoint(traj) - StartPosition;
        geo::Vector_t NextTrajPosition;
        geo::Vector_t NextNextTrajPosition;
        if (fUseStartPos) {
          NextTrajPosition = InitialTrack.LocationAtPoint(nexttraj) - StartPosition;
          NextNextTrajPosition = InitialTrack.LocationAtPoint(nextnexttraj) - StartPosition;
        }
        else {
          NextTrajPosition =
            InitialTrack.LocationAtPoint(nexttraj) - InitialTrack.LocationAtPoint(traj);
          NextNextTrajPosition =
            InitialTrack.LocationAtPoint(nextnexttraj) - InitialTrack.LocationAtPoint(traj + 1);
        }

        //Might still be bogus and we can't use the start point
        if (TrajPosition.R() == 0) { continue; }
        if (NextTrajPosition.R() == 0) { continue; }
        if (NextNextTrajPosition.R() == 0) { continue; }

        //Check to see if the angle between the directions is small enough.
        if (Angle(TrajPosition, NextTrajPosition) < fAngleCut &&
            Angle(TrajPosition, NextNextTrajPosition) < fAngleCut) {
          break;
        }

        //Move the start position onwords.
        if (fAllowDynamicSliding) { StartPosition = InitialTrack.LocationAtPoint(traj); }
      }

      geo::Point_t TrajPosition = InitialTrack.LocationAtPoint(trajpoint);
      Direction = (TrajPosition - StartPosition).Unit();
    }
    else {
      //Loop over the trajectory points and find two corresponding trajectory points where the angle between themselves (or themsleves and the start position) is less the fMinAngle.
      for (unsigned int traj = 0; traj < InitialTrack.NumberTrajectoryPoints() - 2; ++traj) {
        ++trajpoint;

        //ignore bogus info.
        auto trajflags = InitialTrack.FlagsAtPoint(trajpoint);
        if (trajflags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) { continue; }

        //ignore bogus info.
        auto flags = InitialTrack.FlagsAtPoint(traj);
        if (flags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) { continue; }

        bool bail = false;

        geo::Vector_t TrajDirection;

        //Get the next non bogus trajectory points
        if (fUseStartPos) {
          int prevtraj = 0;
          auto prevflags = InitialTrack.FlagsAtPoint(prevtraj);
          while (prevflags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) {
            if (prevtraj == (int)InitialTrack.NumberTrajectoryPoints() - 2) {
              bail = true;
              break;
            }
            ++prevtraj;
            prevflags = InitialTrack.FlagsAtPoint(prevtraj);
          }
          TrajDirection = InitialTrack.DirectionAtPoint(prevtraj);
        }
        else if (fAllowDynamicSliding && traj != 0) {
          int prevtraj = traj - 1;
          auto prevflags = InitialTrack.FlagsAtPoint(prevtraj);
          while (prevflags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) {
            if (prevtraj == 0) {
              bail = true;
              break;
            }
            --prevtraj;
            prevflags = InitialTrack.FlagsAtPoint(prevtraj);
          }
          TrajDirection = InitialTrack.DirectionAtPoint(prevtraj);
        }
        else {
          TrajDirection = InitialTrack.DirectionAtPoint(traj);
        }

        //find the next non bogus traj point.
        int nexttraj = traj + 1;
        auto nextflags = InitialTrack.FlagsAtPoint(nexttraj);
        while (nextflags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) {
          if (nexttraj == (int)InitialTrack.NumberTrajectoryPoints() - 2) {
            bail = true;
            break;
          }
          ++nexttraj;
          nextflags = InitialTrack.FlagsAtPoint(nexttraj);
        }

        //find the next next non bogus traj point.
        int nextnexttraj = nexttraj + 1;
        auto nextnextflags = InitialTrack.FlagsAtPoint(nextnexttraj);
        while (nextnextflags.isSet(recob::TrajectoryPointFlagTraits::NoPoint)) {
          if (nexttraj == (int)InitialTrack.NumberTrajectoryPoints() - 1) {
            bail = true;
            break;
          }
          ++nextnexttraj;
          nextnextflags = InitialTrack.FlagsAtPoint(nextnexttraj);
        }

        if (bail) {
          if (fVerbose)
            mf::LogError("ShowerTrackColinearTrajPointDirection")
              << "Trajectory point not set as rest of the traj points are bogus." << std::endl;
          break;
        }

        //Get the directions.
        geo::Vector_t NextTrajDirection = InitialTrack.DirectionAtPoint(nexttraj);
        geo::Vector_t NextNextTrajDirection = InitialTrack.DirectionAtPoint(nextnexttraj);

        //Might still be bogus and we can't use the start point
        if (TrajDirection.R() == 0) { continue; }
        if (NextTrajDirection.R() == 0) { continue; }
        if (NextNextTrajDirection.R() == 0) { continue; }

        //See if the angle is small enough.
        if (Angle(TrajDirection, NextTrajDirection) < fAngleCut &&
            Angle(TrajDirection, NextNextTrajDirection) < fAngleCut) {
          break;
        }
      }
      Direction = InitialTrack.DirectionAtPoint(trajpoint).Unit();
    }

    if (trajpoint == (int)InitialTrack.NumberTrajectoryPoints() - 3) {
      if (fVerbose)
        mf::LogError("ShowerSmartTrackTrajectoryPointDirectio")
          << "Trajectory point not set." << std::endl;
      return 1;
    }

    //Set the direction.
    geo::Vector_t DirectionErr = {-999, -999, -999};
    ShowerEleHolder.SetElement(Direction, DirectionErr, fShowerDirectionOutputLabel);
    return 0;
  }
}

DEFINE_ART_CLASS_TOOL(ShowerRecoTools::ShowerTrackColinearTrajPointDirection)
