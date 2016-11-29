#pragma once
#include <Colonization/BuildConfiguration.hpp>
#ifdef NDEBUG
    #define CHECK_ANGELSCRIPT_RETURN(expression) expression
#else
    #define CHECK_ANGELSCRIPT_RETURN(expression) assert ((expression) >= 0)
#endif

#define GLOBAL_CONSTANT_GETTER(type, constant) \
    type Get ## constant () \
    {\
        return constant; \
    } \

#define BIND_GLOBAL_CONSTANT(asEngine, asType, constant_namespace, constant) \
    CHECK_ANGELSCRIPT_RETURN ( \
                asEngine->RegisterGlobalFunction ( \
                        (#asType + Urho3D::String (" get_") + #constant_namespace + Urho3D::String ("__")\
                            + #constant + Urho3D::String (" ()")).CString (), \
                        asFUNCTION (constant_namespace::Get ## constant), asCALL_CDECL) \
                ) \

#define OBJECT_PROPERTY_GETTER(objectType, propertyType, propertyName) \
    propertyType objectType ## _get_ ## propertyName (objectType * object) \
    { \
    return object->propertyName; \
    } \

#define OBJECT_PROPERTY_SETTER(objectType, propertyType, propertyName) \
    void objectType ## _set_ ## propertyName (objectType * object, propertyType value) \
    { \
    object->propertyName = value; \
    } \

#define OBJECT_PROPERTY_GETTER_AND_SETTER(objectType, propertyType, propertyName) \
    OBJECT_PROPERTY_GETTER (objectType, propertyType, propertyName) \
    OBJECT_PROPERTY_SETTER (objectType, propertyType, propertyName) \

#define BIND_OBJECT_PROPERTY_GETTER(asEngine, cxxObjectType, asObjectType, asPropertyType, propertyName) \
    CHECK_ANGELSCRIPT_RETURN ( \
                asEngine->RegisterObjectMethod (#asObjectType, (#asPropertyType + Urho3D::String (" get_") \
                + #propertyName + Urho3D::String (" ()")).CString (), asFUNCTION (cxxObjectType ## _get_ ## propertyName ), \
                asCALL_CDECL_OBJFIRST) \
                ) \


#define BIND_OBJECT_PROPERTY_SETTER(asEngine, cxxObjectType, asObjectType, asPropertyType, propertyName) \
    CHECK_ANGELSCRIPT_RETURN ( \
                asEngine->RegisterObjectMethod (#asObjectType, (Urho3D::String ("void set_") \
                + #propertyName + Urho3D::String (" (") + #asPropertyType \
                + Urho3D::String (" value)")).CString (), asFUNCTION (cxxObjectType ## _set_ ## propertyName ), \
                asCALL_CDECL_OBJFIRST) \
                ) \

#define BIND_OBJECT_PROPERTY_GETTER_AND_SETTER(asEngine, cxxObjectType, asObjectType, asPropertyType, propertyName) \
    BIND_OBJECT_PROPERTY_GETTER (asEngine, cxxObjectType, asObjectType, asPropertyType, propertyName); \
    BIND_OBJECT_PROPERTY_SETTER (asEngine, cxxObjectType, asObjectType, asPropertyType, propertyName)

#define OBJECT_ARRAY_PROPERTY_ACESSOR(objectType, arrayValuesType, propertyName) \
    arrayValuesType objectType ## _get_value_of_ ## propertyName ## _at_index (objectType * object, int index) \
    { \
        assert (index < object->propertyName.Size ()); \
        return object->propertyName.At (index); \
    } \
    \
    int objectType ## _get_size_of_ ## propertyName (objectType * object) \
    { \
        return object->propertyName.Size (); \
    } \
    \
    int objectType ## _set_value_of_ ## propertyName ## _at_index (objectType *object, int index, arrayValuesType value) \
    { \
        if (index < object->propertyName.Size ()) \
        { \
            object->propertyName [index] = value; \
            return index; \
        } \
        else \
        { \
            object->propertyName.Push (value); \
            return (object->propertyName.Size () - 1); \
        } \
    }

#define BIND_OBJECT_ARRAY_PROPERTY(asEngine, cxxObjectType, asObjectType, asArrayValuesType, propertyName) \
    CHECK_ANGELSCRIPT_RETURN ( \
                asEngine->RegisterObjectMethod (#asObjectType, (#asArrayValuesType + Urho3D::String (" get_array_") \
                + #propertyName + Urho3D::String ("_value (int index)")).CString (), \
                asFUNCTION (cxxObjectType ## _get_value_of_ ## propertyName ## _at_index), \
                asCALL_CDECL_OBJFIRST) \
                ); \
    CHECK_ANGELSCRIPT_RETURN ( \
                asEngine->RegisterObjectMethod (#asObjectType, (Urho3D::String ("int get_array_") \
                + #propertyName + Urho3D::String ("_size ()")).CString (), \
                asFUNCTION (cxxObjectType ## _get_size_of_ ## propertyName), \
                asCALL_CDECL_OBJFIRST) \
                ); \
    CHECK_ANGELSCRIPT_RETURN ( \
                asEngine->RegisterObjectMethod (#asObjectType, (Urho3D::String ("int set_array_") \
                + #propertyName + Urho3D::String ("_value (int index, ") + #asArrayValuesType \
                + Urho3D::String (" value)")).CString (), \
                asFUNCTION (cxxObjectType ## _set_value_of_ ## propertyName ## _at_index), \
                asCALL_CDECL_OBJFIRST) \
                )
