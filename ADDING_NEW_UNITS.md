# Adding New Units Guide

This guide demonstrates how to add new unit types with custom behaviors to the battle simulation system using the component-based architecture and strategy pattern.

## Architecture Overview

The system uses a **component-based architecture** where units are composed of different behavioral strategies:
- **Health Strategy**: Manages HP, damage, and healing
- **Movement Strategy**: Handles locomotion and pathfinding
- **Attack Strategy**: Defines combat capabilities
- **AI Strategy**: Controls autonomous decision-making

## Step-by-Step Guide: Adding a Tower Unit

Let's add a Tower unit based on the plans in README.md:

### 1. Define New Attributes (if needed)

If the unit needs new attributes, add them to `Types.hpp`:

```cpp
// Add to Types.hpp if needed
using PowerValue = uint32_t;  // For tower's power attribute
```

### 2. Create Health Strategy (if needed)

For most units, the existing `BasicHealthStrategy` is sufficient. Only create custom health if you need special mechanics like regeneration or immunities.

### 3. Create Movement Strategy (if needed)

Towers don't move, so we'll create a static movement strategy:

```cpp
// Add to MovementStrategies.hpp
class StaticMovementStrategy final : public IMovementStrategy
{
public:
    StaticMovementStrategy() = default;
    
    bool move(Entity& self, World& world, Position target, uint32_t turn) override
    {
        return false; // Towers cannot move
    }
    
    bool blocksGround() const override { return true; }
    uint32_t stepSize() const override { return 0; }
};

// Factory function
std::unique_ptr<IMovementStrategy> createStaticMovement();
```

### 4. Create Attack Strategy (if needed)

Towers use ranged attacks, so the existing `RangedAttackStrategy` works. For custom behavior, create new strategies:

```cpp
// Example: Custom tower attack with area effect
class TowerAttackStrategy final : public IAttackStrategy
{
public:
    TowerAttackStrategy(uint32_t damage, uint32_t range)
        : _damage(damage), _range(range) {}
    
    AttackType type() const override { return AttackType::Ranged; }
    uint32_t damage() const override { return _damage; }
    
    bool attack(Entity& self, Entity& target, World& world, uint32_t turn) override
    {
        // Custom tower attack logic here
        return world.executeAttack(self, target, turn, AttackType::Ranged);
    }
    
private:
    uint32_t _damage;
    uint32_t _range;
};
```

### 5. Create AI Strategy

Create a new AI strategy for the tower's behavior:

```cpp
// Add to AIStrategies.hpp
class TowerAIStrategy final : public IAIStrategy
{
public:
    bool update(Entity& self, World& world, uint32_t turn) override
    {
        // Find enemies in range (2-5 squares)
        auto enemies = sw::core::detail::gatherEnemies(self, world);
        
        // Filter enemies within tower range
        std::vector<Entity*> targetsInRange;
        for (auto* enemy : enemies)
        {
            uint32_t distance = self.position().distanceTo(enemy->position());
            if (distance >= 2 && distance <= 5)
            {
                targetsInRange.push_back(enemy);
            }
        }
        
        // Attack random target in range
        if (!targetsInRange.empty())
        {
            sw::core::detail::shuffleEnemies(targetsInRange);
            return world.executeAttack(self, *targetsInRange[0], turn, AttackType::Ranged);
        }
        
        return false; // No action taken
    }
};

// Factory function
std::unique_ptr<IAIStrategy> createTowerAI();
```

### 6. Create Prefab Factory Function

Add the tower creation function to `Prefabs.hpp`:

```cpp
// Add to Prefabs.hpp
inline std::unique_ptr<Entity> makeTower(UnitId id, Position pos, HealthPoints hp, PowerValue power)
{
    auto entity = std::make_unique<Entity>(id, pos, "Tower");
    entity->setHealth(createBasicHealth(hp));
    entity->setMovement(createStaticMovement()); // Cannot move
    entity->addAttack(createRangedAttack(power, 2, 5)); // Range 2-5
    entity->setAI(sw::core::createTowerAI());
    return entity;
}
```

### 7. Add Simulation Support

Add tower spawning to `Simulation.hpp` and `Simulation.cpp`:

