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
using EntityMap             = std::map<EntityEnum, EntityVec>;

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
    NttPtr                  addEntity(const EntityEnum& tag);
    EntityVec&              getEntities();
    //EntityVec&              getEntities(const std::string& tag);
    EntityVec&              getEntities(const EntityEnum& tag);

    void                    update();

};
