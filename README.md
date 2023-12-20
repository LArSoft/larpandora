# LArPandora

This code is part of the Liquid Argon Software (LArSoft) project.
It contains simulation and reconstruction algorithms for LAr TPC detectors.

## Dependencies

```
larpandora
|__larpandoracontent
|__larreco
```

## Overview

The larpandora repository contains the interfaces and algorithms for running the Pandora reconstruction chain within LArSoft. Pandora provides a highly modular approach to event reconstruction in fine-grain detectors, using a series of focused algorithms to reconstruct the particle flow of an event. The Pandora reconstruction currently supports the MicroBooNE, DUNE FD, ProtoDUNE, SBND and Icarus detector geometries.

The Pandora LAr-TPC reconstruction chain takes as its input 2D hits and builds a set of 3D particles. The reconstruction chain runs within the lightweight framework supplied by the Pandora Software Development Kit (SDK), which manages the algorithms and provides a set of APIs for controlled memory management. The LAr-TPC algorithms themselves are developed and housed within LArSoft, and are run inside LArSoft using an ART Producer module, which inputs recob::Hit objects and outputs recob::PFParticle objects. The aim is to return one reconstructed particle for each true final-state particle in the event. The reconstruction also returns the parent/daughter hierarchy of particle associations (representing the particle flow of the event), along with a series of ART associations to low-level objects (recob::Cluster, recob::Seed, recob::SpacePoint).

The following sections provide notes on the two packages within the larpandora repository, LArPandoraAlgorithms and LArPandoraInterface, which support the pattern recognition algorithms and ART/LArSoft inputs/outputs respectively. Also included are instructions for running the Pandora reconstruction within LArSoft.

## Packages

### LArPandoraInterface

This package contains the interface between ART/LArSoft and the Pandora SDK. The central component is an ART Producer module that reads in recob::Hit objects from the ART framework, passes them to the Pandora SDK for pattern recognition, and then writes the reconstructed proto-particles back to the ART framework as recob::PFParticle objects. The LArPandoraInterface package holds a number of additional Producer and Analyzer modules that manipulate and analyse the reconstructed recob::PFParticle objects. Also distributed in this packages are .fcl scripts for configuring and running the Producer modules, and a number of Pandora .xml steering files, which are loaded into Pandora by the Producer module and define the order and configuration of the pattern recognition algorithms.

### Pandora SDK (Note: external package).

This external package houses a lightweight framework for running pattern recognition algorithms and performing memory management operations. The Pandora SDK also supplies design-pattern algorithms and helper functions that are used in building reconstruction chains. Note that the LAr-TPC specific algorithms and helpers live in the larpandoracontent package of LArSoft.

## Instructions

### Running the reconstruction

In general, Pandora is run as part of a broader reconstruction chain defined by the particular experiment under consideration, and you should refer to the documentation for your experiment for details. However, in general, Pandora will be defined as a producer of the form `pandora: @local::dunefd_pandora` defined via a fcl file with a name like `pandoramodules_dune.fcl`, to take examples from DUNE.

### Build instructions for Pandora SDK and Monitoring packages

The larpandora repository depends on the Pandora SDK. In addition to the SDK, there is also an optional Pandora Monitoring package, which depends on ROOT. Both packages can be cloned from the "Pandora git repository": https://github.com/PandoraPFA/. Here are a set of instructions for checking out and building the SDK and Monitoring packages in the context of DUNE (you can adapt the inital setup for your experiment, other details will remain unchanged):

```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup root <version> -q <qualifier>

export MY_TEST_AREA=`pwd`

cd $MY_TEST_AREA
git clone git@github.com:PandorPFA/PandoraPFA.git
git clone git@github.com:PandorPFA/PandoraSDK.git
git clone git@github.com:PandorPFA/PandoraMonitoring.git

mkdir -p $MY_TEST_AREA/PandoraSDK/build
mkdir -p $MY_TEST_AREA/PandoraMonitoring/build

cd $MY_TEST_AREA/PandoraSDK/build
cmake -DCMAKE_MODULE_PATH="$MY_TEST_AREA/PandoraPFA/cmakemodules;$ROOTSYS/etc/cmake" -DCMAKE_CXX_STANDARD=17 ..
make -j4 install

cd $MY_TEST_AREA/PandoraMonitoring/build
cmake -DCMAKE_MODULE_PATH="$MY_TEST_AREA/PandoraPFA/cmakemodules;$ROOTSYS/etc/cmake" -DPandoraSDK_DIR=$MY_TEST_AREA/PandoraSDK -DCMAKE_CXX_STANDARD=17 ..
make -j4 install
