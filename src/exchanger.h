#ifndef CYCLUS_EXCHANGER_H_
#define CYCLUS_EXCHANGER_H_

#include <set>

#include "bid_portfolio.h"
#include "generic_resource.h"
#include "material.h"
#include "request_portfolio.h"

namespace cyclus {

template <class T> class ResourceExchange;
  
/// @class Exchanger
///
/// @brief A simple API for agents that wish to exchange resources in the
/// simulation
///
/// An Exchanger is a mixin class designed for agents that wish to exchange
/// resources. It defines the API for the querying of requests, offers, and the
/// corresponding exchanges. 
class Exchanger {
 public:
  /// @brief default implementation for material requests
  virtual std::set< RequestPortfolio<Material> > AddMatlRequests() {
    return std::set< RequestPortfolio<Material> >();
  }
  
  /// @brief default implementation for generic resource requests
  virtual std::set< RequestPortfolio<GenericResource> > AddGenRsrcRequests() {
    return std::set< RequestPortfolio<GenericResource> >();
  }

  /// @brief default implementation for material requests
  virtual std::set< BidPortfolio<Material> >
      AddMatlBids(ResourceExchange<Material>* re) {
    return std::set< BidPortfolio<Material> >();
  }
  
  /// @brief default implementation for generic resource requests
  virtual std::set< BidPortfolio<GenericResource> >
      AddGenRsrcBids(ResourceExchange<GenericResource>* re) {
    return std::set< BidPortfolio<GenericResource> >();
  }
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGER_H_
