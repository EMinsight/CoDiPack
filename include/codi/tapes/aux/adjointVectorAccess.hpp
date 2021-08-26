#pragma once

#include <cstddef>

#include "../../aux/macros.hpp"
#include "../../config.h"
#include "../../tools/data/direction.hpp"
#include "../../traits/realTraits.hpp"
#include "vectorAccessInterface.hpp"

/** \copydoc codi::Namespace */
namespace codi {

  /**
   * @brief Implementation of VectorAccessInterface for adjoint vectors.
   *
   * The adjoint vector is used as is, it is assumed to have the correct size. No bounds checking is performed.
   *
   * @tparam T_Real        The computation type of a tape, usually chosen as ActiveType::Real.
   * @tparam T_Identifier  The adjoint/tangent identification of a tape, usually chosen as ActiveType::Identifier.
   * @tparam T_Gradient    The gradient type of a tape, usually chosen as ActiveType::Gradient.
   */
  template<typename T_Real, typename T_Identifier, typename T_Gradient>
  struct AdjointVectorAccess : public VectorAccessInterface<T_Real, T_Identifier> {
      using Real = CODI_DD(T_Real, double);           ///< See AdjointVectorAccess.
      using Identifier = CODI_DD(T_Identifier, int);  ///< See AdjointVectorAccess.
      using Gradient = CODI_DD(T_Gradient, double);   ///< See AdjointVectorAccess.

    private:

      Gradient* adjointVector;  ///< Pointer to the gradient vector.

      Gradient lhs;  ///< Temporary storage for indirect adjoint or tangent updates.

    public:

      /// Constructor. See interface documentation for details about the adjoint vector.
      AdjointVectorAccess(Gradient* adjointVector) : adjointVector(adjointVector), lhs() {}

      /*******************************************************************************/
      /// @name Misc

      /// \copydoc codi::VectorAccessInterface::getVectorSize
      size_t getVectorSize() const {
        return GradientTraits::dim<Gradient>();
      }

      /// \copydoc codi::VectorAccessInterface::isLhsZero
      bool isLhsZero() {
        return RealTraits::isTotalZero(lhs);
      }

      /*******************************************************************************/
      /// @name Indirect adjoint access

      /// \copydoc codi::VectorAccessInterface::setLhsAdjoint
      void setLhsAdjoint(Identifier const& index) {
        lhs = adjointVector[index];
        adjointVector[index] = Gradient();
      }

      /// \copydoc codi::VectorAccessInterface::updateAdjointWithLhs
      void updateAdjointWithLhs(Identifier const& index, Real const& jacobian) {
        adjointVector[index] += jacobian * lhs;
      }

      /*******************************************************************************/
      /// @name Indirect tangent access

      /// \copydoc codi::VectorAccessInterface::setLhsTangent
      void setLhsTangent(Identifier const& index) {
        adjointVector[index] = lhs;
        lhs = Gradient();
      }

      /// \copydoc codi::VectorAccessInterface::updateTangentWithLhs
      void updateTangentWithLhs(Identifier const& index, Real const& jacobian) {
        lhs += jacobian * adjointVector[index];
      }

      /*******************************************************************************/
      /// @name Direct adjoint access

      /// \copydoc codi::VectorAccessInterface::resetAdjoint
      void resetAdjoint(Identifier const& index, size_t dim) {
        GradientTraits::at(adjointVector[index], dim) = typename GradientTraits::Real<Gradient>();
      }

      /// \copydoc codi::VectorAccessInterface::resetAdjointVec
      void resetAdjointVec(Identifier const& index) {
        adjointVector[index] = Gradient();
      }

      /// \copydoc codi::VectorAccessInterface::getAdjoint
      Real getAdjoint(Identifier const& index, size_t dim) {
        CODI_UNUSED(dim);

        return (Real)GradientTraits::at(adjointVector[index], dim);
      }

      /// \copydoc codi::VectorAccessInterface::getAdjointVec
      void getAdjointVec(Identifier const& index, Real* const vec) {
        for (size_t i = 0; i < getVectorSize(); ++i) {
          vec[i] = (Real)GradientTraits::at(adjointVector[index], i);
        }
      }

      /// \copydoc codi::VectorAccessInterface::updateAdjoint
      void updateAdjoint(Identifier const& index, size_t dim, Real const& adjoint) {
        GradientTraits::at(adjointVector[index], dim) += adjoint;
      }

      /// \copydoc codi::VectorAccessInterface::updateAdjointVec
      void updateAdjointVec(Identifier const& index, Real const* const vec) {
        for (size_t i = 0; i < getVectorSize(); ++i) {
          GradientTraits::at(adjointVector[index], i) += vec[i];
        }
      }

      /*******************************************************************************/
      /// @name Primal access

      /// \copydoc codi::VectorAccessInterface::setPrimal <br><br>
      /// Implementation: Not implemented, empty function.
      void setPrimal(Identifier const& index, Real const& primal) {
        CODI_UNUSED(index, primal);
      }

      /// \copydoc codi::VectorAccessInterface::getPrimal <br><br>
      /// Implementation: Not implemented, returns zero.
      Real getPrimal(Identifier const& index) {
        CODI_UNUSED(index);

        return Real();
      }

      /// \copydoc codi::VectorAccessInterface::setPrimal <br><br>
      /// Implementation: Always returns false.
      bool hasPrimals() {
        return false;
      }
  };
}
