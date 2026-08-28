#pragma once

class Damageable {
public:
    virtual ~Damageable() = default;
    virtual void takeDamage(float amount) = 0;
    virtual bool isAlive() const = 0;
    virtual float getAttackPower() const { return 0.f; }
    virtual void setAttacking(bool) {}
};