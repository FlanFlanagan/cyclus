// NullFacility.h
#if !defined(_NULLFACILITY_H)
#define _NULLFACILITY_H
#include <iostream>
#include <queue>

#include "FacilityModel.h"

/**
 * The NullFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility model does very little.  New material is added to queue inventory
 * and old material is removed from the same queue inventory.
 *
 */
class NullFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** 
   * Default constructor for the NullFacility class.
   */
  NullFacility() {};

  /**
   * Destructor for the NullFacility class. 
   */
  ~NullFacility() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(NullFacility* src);

  /**
   * Print information about this model
   */
  virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

public:
  /**
   * When the facility receives a message, execute any transaction therein
   */
    virtual void receiveMessage(Message* msg);

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:
    /**
     * This sends material up the Inst/Region/Logician line
     * to be passed back down to the receiver
     *
     * @param trans the Transaction object defining the order being filled
     * @param receiver the ultimate facility to receive this transaction
     */
    virtual void sendMaterial(Transaction trans, const Communicator* receiver);
    
    /**
     * The facility receives the materials other facilities have sent.
     *
     * @param trans the Transaction object defining the order being filled
     * @param manifest the list of material objects being received
     */
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);

    /**
     * The handleTick function specific to the NullFacility.
     * At each tick, it requests as much raw inCommod as it can process this
     * month and offers as much outCommod as it will have in its inventory by the
     * end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the NullFacility.
     * At each tock, it processes material and handles orders, and records this
     * month's actions.
     *
     * @param time the time of the tock
     */
    virtual void handleTock(int time);


/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

protected:
    /**
     * The NullFacility has one input commodity
     */
    Commodity* in_commod;

    /**
     * The NullFacility has one output commodity
     */
    Commodity* out_commod;

    /**
     * The NullFacility has a limit to how material it can process.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity;

    /**
     * The stocks of raw material available to be processed.
     */
    deque<Material*> stocks;
    
    /**
     * The inventory of processed material.
     */
    deque<Material*> inventory;

    /**
     * The list of orders to process on the Tock
     */
    deque<Message*> ordersWaiting;

    /**
     * get the total mass of the stuff in the inventory
     *
     * @return the total mass of the materials in storage
     */
    Mass checkInventory();

    /**
     * get the total mass of the stuff in the inventory
     *
     * @return the total mass of the materials in storage
     */
    Mass checkStocks();


    /**
     * The time that the stock material spends in the facility.
     */
    int residence_time;

    /**
     * The maximum size that the inventory can grow to.
     * The NullFacility must stop processing the material in its stocks 
     * when its inventory is full.
     */
    int inventory_size;

    /**
     * The receipe of input materials.
     */
    Material* in_recipe;

    /**
     * The receipe of the output material.
     */
    Material* out_recipe;

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new NullFacility();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif