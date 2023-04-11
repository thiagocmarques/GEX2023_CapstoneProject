// New Brunswick Community College
// Gaming Experience Development
// -------------------------------
// Capstone Project - SeaQuest
// -------------------------------
// Instructor: David Burchill
// Student: Thiago Marques
// 
// April 2023
// 


#pragma once
#include <vector>
#include <map>
#include <memory>
#include <string>
#include "Entity.h"


//forward declaration
class Entity;

using NttPtr                = std::shared_ptr<Entity>;
using EntityVec             = std::vector<NttPtr>;
//using EntityMap             = std::map<std::string, EntityVec>;
using EntityMap             = std::map<EntityType, EntityVec>;

class EntityManager {
private:
    EntityVec               m_entities;
    EntityMap               m_entityMap;
    size_t                  m_totalEntites{ 0 };

    EntityVec               m_entitiesToAdd;

    void                    removeDeadEntities(EntityVec& v);
public:
    EntityManager();

    //NttPtr                  addEntity(const std::string& tag);
    NttPtr                  addEntity(const EntityType& tag);
    EntityVec&              getEntities();
    //EntityVec&              getEntities(const std::string& tag);
    EntityVec&              getEntities(const EntityType& tag);

    void                    update();

};
