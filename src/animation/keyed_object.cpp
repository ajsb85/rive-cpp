#include "rive/animation/keyed_object.hpp"
#include "rive/animation/keyed_property.hpp"
#include "rive/animation/linear_animation.hpp"
#include "rive/artboard.hpp"
#include "rive/importers/linear_animation_importer.hpp"

using namespace rive;

KeyedObject::KeyedObject() {}
KeyedObject::~KeyedObject() {}

void KeyedObject::addKeyedProperty(std::unique_ptr<KeyedProperty> property) {
    m_KeyedProperties.push_back(std::move(property));
}

StatusCode KeyedObject::onAddedDirty(CoreContext* context) {
    // Make sure we're keying a valid object.
    if (context->resolve(objectId()) == nullptr) {
        return StatusCode::MissingObject;
    }

    for (auto& property : m_KeyedProperties) {
        StatusCode code;
        if ((code = property->onAddedDirty(context)) != StatusCode::Ok) {
            return code;
        }
    }
    return StatusCode::Ok;
}

StatusCode KeyedObject::onAddedClean(CoreContext* context) {
    for (auto& property : m_KeyedProperties) {
        property->onAddedClean(context);
    }
    return StatusCode::Ok;
}

void KeyedObject::apply(Artboard* artboard, float time, float mix) {
    Core* object = artboard->resolve(objectId());
    if (object == nullptr) {
        return;
    }
    for (auto& property : m_KeyedProperties) {
        property->apply(object, time, mix);
    }
}

StatusCode KeyedObject::import(ImportStack& importStack) {
    auto importer = importStack.latest<LinearAnimationImporter>(LinearAnimationBase::typeKey);
    if (importer == nullptr) {
        return StatusCode::MissingObject;
    }
    // we transfer ownership of ourself to the importer!
    importer->addKeyedObject(std::unique_ptr<KeyedObject>(this));
    return Super::import(importStack);
}