```cpp
// Add to Simulation.hpp
bool spawnTower(UnitId unitId, uint32_t x, uint32_t y, HealthPoints hp, PowerValue power);

// Add to Simulation.cpp
bool Simulation::spawnTower(UnitId unitId, uint32_t x, uint32_t y, HealthPoints hp, PowerValue power)
{
    if (world_.getEntity(unitId))
    {
        return false;
    }
    
    auto entity = makeTower(unitId, Position{x, y}, hp, power);
    world_.addEntity(std::move(entity));
    return true;
}
```

### 8. Add Command Support

Create command structure and parsing:

```cpp
// Create src/IO/Commands/SpawnTower.hpp
#pragma once
#include "Command.hpp"

namespace sw::io
{
    struct SpawnTower
    {
        constexpr static const char* Name = "SPAWN_TOWER";
        
        uint32_t unitId{};
        uint32_t x{};
        uint32_t y{};
        uint32_t hp{};
        uint32_t power{};
        
        template <typename Visitor>
        void visit(Visitor& visitor)
        {
            visitor.visit("unitId", unitId);
            visitor.visit("x", x);
            visitor.visit("y", y);
            visitor.visit("hp", hp);
            visitor.visit("power", power);
        }
    };
}
```

### 9. Register Command in main.cpp

```cpp
// Add to main.cpp
parser.add<io::SpawnTower>(
    [&simulation](const io::SpawnTower& command)
    {
        if (!simulation.spawnTower(command.unitId, command.x, command.y, command.hp, command.power))
        {
            throw std::runtime_error("Failed to spawn tower");
        }
    });
```

## Advanced Example: Flying Unit (Raven)

For more complex units like the Raven (flying, doesn't block ground), you need custom strategies:

### Custom Movement Strategy
```cpp
class FlyingMovementStrategy final : public IMovementStrategy
{
public:
    explicit FlyingMovementStrategy(uint32_t step = 2) : step_(step) {}
    
    bool move(Entity& self, World& world, Position target, uint32_t turn) override
    {
        // Flying units can move 2 squares and don't check for blocking
        // Custom pathfinding logic here
        return true;
    }
    
    bool blocksGround() const override { return false; } // Flying!
    uint32_t stepSize() const override { return _step; }
    
private:
    uint32_t _step;
};
```

### Custom Health Strategy
```cpp
class FlyingHealthStrategy final : public IHealthStrategy
{
public:
    explicit FlyingHealthStrategy(uint32_t hp) : _hp(hp) {}
    
    bool isAlive() const override { return _hp > 0; }
    uint32_t hitPoints() const override { return _hp; }
    void applyDamage(int amount) override { _hp -= amount; }
    void heal(uint32_t amount) override { _hp += amount; }
    
    bool canBeAttackedBy(AttackType attackType) const override
    {
        // Flying units cannot be attacked by melee
        return attackType == AttackType::Ranged;
    }
    
    uint32_t getModifiedRange(uint32_t originalRange, AttackType attackType) const override
    {
        // Reduce ranged attack range against flying units by 1
        if (attackType == AttackType::Ranged && originalRange > 1)
        {
            return originalRange - 1;
        }
        return originalRange;
    }
    
private:
    int _hp;
};
```

## Testing Your New Unit

1. **Build the project**: `cmake --build cmake-build-debug`
2. **Create test scenario**: Add commands to `commands_example.txt`
3. **Run simulation**: `./cmake-build-debug/sw_battle_test commands_example.txt`
4. **Verify behavior**: Check that events are logged correctly

## Best Practices

1. **Reuse existing strategies** when possible
2. **Follow naming conventions**: `UnitTypeStrategy` pattern
3. **Add comprehensive documentation** for new strategies
4. **Test thoroughly** with different scenarios
5. **Consider edge cases** (empty maps, single units, etc.)
6. **Maintain consistency** with existing code style

## Architecture Benefits

This component-based approach provides:
- **Flexibility**: Mix and match different behaviors
- **Extensibility**: Easy to add new strategies
- **Maintainability**: Clear separation of concerns
- **Testability**: Each strategy can be tested independently
- **Scalability**: Multiple developers can work on different strategies simultaneously

The system is designed to handle the planned expansion to 10+ junior developers working simultaneously on new features!
