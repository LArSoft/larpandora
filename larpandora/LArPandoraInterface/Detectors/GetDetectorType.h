#ifndef LAR_PANDORA_GET_DETECTOR_TYPE_H
#define LAR_PANDORA_GET_DETECTOR_TYPE_H 1

namespace lar_pandora {

  class LArPandoraDetectorType;

  namespace detector_functions {

    /**
         *  @brief  Factory class that returns the correct detector type interface
         *
         *  @result The detector type interface
         */
    LArPandoraDetectorType* GetDetectorType();

  } // namespace detector_functions

} // namespace lar_pandora

#endif
