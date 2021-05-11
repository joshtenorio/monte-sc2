/**
 * @file api.h
 * @author Joshua Tenorio
 * 
 * Contains the Observation and Action interfaces for use by managers
 */

#pragma once
#include <sc2api/sc2_control_interfaces.h>
#include <sc2api/sc2_interfaces.h>
#include <memory>

using namespace sc2;

class Interface {
    public:
    Interface(const ObservationInterface* observation_,
        ActionInterface* actions_,
        QueryInterface* query_){
            observation = observation_;
            actions = actions_;
            query = query_;
        };
    
    const ObservationInterface* observation;
    ActionInterface* actions;
    QueryInterface* query;
};

extern std::unique_ptr<Interface> gInterface;