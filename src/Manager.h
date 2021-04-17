/**
 * @file Manager.h
 * @author Joshua Tenorio
 * 
 * An abstract Manager class
 */

#pragma once

class Manager {
    public:
    Manager() {}
    virtual void OnStep();
};