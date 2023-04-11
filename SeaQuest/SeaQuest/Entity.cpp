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

#include "Entity.h"




Entity::Entity(size_t id, const EntityType& tag)
    : m_tag(tag), m_id(id)
{}


void Entity::destroy() {
    m_active = false;
}


const size_t& Entity::getId() const
{
    return m_id;
}



const EntityType& Entity::getTag() const
{
    return m_tag;
}


bool Entity::isActive() const
{
    return m_active;
}