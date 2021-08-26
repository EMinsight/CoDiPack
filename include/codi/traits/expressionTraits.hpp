#pragma once

#include <type_traits>

#include "../aux/macros.hpp"
#include "../config.h"
#include "../expressions/logic/compileTimeTraversalLogic.hpp"
#include "aux/enableIfHelpers.hpp"

/** \copydoc codi::Namespace */
namespace codi {

  template<typename T_Real, typename T_Impl>
  struct ExpressionInterface;

  template<typename T_Real, typename T_Gradient, typename T_Tape, typename T_Impl>
  struct LhsExpressionInterface;

  template<typename T_Real>
  struct ConstantExpression;

  template<typename T_Tape>
  struct StaticContextActiveType;

  /// Traits for everything that can be an expression e.g. codi::RealReverse, a + b, etc..
  namespace ExpressionTraits {

    /*******************************************************************************/
    /// @name Expression traits.
    /// @{

    /// Validates if the active type results of two expressions are the same or compatible. `void` results are
    /// interpreted as the active type result of a constant expression.
    template<typename ResultA, typename ResultB, typename = void>
    struct ValidateResultImpl {
      private:
        static bool constexpr isAVoid = std::is_same<void, ResultA>::value;
        static bool constexpr isBVoid = std::is_same<void, ResultB>::value;
        static bool constexpr isBothVoid = isAVoid & isBVoid;
        static bool constexpr isBothSame = std::is_same<ResultA, ResultB>::value;

        // Either one can be void (aka. constant value) but not both otherwise both need to be the same.
        static_assert((!isBothVoid) & (!isAVoid | !isBVoid | isBothSame), "Result types need to be the same.");

      public:

        /// The resulting active type of an expression.
        using ActiveResult = typename std::conditional<isBVoid, ResultA, ResultB>::type;
    };

    /// \copydoc ValidateResultImpl
    template<typename ResultA, typename ResultB>
    using ValidateResult = ValidateResultImpl<ResultA, ResultB>;

    /// @}
    /*******************************************************************************/
    /// @name Detection of specific node types
    /// @{

    /// If the expression inherits from ExpressionInterface. Is either std::false_type or std::true_type
    template<typename Expr, typename = void>
    struct IsExpression : std::false_type {};

#ifndef DOXYGEN_DISABLE
    template<typename Expr>
    struct IsExpression<Expr, typename enable_if_base_of<ExpressionInterface<typename Expr::Real, Expr>, Expr>::type>
        : std::true_type {};
#endif

#if CODI_IS_CPP14
    /// Value entry of IsExpression
    template<typename Expr>
    bool constexpr isExpression = IsExpression<Expr>::value;
#endif

    /// Enable if wrapper for IsExpression
    template<typename Expr, typename T = void>
    using EnableIfExpression = typename std::enable_if<IsExpression<Expr>::value, T>::type;

    /// If the expression inherits from LhsExpressionInterface. Is either std::false_type or std::true_type
    template<typename Expr, typename = void>
    struct IsLhsExpression : std::false_type {};

#ifndef DOXYGEN_DISABLE
    template<typename Expr>
    struct IsLhsExpression<
        Expr, typename enable_if_base_of<
                  LhsExpressionInterface<typename Expr::Real, typename Expr::Gradient, typename Expr::Tape, Expr>,
                  Expr>::type> : std::true_type {};

    template<typename Tape>
    struct IsLhsExpression<StaticContextActiveType<Tape>> : std::true_type {};
#endif

#if CODI_IS_CPP14
    /// Value entry of IsLhsExpression
    template<typename Expr>
    bool constexpr isLhsExpression = IsLhsExpression<Expr>::value;
#endif

    /// Enable if wrapper for IsLhsExpression
    template<typename Expr, typename T = void>
    using EnableIfLhsExpression = typename std::enable_if<IsLhsExpression<Expr>::value, T>::type;

    /// If the expression inherits from ConstantExpression. Is either std::false_type or std::true_type
    template<typename Expr>
    struct IsConstantExpression : std::false_type {};

#ifndef DOXYGEN_DISABLE
    template<typename Real>
    struct IsConstantExpression<ConstantExpression<Real>> : std::true_type {};
#endif

#if CODI_IS_CPP14
    template<typename Expr>
    /// Value entry of IsConstantExpression
    bool constexpr isConstantExpression = IsConstantExpression<Expr>::value;
#endif

    /// Enable if wrapper for IsConstantExpression
    template<typename Expr, typename T = void>
    using EnableIfConstantExpression = typename std::enable_if<IsConstantExpression<Expr>::value, T>::type;

    /// If the expression inherits from StaticContextActiveType. Is either std::false_type or std::true_type
    template<typename Expr>
    struct IsStaticContextActiveType : std::false_type {};

#ifndef DOXYGEN_DISABLE
    template<typename Tape>
    struct IsStaticContextActiveType<StaticContextActiveType<Tape>> : std::true_type {};
#endif

#if CODI_IS_CPP14
    /// Value entry of IsStaticContextActiveType
    template<typename Expr>
    bool constexpr isStaticContextActiveType = IsStaticContextActiveType<Expr>::value;
#endif

    /// Enable if wrapper for IsStaticContextActiveType
    template<typename Expr, typename T = void>
    using EnableIfStaticContextActiveType = typename std::enable_if<IsStaticContextActiveType<Expr>::value, T>::type;

    /// @}
    /*******************************************************************************/
    /// @name Static values on expressions
    /// @{

    /// Counts the number of nodes that inherit from LhsExpressionInterface in the expression.
    template<typename Expr>
    struct NumberOfActiveTypeArguments : public CompileTimeTraversalLogic<size_t, NumberOfActiveTypeArguments<Expr>> {
      public:

        /// \copydoc CompileTimeTraversalLogic::leaf()
        template<typename Node, typename = ExpressionTraits::EnableIfLhsExpression<Node>>
        CODI_INLINE static size_t constexpr leaf() {
          return 1;
        }
        using CompileTimeTraversalLogic<size_t, NumberOfActiveTypeArguments>::leaf;

        /// See NumberOfActiveTypeArguments
        static size_t constexpr value = NumberOfActiveTypeArguments::template eval<Expr>();
    };

#if CODI_IS_CPP14
    /// Value entry of NumberOfActiveTypeArguments
    template<typename Expr>
    bool constexpr numberOfActiveTypeArguments = NumberOfActiveTypeArguments<Expr>::value;
#endif

    /// Counts the number of types that inherit from ConstantExpression in the expression.
    template<typename Expr>
    struct NumberOfConstantTypeArguments
        : public CompileTimeTraversalLogic<size_t, NumberOfConstantTypeArguments<Expr>> {
      public:

        /// \copydoc CompileTimeTraversalLogic::leaf()
        template<typename Node, typename = EnableIfConstantExpression<Node>>
        CODI_INLINE static size_t constexpr leaf() {
          return 1;
        }
        using CompileTimeTraversalLogic<size_t, NumberOfConstantTypeArguments>::leaf;

        /// See NumberOfConstantTypeArguments
        static size_t constexpr value = NumberOfConstantTypeArguments::template eval<Expr>();
    };

#if CODI_IS_CPP14
    /// Value entry of NumberOfConstantTypeArguments
    template<typename Expr>
    bool constexpr numberOfConstantTypeArguments = NumberOfConstantTypeArguments<Expr>::value;
#endif

    /// @}
  }
}
