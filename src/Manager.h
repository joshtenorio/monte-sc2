/**
 * @file Manager.h
 * @author Joshua Tenorio
 * 
 * An abstract Manager class
 */

#pragma once

#include "Logger.h"

class Manager {
    public:
    Manager() { logger = Logger(); };
    virtual void OnStep() = 0;
    protected:
    Logger logger;
};