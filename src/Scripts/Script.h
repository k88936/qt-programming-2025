//
// Created by root on 7/5/25.
//

#ifndef SCRIPT_H
#define SCRIPT_H
#include "entt/entity/entity.hpp"
#include "../Systems/ScriptSystem.h"
#include "../Utils/Registerable.h"

#include "boost/preprocessor/seq/for_each.hpp"
#include "boost/preprocessor/seq/enum.hpp"
#include "boost/preprocessor/seq/transform.hpp"
#include "boost/preprocessor/tuple/elem.hpp"

//------------------------------------------------ Helper macros -------------------------------------------------------
/*
    usage: DECLARE_COMPONENTS((Input)(Output))
    gen:

        Input* componentInput;
        Output* componentOutput;

        void bindComponents(const entt::entity entity)override
        {
            auto& registry = World::getInstance().registry;
            componentInput = const_cast<Input*>(&registry.get<Input>(entity));
            componentOutput = const_cast<Output*>(&registry.get<Output>(entity));
        }
*/
#define ADD_POINTER(r, data, elem) elem* BOOST_PP_CAT(component, elem) = nullptr;

#define ASSIGN_FIELD(r, data, elem) BOOST_PP_CAT(component, elem) = &registry.get<elem>(entity);

// Main macro
#define DECLARE_COMPONENTS(seq)                                                                                         \
protected:                                                                                                                 \
BOOST_PP_SEQ_FOR_EACH(ADD_POINTER, , seq)                                                                               \
private:                                                                                                                \
void bindComponents(entt::entity entity) {                                                                              \
    auto& registry = World::getInstance().registry;                                                                     \
    BOOST_PP_SEQ_FOR_EACH(ASSIGN_FIELD, , seq)                                                                          \
}
//--------------------------------------------- Script declaration macro -----------------------------------------------
/*  usage:
    SCRIPT(playerScript, (Input)(Output)(Transform))
*/

#define AUX_CLASS(className) BOOST_PP_CAT(aux_,BOOST_PP_CAT(className,_aux))
#define PREPEND_COMMA(r, data, elem) ,elem
#define TEMPLATE_PARAM(className,seq)<className BOOST_PP_SEQ_FOR_EACH(PREPEND_COMMA, , seq)>
#define SCRIPT(className,seq)                                                                                           \
template<typename... Components>                                                                                        \
class AUX_CLASS(className) :public Script, Registerable<AUX_CLASS(className)<Components... >> {                         \
    DECLARE_COMPONENTS(seq)                                                                                             \
public:                                                                                                                 \
static bool _register(){                                                                                                \
    ScriptSystem::getInstance().registerScript<Components...  >();                                                      \
    return true;                                                                                                        \
    }                                                                                                                   \
};                                                                                                                      \
class className :public AUX_CLASS(className)TEMPLATE_PARAM(className,seq)
//----------------------------------------------------------------------------------------------------------------------
class Script
{
public:
    virtual ~Script() = default;

    virtual void bindComponents(entt::entity entity)=0;
    virtual void update(const entt::entity entity)
    {
        bindComponents(entity);
    }

    virtual void init(const entt::entity entity)
    {
        bindComponents(entity);
    }
};
#endif //SCRIPT_H
