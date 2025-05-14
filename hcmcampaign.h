/*
 * Ho Chi Minh City University of Technology
 * Faculty of Computer Science and Engineering
 * Initial code for Assignment 2
 * Programming Fundamentals Spring 2025
 * Date: 02.02.2025
 */

// The library here is concretely set, students are not allowed to include any other libraries.
#ifndef _H_HCM_CAMPAIGN_H_
#define _H_HCM_CAMPAIGN_H_

#include "main.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
class Unit;
class UnitList;
class Army;
class TerrainElement;

class Vehicle;
class Infantry;

class LiberationArmy;
class ARVN;

class Position;

class Road;
class Mountain;
class River;
class Urban;
class Fortification;
class SpecialZone;

class BattleField;

class HCMCampaign;
class Configuration;

enum VehicleType
{
    TRUCK,
    MORTAR,
    ANTIAIRCRAFT,
    ARMOREDCAR,
    APC,
    ARTILLERY,
    TANK
};
enum InfantryType
{
    SNIPER,
    ANTIAIRCRAFTSQUAD,
    MORTARSQUAD,
    ENGINEER,
    SPECIALFORCES,
    REGULARINFANTRY
};

class Army
{
protected:
    int LF, EXP;
    string name;
    UnitList* unitList;
    BattleField* battleField;

public:
    Army(Unit** unitArray, int size, string name, BattleField* battleField);
    virtual void fight(Army* enemy, bool defense = false) = 0;
    virtual string str() const = 0;
    UnitList* getUnitList() {
        return unitList;
    }
    void addEXP(int amount) {
        EXP = static_cast<int>(ceil(EXP + amount));
    }
    void subEXP(int amount) {
        EXP = static_cast<int>(ceil(EXP - amount));
    }
    void addLF(int amount) {
        LF = static_cast<int>(ceil(LF + amount));
    }

    void subLF(int amount) {
        LF = static_cast<int>(ceil(LF - amount));
    }
};

class Position
{
private:
    int r, c;

public:
    Position(int r = 0, int c = 0);
    Position(const string& str_pos); // Example: str_pos = "(1,15)"
    int getRow() const;
    int getCol() const;
    void setRow(int r);
    void setCol(int c);
    string str() const; // Example: returns "(1,15)"
};

class Unit
{
protected:
    int quantity, weight;
    Position pos;

public:
    Unit(int quantity, int weight, Position pos);
    virtual ~Unit();
    virtual int getAttackScore() = 0;
    Position getCurrentPosition() const;
    virtual string str() const = 0;
    virtual bool isVehicle() const = 0;
    virtual void addQuantity(int q) = 0;
    virtual int getQuantity() const = 0;
    int getWeight() const {
        return weight;
    }

    void setWeight(int newWeight) {
        weight = newWeight;
    }

};

class UnitList
{
private:
    int capacity;
    struct Node {
        Unit* unit;
        Node* next;

        Node(Unit* unit) : unit(unit), next(nullptr) {}
    };

    Node* head;
    bool isSpecialNumber(int n, int k);
    bool isSpecialForAnyOddPrimeBase(int s);

public:
    UnitList(int capacity);
    ~UnitList();
    bool insert(Unit* unit);                   // return true if insert successfully
    bool isContain(VehicleType vehicleType);   // return true if it exists
    bool isContain(InfantryType infantryType); // return true if it exists
    string str() const;
    UnitList(int LF, int EXP);
    Node* getHead() const;
    int getCapacity() const;
    friend class ARVN;
    friend class LiberationArmy;
    bool remove(Unit* u);
    friend class Forest;
    friend class River;
    friend class Urban;
    friend class Fortification;
    friend class SpecialZone;


};

class TerrainElement
{
public:
    TerrainElement();
    ~TerrainElement();
    virtual void getEffect(Army* army) = 0;
};


class Vehicle : public Unit {
private:
    VehicleType vehicleType;
public:
    Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType);
    int getAttackScore();
    string str() const;
    VehicleType getType() const;
    bool isVehicle() const override;
    void addQuantity(int q) override;
    int getQuantity() const override;
    friend class UnitList;
    friend class ARVN;

};


class Infantry : public Unit {
private:
    InfantryType infantryType;
public:
    Infantry(int quantity, int weight, const Position pos, InfantryType infantryType);
    int getAttackScore();
    string str() const;
    InfantryType getType() const;
    bool isVehicle() const override;
    void addQuantity(int q) override;
    int getQuantity() const override;
    friend class UnitList;
    friend class ARVN;


};


class LiberationArmy : public Army {
public:
    LiberationArmy(Unit** unitArray, int size, string name, BattleField* battleField);
    void fight(Army* enemy, bool defense = false);
    string str() const;
    void addUnit(Unit* unit) {
        unitList->insert(unit);
    }
};





class ARVN : public Army {
public:
    ARVN(Unit** unitArray, int size, string name, BattleField* battleField);
    void fight(Army* enemy, bool defense = false);
    string str() const;
};

class Road : public TerrainElement
{
public:
    Road();
    ~Road();
    void getEffect(Army* army);
};
class Forest : public TerrainElement
{
private:

    Position pos;

public:
    Forest();
    ~Forest();
    void getEffect(Army* army) override;
};

class River : public TerrainElement
{
private:

    Position pos;
public:
    River();
    ~River();
    void getEffect(Army* army) override;
};
class Urban : public TerrainElement
{
private:

    Position pos;
public:
    Urban();
    ~Urban();
    void getEffect(Army* army);
};

class Fortification : public TerrainElement
{
private:

    Position pos;
public:
    Fortification();
    ~Fortification();
    void getEffect(Army* army);
};
class SpecialZone : public TerrainElement
{
private:

    Position pos;
public:
    SpecialZone();
    ~SpecialZone();
    void getEffect(Army* army);
};


class BattleField
{
private:
    int n_rows, n_cols;
    TerrainElement*** terrain;
public:
    BattleField(int n_rows, int n_cols, vector<Position*> arrayForest,
        vector<Position*> arrayRiver, vector<Position*> arrayFortification,
        vector<Position*> arrayUrban, vector<Position*> arraySpecialZone);
    ~BattleField();
    string str() const;
    void applyTerrainEffects(Army* army);
};

class HCMCampaign
{
private:
    Configuration* config;
    BattleField* battleField;
    LiberationArmy* liberationArmy;
    ARVN* arvn;

public:
    HCMCampaign(const string& config_file_path);
    void run();
    string printResult();
};


class Configuration {
private:

    int num_rows, num_cols;
    vector<Position*> arrayForest, arrayRiver, arrayFortification, arrayUrban, arraySpecialZone;

    Unit** liberationUnits;
    int numLiberationUnits;

    Unit** ARVNUnits;
    int numARVNUnits;

    BattleField* battleField;
    LiberationArmy* liberationArmy;
    ARVN* arvn;

    int eventCode;
public:
    Configuration(const string& filepath);
    ~Configuration();
    string str() const;
    BattleField* getBattleField() const { return battleField; }
    LiberationArmy* getLiberationArmy() const { return liberationArmy; }
    ARVN* getARVN() const { return arvn; }
    int getEventCode() const { return eventCode; }

};

#endif