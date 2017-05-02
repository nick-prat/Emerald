# Emerald
### Entity Component System

##### Installation

To instal copy this folder to your project directory or an include directory

##### Components

To create a component all you need to do is create a class! For example

```c++
class CThing {
public:
    void saySomething() {
        std::cout << "something\n";
    }
};

class CThing2 {
    void saySomethingElse() {
        std::cout << "something else\n";
    }
};
```

##### Entity Manager

Now to create an entity manager that can use these components you would define it as

```c++
Emerald::EntityManager<CThing, CThing2> entMan;
```

Idealy this will be changed in a later release so you won't have to define all component types

##### Systems

Do create a system all that is needed is for you to create a class and inherit

```c++
Emerald::IBaseSystem<system_t>
```

where system_t is the type of the system you are creating, then you need to implement the method

```c++
template<typename entity_manager_t>
void update(entity_manager_t& entMan) {}
```
in that function you can use the method

```c++
entMan.mapEntities(m_entities, [](auto& ent) {

});
```

to act on all subscribed entities

and example system would be

```c++
class ASystem : ISystem<ASystem> {
public:
    template<typename entity_manager_t>
    void update(entity_manager_t& entMan) {
        entMan.mapEntities(m_entities, [](auto& ent) {
            auto& ct = ent.getComponent<CThing>();
            ct.saySomething();
        });
    }
}
```

to register that system to the entity manager all you need to do is

```c++
entMan.registerSystem<ASystem>()
```

Be careful though, there can only be one system of each type in a single entity manager

And now if you want to update the registered systems just call the function

```c++
entMan.updateSystems(time_scale);
```

##### Entities

To create an entity all you need to do is

```c++
auto id = m_entityManager.createEntity();
```

Emerald returns id's because it can change the location of an entity if many are added, because it stores the components continuously in memory

To get an entity and change it's component composition, do this

```c++
auto& ent = m_entityManager.getEntity(id);
```

and then you can use functions like

```c++
ent.addComponent<CThing>();
auto& comp = ent.getComponent<CThing>();
ent.removeComponent<CThing>();
```

Just be careful not to hold on to that reference for very long, because it could be invalidated

If you want to subscribe your entity to a system, do this

```c++
m_entityManager.getSystem<ASystem>().subscribe(id);
```

Where the id is the entities id

##### Disclaimer

This is in alpha, so I wouldn't count on it working perfectly under heavy load or multithreaded applications
