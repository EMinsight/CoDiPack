#pragma once

#include "../../config.h"
#include "../../aux/macros.hpp"

/** \copydoc codi::Namespace */
namespace codi {

  template<typename Logic>
  struct TraversalLogic;

  template<typename _Impl>
  struct NodeInterface {
    public:

      using Impl = CODI_DECLARE_DEFAULT(_Impl, NodeInterface);

      CODI_INLINE Impl const& cast() const {
        return static_cast<Impl const&>(*this);
      }

      /*******************************************************************************
       * Section: Start of interface definition
       *
       */

      static bool constexpr EndPoint = CODI_UNDEFINED_VALUE;

      template<typename Logic, typename ... Args>
      CODI_INLINE void forEachLink(TraversalLogic<Logic>& logic, Args&& ... args) const;

      template<typename Logic, typename ... Args>
      CODI_INLINE static typename Logic::ResultType constexpr forEachLinkConstExpr(Args&& ... args);
  };
}
