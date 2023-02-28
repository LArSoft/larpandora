/**
 *  @file   larpandora/LArPandoraEventBuilding/Slice.h
 *
 *  @brief  header for the lar pandora slice class
 */

#ifndef LAR_PANDORA_SLICE_H
#define LAR_PANDORA_SLICE_H 1

#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"

namespace lar_pandora {

  /**
 *  @brief Slice class
 */
  class Slice {
  public:
    /**
     *  @brief  Default constructor
     *
     *  @param  nuScore the neutrino score from Pandora
     *  @param  nuHypothesis the slice as reconstructed under the neutrino hypothesis
     *  @param  crHypothesis the slice as reconstructed under the cosmic-ray hypothesis
     *  @param  isNeutrino if the slice has been identified as a neutrino
     */
    Slice(const float nuScore,
          const PFParticleVector& nuHypothesis,
          const PFParticleVector& crHypothesis,
          const bool isNeutrino = false);

    /**
     *  @brief Get the neutrino score for the slice
     */
    float GetNeutrinoScore() const;

    /**
     *  @brief Get the slice as reconstructed under the neutrino hypothesis
     */
    const PFParticleVector& GetNeutrinoHypothesis() const;

    /**
     *  @brief Get the slice as reconstructed under the cosmic-ray hypothesis
     */
    const PFParticleVector& GetCosmicRayHypothesis() const;

    /**
     *  @brief Check if the slice has been identified as a neutrino
     */
    bool IsTaggedAsNeutrino() const;

    /**
     *  @brief Tag the slice as a neutrino
     */
    void TagAsNeutrino();

    /**
     *  @brief Tag the slice as a cosmic
     */
    void TagAsCosmic();

  private:
    float m_nuScore;                 ///< The neutrino score from Pandora
    PFParticleVector m_nuHypothesis; ///< The slice as reconstructed under the neutrino hypothesis
    PFParticleVector m_crHypothesis; ///< The slice as reconstructed under the cosmic-ray hypothesis
    bool m_isNeutrino;               ///< If the slice has been identified as a neutrino
  };

  typedef std::vector<Slice> SliceVector;

  //------------------------------------------------------------------------------------------------------------------------------------------

  inline Slice::Slice(const float nuScore,
                      const PFParticleVector& nuHypothesis,
                      const PFParticleVector& crHypothesis,
                      const bool isNeutrino)
    : m_nuScore(nuScore)
    , m_nuHypothesis(nuHypothesis)
    , m_crHypothesis(crHypothesis)
    , m_isNeutrino(isNeutrino)
  {}

  //------------------------------------------------------------------------------------------------------------------------------------------

  inline float Slice::GetNeutrinoScore() const { return m_nuScore; }

  //------------------------------------------------------------------------------------------------------------------------------------------

  inline const PFParticleVector& Slice::GetNeutrinoHypothesis() const { return m_nuHypothesis; }

  //------------------------------------------------------------------------------------------------------------------------------------------

  inline const PFParticleVector& Slice::GetCosmicRayHypothesis() const { return m_crHypothesis; }

  //------------------------------------------------------------------------------------------------------------------------------------------

  inline bool Slice::IsTaggedAsNeutrino() const { return m_isNeutrino; }

  //------------------------------------------------------------------------------------------------------------------------------------------

  inline void Slice::TagAsNeutrino() { m_isNeutrino = true; }

  //------------------------------------------------------------------------------------------------------------------------------------------

  inline void Slice::TagAsCosmic() { m_isNeutrino = false; }

} // namespace lar_pandora

#endif // #ifndef LAR_PANDORA_SLICE_H
