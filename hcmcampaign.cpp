#include "hcmcampaign.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
////////////////////////////////////////////////////////////////////////

Unit::Unit(int quantity, int weight, const Position pos) {
    this->quantity = quantity;
    this->weight = weight;
    this->pos = pos;
}

Unit::~Unit() {}

Position Unit::getCurrentPosition() const {
    return pos;
}

// VEHICLE ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vehicle::Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType) : Unit(quantity, weight, pos) {

    this->vehicleType = vehicleType;

}
bool Vehicle::isVehicle() const { return true; }
void Vehicle::addQuantity(int q) { Unit::quantity += q; }
int Vehicle::getQuantity() const { return Unit::quantity; }
VehicleType Vehicle::getType() const {
    return vehicleType;
}
int Vehicle::getAttackScore() {
    return static_cast<int>(vehicleType) * 304 + quantity * weight / 30;
}
string Vehicle::str() const {
    ostringstream oss;
    oss << "Vehicle[vehicleType=" << static_cast<int>(vehicleType)
        << ",quantity=" << Unit::quantity
        << ",weight=" << Unit::weight
        << ",pos=" << Unit::pos.str()
        << "]";
    return oss.str();
}

// INFANTRY //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Infantry::Infantry(int quantity, int weight, const Position pos, InfantryType infantryType) : Unit(quantity, weight, pos) {

    this->infantryType = infantryType;
}

void Infantry::addQuantity(int q) { Unit::quantity += q; }
int Infantry::getQuantity() const { return Unit::quantity; }
InfantryType Infantry::getType() const { return infantryType; }
bool Infantry::isVehicle() const {
    return false;
}
int Infantry::getAttackScore() {

    int score = static_cast<int>(infantryType) * 56 + quantity * weight;


    if (infantryType == InfantryType::SPECIALFORCES) {

        int sqrtWeight = static_cast<int>(sqrt(weight));
        if (sqrtWeight * sqrtWeight == weight) {
            score += 75;
        }


        int year = 1975;
        auto calculatePersonalNumber = [](int num) {
            while (num >= 10) {
                int sum = 0;
                while (num > 0) {
                    sum += num % 10;
                    num /= 10;
                }
                num = sum;
            }
            return num;
            };

        int personalNumber = calculatePersonalNumber(weight) + calculatePersonalNumber(year);
        personalNumber = calculatePersonalNumber(personalNumber);


        if (personalNumber > 7) {
            quantity += static_cast<int>(quantity * 0.2);
        }
        else if (personalNumber < 3) {
            quantity -= static_cast<int>(quantity * 0.1);
        }


        score = static_cast<int>(infantryType) * 56 + quantity * weight;
    }

    return score;

}

string Infantry::str() const {
    ostringstream oss;
    oss << "Infantry[infantryType=" << static_cast<int>(infantryType)
        << ",quantity=" << Unit::quantity
        << ",weight=" << Unit::weight
        << ",pos=" << Unit::pos.str()
        << "]";
    return oss.str();
}

// QUÂN ĐỘI ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Army::Army(Unit** unitArray, int size, string name, BattleField* battleField) {
    this->name = name;
    this->battleField = battleField;
    this->unitList = new UnitList(size);
    this->LF = 0;
    this->EXP = 0;


    for (int i = 0; i < size; ++i) {
        Unit* unit = unitArray[i];
        if (unitList->insert(unit)) {
            if (dynamic_cast<Vehicle*>(unit)) {
                LF += unit->getAttackScore();
            }
            else if (dynamic_cast<Infantry*>(unit)) {
                EXP += unit->getAttackScore();
            }
        }
    }

    LF = min(1000, max(0, LF));
    EXP = min(500, max(0, EXP));
}


// QUÂN ĐỘI GIẢI PHÓNG ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



bool UnitList::remove(Unit* unit) {
    Node* current = head;
    Node* prev = nullptr;

    while (current) {
        if (current->unit == unit) {
            // Nếu là node đầu tiên
            if (prev == nullptr) {
                head = current->next;
            }
            else {
                prev->next = current->next;
            }

            // Giải phóng bộ nhớ của node hiện tại
            delete current->unit; // Xóa đơn vị quân sự
            delete current;       // Xóa node
            return true;          // Xóa thành công
        }

        // Tiếp tục duyệt
        prev = current;
        current = current->next;
    }

    return false; // Không tìm thấy đơn vị cần xóa
}


LiberationArmy::LiberationArmy(Unit** unitArray, int size, string name, BattleField* battleField)
    : Army(unitArray, size, name, battleField) {
}

void LiberationArmy::fight(Army* enemy, bool defense) {
    if (!defense) {
        // Tấn công → nhân hệ số
        this->LF = static_cast<int>(this->LF * 1.5);
        this->EXP = static_cast<int>(this->EXP * 1.5);

        // Tính EXP và LF của địch
        int enemyEXP = 0, enemyLF = 0;
        for (UnitList::Node* node = enemy->getUnitList()->getHead(); node; node = node->next) {
            if (node->unit->isVehicle())
                enemyLF += node->unit->getAttackScore();
            else
                enemyEXP += node->unit->getAttackScore();
        }

        // Tìm tổ hợp A (bộ binh) có tổng điểm nhỏ nhất lớn hơn enemyEXP
        vector<Unit*> comboA;
        int sumA = 0;
        for (UnitList::Node* node = unitList->getHead(); node; node = node->next) {
            if (!node->unit->isVehicle()) {
                comboA.push_back(node->unit);
                sumA += node->unit->getAttackScore();
                if (sumA > enemyEXP) break;
            }
        }

        // Tìm tổ hợp B (phương tiện) có tổng điểm nhỏ nhất lớn hơn enemyLF
        vector<Unit*> comboB;
        int sumB = 0;
        for (UnitList::Node* node = unitList->getHead(); node; node = node->next) {
            if (node->unit->isVehicle()) {
                comboB.push_back(node->unit);
                sumB += node->unit->getAttackScore();
                if (sumB > enemyLF) break;
            }
        }

        bool hasA = sumA > enemyEXP;
        bool hasB = sumB > enemyLF;

        if (hasA && hasB) {
            // THẮNG hoàn toàn
            for (Unit* u : comboA) {
                enemy->getUnitList()->remove(u); // Xóa khỏi địch
                this->unitList->insert(u);  // Thêm vào quân mình
            }
            for (Unit* u : comboB) {
                enemy->getUnitList()->remove(u);
                this->unitList->insert(u);
            }
        }
        else if (hasA || hasB) {
            bool win = false;

            if (hasA && this->EXP > enemyEXP) {
                win = true;
                for (Unit* u : comboA) {
                    enemy->getUnitList()->remove(u);
                    this->unitList->insert(u);
                }
                for (Unit* u : comboB) {
                    enemy->getUnitList()->remove(u); // Xóa luôn
                }
            }
            else if (hasB && this->LF > enemyLF) {
                win = true;
                for (Unit* u : comboB) {
                    enemy->getUnitList()->remove(u);
                    this->unitList->insert(u);
                }
                for (Unit* u : comboA) {
                    enemy->getUnitList()->remove(u); // Xóa luôn
                }
            }

            if (!win) {
                // Không đủ mạnh dù có combo → không giao tranh
                for (UnitList::Node* node = this->unitList->getHead(); node; node = node->next) {
                    node->unit->setWeight(static_cast<int>(node->unit->getWeight() * 0.9)); // Mất 10% trọng lượng
                }
            }
        }
        else {
            // Không có tổ hợp nào
            for (UnitList::Node* node = this->unitList->getHead(); node; node = node->next) {
                node->unit->setWeight(static_cast<int>(node->unit->getWeight() * 0.9));
            }
        }

        // Cập nhật lại EXP và LF
        this->EXP = 0;
        this->LF = 0;
        for (UnitList::Node* node = this->unitList->getHead(); node; node = node->next) {
            if (node->unit->isVehicle())
                this->LF += node->unit->getAttackScore();
            else
                this->EXP += node->unit->getAttackScore();
        }
    }
    else {
        // Phòng thủ
        this->LF = static_cast<int>(this->LF * 1.3);
        this->EXP = static_cast<int>(this->EXP * 1.3);

        // So sánh chỉ số chiến đấu
        int enemyEXP = 0, enemyLF = 0;
        for (UnitList::Node* node = enemy->getUnitList()->getHead(); node; node = node->next) {
            if (node->unit->isVehicle())
                enemyLF += node->unit->getAttackScore();
            else
                enemyEXP += node->unit->getAttackScore();
        }

        if (this->LF >= enemyLF && this->EXP >= enemyEXP) {
            // Chiến thắng
            return;
        }
        else if (this->LF < enemyLF || this->EXP < enemyEXP) {
            // Mất 10% số lượng
            for (UnitList::Node* node = this->unitList->getHead(); node; node = node->next) {
                node->unit->addQuantity(static_cast<int>(node->unit->getQuantity() * -0.1));
            }
        }
        else {
            // Cần chi viện → tăng số lượng lên số Fibonacci gần nhất
            for (UnitList::Node* node = this->unitList->getHead(); node; node = node->next) {
                int quantity = node->unit->getQuantity();
                int fib1 = 0, fib2 = 1;
                while (fib2 < quantity) {
                    int temp = fib2;
                    fib2 += fib1;
                    fib1 = temp;
                }
                node->unit->addQuantity(fib2 - quantity);
            }
        }

        // Cập nhật lại EXP và LF
        this->EXP = 0;
        this->LF = 0;
        for (UnitList::Node* node = this->unitList->getHead(); node; node = node->next) {
            if (node->unit->isVehicle())
                this->LF += node->unit->getAttackScore();
            else
                this->EXP += node->unit->getAttackScore();
        }
    }
}



string LiberationArmy::str() const {
    ostringstream oss;
    oss << "LiberationArmy[name=" << name
        << ",LF=" << LF
        << ",EXP=" << EXP
        << ",unitList=" << unitList->str()
        << ",battleField=" << battleField->str()
        << "]";
    return oss.str();
}








// ARVN ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARVN::ARVN(Unit** unitArray, int size, string name, BattleField* battleField)
    : Army(unitArray, size, name, battleField) {
}

void ARVN::fight(Army* enemy, bool defense) {
    if (!defense) {
        // Tạo danh sách mới để lưu các đơn vị còn lại
        UnitList* updatedUnitList = new UnitList(unitList->getCapacity());
        LF = 0;
        EXP = 0;

        // Duyệt qua danh sách các đơn vị
        UnitList::Node* current = unitList->getHead();
        while (current != nullptr) {
            Unit* unit = current->unit;

            if (unit != nullptr) {
                // Giảm số lượng (quantity) của đơn vị
                if (auto vehicle = dynamic_cast<Vehicle*>(unit)) {
                    int q = vehicle->getQuantity();
                    vehicle->addQuantity(-static_cast<int>(q * 0.2));  // giảm 20%
                }
                else if (auto infantry = dynamic_cast<Infantry*>(unit)) {
                    int q = infantry->getQuantity();
                    infantry->addQuantity(-static_cast<int>(q * 0.2));  // giảm 20%
                }

                // Nếu số lượng còn lớn hơn 1, thêm vào danh sách mới
                if (unit->getQuantity() > 1) {
                    updatedUnitList->insert(unit);

                    // Cập nhật lại chỉ số chiến đấu
                    if (unit->isVehicle()) {
                        LF += unit->getAttackScore();
                    }
                    else {
                        EXP += unit->getAttackScore();
                    }
                }
                else {
                    delete unit;
                }
            }

            current = current->next;
        }

        // Giới hạn LF và EXP
        LF = min(1000, max(0, LF));
        EXP = min(500, max(0, EXP));

        // Cập nhật danh sách đơn vị
        delete unitList;
        unitList = updatedUnitList;
    }

    // Trường hợp phòng thủ
    if (defense) {
        UnitList::Node* current = unitList->getHead();
        while (current != nullptr) {
            Unit* unit = current->unit;

            if (unit != nullptr) {
                // Giảm 20% trọng số
                int w = unit->isVehicle()
                    ? static_cast<Vehicle*>(unit)->getAttackScore() / static_cast<Vehicle*>(unit)->getQuantity()
                    : static_cast<Infantry*>(unit)->getAttackScore() / static_cast<Infantry*>(unit)->getQuantity();

                int newWeight = static_cast<int>(w * 0.8);
                if (unit->isVehicle()) {
                    static_cast<Vehicle*>(unit)->addQuantity(0); // chỉ để dùng method tránh trực tiếp
                    static_cast<Vehicle*>(unit)->weight = newWeight; // chỉ nếu weight là public hoặc friend
                }
                else {
                    static_cast<Infantry*>(unit)->addQuantity(0);
                    static_cast<Infantry*>(unit)->weight = newWeight;
                }

                UnitList::Node* temp = current;
                current = current->next;

                // Chuyển đơn vị sang quân địch
                dynamic_cast<LiberationArmy*>(enemy)->addUnit(unit);
                delete temp;  // Xóa node, KHÔNG xóa unit vì đã chuyển đi
            }
            else {
                current = current->next;
            }
        }

        delete unitList;
        unitList = new UnitList(0); // Rỗng
        LF = 0;
        EXP = 0;
    }

    // Áp dụng hiệu ứng địa hình
    battleField->applyTerrainEffects(this);
    battleField->applyTerrainEffects(enemy);
}


string ARVN::str() const {
    ostringstream oss;
    oss << "ARVN[name=" << name
        << ",LF=" << LF
        << ",EXP=" << EXP
        << ",unitList=" << unitList->str()
        << ",battleField=" << battleField->str()
        << "]";
    return oss.str();
}


// DANH SÁCH LIÊN KẾT ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UnitList::UnitList(int capacity) : capacity(capacity), head(nullptr) {
    // Khởi tạo danh sách rỗng với sức chứa capacity
}

UnitList::Node* UnitList::getHead() const {
    return head;
}

// Trả về sức chứa tối đa của danh sách đơn vị
int UnitList::getCapacity() const {
    return capacity;
}
UnitList::~UnitList() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
         
        delete current;        // giải phóng node
        current = next;
    }
}
bool UnitList::insert(Unit* unit) {
    Node* current = head;
    while (current != nullptr) {
        // Nếu đơn vị đã tồn tại → cập nhật số lượng
        if (unit->isVehicle() && current->unit->isVehicle()) {
            if (static_cast<Vehicle*>(unit)->getType() == static_cast<Vehicle*>(current->unit)->getType()) {
                current->unit->addQuantity(unit->getQuantity());
                return true;
            }
        }
        else if (!unit->isVehicle() && !current->unit->isVehicle()) {
            if (static_cast<Infantry*>(unit)->getType() == static_cast<Infantry*>(current->unit)->getType()) {
                current->unit->addQuantity(unit->getQuantity());
                return true;
            }
        }
        current = current->next;
    }

    // Nếu chưa tồn tại → thêm mới
    Node* newNode = new Node(unit);
    if (unit->isVehicle()) {
        // Thêm cuối danh sách
        if (!head) head = newNode;
        else {
            Node* tail = head;
            while (tail->next != nullptr) tail = tail->next;
            tail->next = newNode;
        }
    }
    else {
        // Thêm đầu danh sách
        newNode->next = head;
        head = newNode;
    }
    return true;
}

bool UnitList::isContain(VehicleType type) {
    Node* current = head;
    while (current) {
        if (current->unit->isVehicle()) {
            if (static_cast<Vehicle*>(current->unit)->getType() == type)
                return true;
        }
        current = current->next;
    }
    return false;
}
bool UnitList::isContain(InfantryType type) {
    Node* current = head;
    while (current) {
        if (!current->unit->isVehicle()) {
            if (static_cast<Infantry*>(current->unit)->getType() == type)
                return true;
        }
        current = current->next;
    }
    return false;
}
string UnitList::str() const {
    int count_vehicle = 0, count_infantry = 0;
    stringstream unitStream;

    Node* current = head;
    bool first = true;

    while (current) {
        if (!first) unitStream << ","; // Ngăn cách bởi dấu phẩy
        first = false;

        if (current->unit->isVehicle()) {
            count_vehicle++;
        }
        else {
            count_infantry++;
        }

        unitStream << current->unit->str(); // Gọi str() của từng Unit
        current = current->next;
    }

    stringstream ss;
    ss << "UnitList[count_vehicle=" << count_vehicle
        << ";count_infantry=" << count_infantry
        << ";" << unitStream.str() << "]";

    return ss.str();
}

bool UnitList::isSpecialNumber(int n, int k) {
    while (n > 0) {
        int digit = n % k;
        if (digit > 1) return false;
        n /= k;
    }
    return true;
}
bool UnitList::isSpecialForAnyOddPrimeBase(int s) {
    int bases[] = { 3, 5, 7 };
    for (int base : bases) {
        if (isSpecialNumber(s, base)) return true;
    }
    return false;
}
UnitList::UnitList(int LF, int EXP) {
    int S = LF + EXP;
    if (isSpecialForAnyOddPrimeBase(S)) {
        capacity = 12;
    }
    else {
        capacity = 8;
    }
    head = nullptr;
}

// VỊ TRÍ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Position::Position(int r, int c) : r(r), c(c) {}

Position::Position(const string& str_pos) : r(0), c(0) {
    size_t commaPos = str_pos.find(',');
    if (commaPos != string::npos) {
        r = stoi(str_pos.substr(1, commaPos - 1));
        c = stoi(str_pos.substr(commaPos + 1, str_pos.length() - commaPos - 2));
    }
}

int Position::getRow() const {
    return r;
}
int Position::getCol() const {
    return c;
}

void Position::setRow(int r) {
    this->r = r;
}
void Position::setCol(int c) {
    this->c = c;
}

string Position::str() const {
    ostringstream oss;
    oss << "(" << r << "," << c << ")";
    return oss.str();
}
// ĐỊA HÌNH ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TerrainElement::TerrainElement() {
    // constructor mặc định, có thể để trống
}

TerrainElement::~TerrainElement() {
    // destructor mặc định, có thể để trống
}

Road::Road() {}
Road::~Road() {}
void Road::getEffect(Army* army) {

}


Forest::Forest() {}
Forest::~Forest() {}
void Forest::getEffect(Army* army) {
    UnitList::Node* current = army->getUnitList()->getHead();

    // Kiểm tra loại army
    LiberationArmy* libArmy = dynamic_cast<LiberationArmy*>(army);
    ARVN* arvnArmy = dynamic_cast<ARVN*>(army);

    // Xác định bán kính ảnh hưởng
    int effectRadius = 0;
    if (libArmy) effectRadius = 2;
    else if (arvnArmy) effectRadius = 4;
    else return; // Không thuộc loại cần xử lý

    while (current != nullptr) {
        Unit* unit = current->unit;
        Position unitPos = unit->getCurrentPosition();

        double distance = sqrt(
            pow(unitPos.getRow() - this->pos.getRow(), 2) + pow(unitPos.getCol() - this->pos.getCol(), 2)
        );

        if (distance <= effectRadius) {
            int Scoregoc = unit->getAttackScore();

            if (!unit->isVehicle()) {
                // Bộ binh
                if (libArmy) {
                    int addedExp = static_cast<int>(Scoregoc * 0.3);
                    libArmy->addEXP(addedExp);
                }
                else if (arvnArmy) {
                    int addedExp = static_cast<int>(Scoregoc * 0.2);
                    arvnArmy->addEXP(addedExp);
                }
            }
            else {
                // Phương tiện
                if (libArmy) {
                    int subtractedLF = static_cast<int>(Scoregoc * 0.1);
                    libArmy->subLF(subtractedLF);
                }
                else if (arvnArmy) {
                    int subtractedLF = static_cast<int>(Scoregoc * 0.05);
                    arvnArmy->subLF(subtractedLF);
                }
            }
        }

        current = current->next;
    }
}


River::River() {}
River::~River() {}

void River::getEffect(Army* army) {
    UnitList::Node* current = army->getUnitList()->getHead();

    LiberationArmy* libArmy = dynamic_cast<LiberationArmy*>(army);
    ARVN* arvnArmy = dynamic_cast<ARVN*>(army);

    int effectRadius = 0;
    if (libArmy) effectRadius = 2;
    else if (arvnArmy) effectRadius = 2;
    else return;


    while (current != nullptr) {
        Unit* unit = current->unit;
        Position unitPos = unit->getCurrentPosition();

        double distance = sqrt(
            pow(unitPos.getRow() - this->pos.getRow(), 2) + pow(unitPos.getCol() - this->pos.getCol(), 2)
        );

        if (distance <= effectRadius) {
            int Scoregoc = unit->getAttackScore();

            if (!unit->isVehicle()) {

                if (libArmy) {


                    int subtractedExp = static_cast<int>(Scoregoc * 0.1);
                    libArmy->subEXP(subtractedExp);

                }
                else if (arvnArmy) {
                    int subtractedExp = static_cast<int>(Scoregoc * 0.1);
                    arvnArmy->subEXP(subtractedExp);



                }
            }

            current = current->next;
        }

    }
}



Urban::Urban() {}
Urban::~Urban() {}

void Urban::getEffect(Army* army) {
    UnitList::Node* current = army->getUnitList()->getHead();

    LiberationArmy* libArmy = dynamic_cast<LiberationArmy*>(army);
    ARVN* arvnArmy = dynamic_cast<ARVN*>(army);

    while (current != nullptr) {
        Unit* unit = current->unit;
        Position unitPos = unit->getCurrentPosition();

        double distance = sqrt(
            pow(unitPos.getRow() - this->pos.getRow(), 2) + pow(unitPos.getCol() - this->pos.getCol(), 2)
        );


        if (libArmy) {
            int infantryEffectRadius = 5;
            int vehicleEffectRadius = 2;

            if (distance <= infantryEffectRadius && !unit->isVehicle()) {
                Infantry* infantryUnit = dynamic_cast<Infantry*>(unit);
                if (infantryUnit) {
                    if (infantryUnit->getType() == InfantryType::SPECIALFORCES || infantryUnit->getType() == InfantryType::REGULARINFANTRY) {
                        int Scoregoc = unit->getAttackScore();
                        int D = static_cast<int>(distance);

                        int addedExp = static_cast<int>(2 * Scoregoc * D);
                        libArmy->addEXP(addedExp);
                    }
                }

            }
            else if (distance <= vehicleEffectRadius && unit->isVehicle()) {
                Vehicle* vehicleUnit = dynamic_cast<Vehicle*>(unit);
                if (vehicleUnit) {
                    if (vehicleUnit->getType() == VehicleType::ARTILLERY) {
                        int Scoregoc = unit->getAttackScore();
                        int subtractedLF = static_cast<int>(Scoregoc * 0.5);
                        libArmy->subLF(subtractedLF);
                    }
                }
            }

        }
        else if (arvnArmy) {
            int infantryEffectRadius = 3;
            if (distance <= infantryEffectRadius && !unit->isVehicle()) {
                Infantry* infantryUnit = dynamic_cast<Infantry*>(unit);
                if (infantryUnit) {
                    if (infantryUnit->getType() == InfantryType::REGULARINFANTRY) {
                        int Scoregoc = unit->getAttackScore();
                        int D = static_cast<int>(distance);

                        int addedExp = static_cast<int>((3 * Scoregoc) / (2 * D));
                        arvnArmy->addEXP(addedExp);
                    }
                }

            }
            else if (unit->isVehicle()) {

            }


        }
        else return;
    }
}


Fortification::Fortification() {}
Fortification::~Fortification() {}
void Fortification::getEffect(Army* army) {
    UnitList::Node* current = army->getUnitList()->getHead();

    LiberationArmy* libArmy = dynamic_cast<LiberationArmy*>(army);
    ARVN* arvnArmy = dynamic_cast<ARVN*>(army);

    int effectRadius = 0;
    if (libArmy) effectRadius = 2;
    else if (arvnArmy) effectRadius = 2;
    else return;


    while (current != nullptr) {
        Unit* unit = current->unit;
        Position unitPos = unit->getCurrentPosition();

        double distance = sqrt(
            pow(unitPos.getRow() - this->pos.getRow(), 2) + pow(unitPos.getCol() - this->pos.getCol(), 2)
        );

        if (distance <= effectRadius) {
            int Scoregoc = unit->getAttackScore();

            if (!unit->isVehicle()) {

                if (libArmy) {
                    int subtractedExp = static_cast<int>(Scoregoc * 0.2);
                    libArmy->subEXP(subtractedExp);
                }
                else if (arvnArmy) {
                    int addedExp = static_cast<int>(Scoregoc * 0.2);
                    arvnArmy->addEXP(addedExp);
                }
            }
            else {

                if (libArmy) {
                    int subtractedLF = static_cast<int>(Scoregoc * 0.2);
                    libArmy->subLF(subtractedLF);
                }
                else if (arvnArmy) {
                    int addedLF = static_cast<int>(Scoregoc * 0.2);
                    arvnArmy->addLF(addedLF);
                }
            }
        }

    }
}


SpecialZone::SpecialZone() {}
SpecialZone::~SpecialZone() {}

void SpecialZone::getEffect(Army* army) {
    UnitList::Node* current = army->getUnitList()->getHead();

    LiberationArmy* libArmy = dynamic_cast<LiberationArmy*>(army);
    ARVN* arvnArmy = dynamic_cast<ARVN*>(army);

    int effectRadius = 0;
    if (libArmy) effectRadius = 2;
    else if (arvnArmy) effectRadius = 2;
    else return;


    while (current != nullptr) {
        Unit* unit = current->unit;
        Position unitPos = unit->getCurrentPosition();

        double distance = sqrt(
            pow(unitPos.getRow() - this->pos.getRow(), 2) + pow(unitPos.getCol() - this->pos.getCol(), 2)
        );

        if (distance <= effectRadius) {
            int Scoregoc = unit->getAttackScore();

            if (!unit->isVehicle()) {

                if (libArmy) {
                    int subtractedExp = static_cast<int>(Scoregoc * 1);
                    libArmy->subEXP(subtractedExp);
                }
                else if (arvnArmy) {
                    int subtractedExp = static_cast<int>(Scoregoc * 1);
                    arvnArmy->subEXP(subtractedExp);
                }
            }
            else {

                if (libArmy) {
                    int subtractedLF = static_cast<int>(Scoregoc * 1);
                    libArmy->subLF(subtractedLF);
                }
                else if (arvnArmy) {
                    int subtractedLF = static_cast<int>(Scoregoc * 1);
                    arvnArmy->subLF(subtractedLF);
                }
            }
        }

    }
}










// BATTLEFIELD ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


BattleField::BattleField(int n_rows, int n_cols, vector<Position*> arrayForest,
    vector<Position*> arrayRiver, vector<Position*> arrayFortification,
    vector<Position*> arrayUrban, vector<Position*> arraySpecialZone)
    : n_rows(n_rows), n_cols(n_cols) {
    // Cấp phát động mảng 2 chiều
    terrain = new TerrainElement * *[n_rows];
    for (int i = 0; i < n_rows; ++i) {
        terrain[i] = new TerrainElement * [n_cols];
        for (int j = 0; j < n_cols; ++j) {
            terrain[i][j] = new Road(); // Mặc định là đường mòn
        }
    }

    // Đánh dấu các vị trí địa hình
    for (Position* pos : arrayForest) {
        terrain[pos->getRow()][pos->getCol()] = new Forest();
    }
    for (Position* pos : arrayRiver) {
        terrain[pos->getRow()][pos->getCol()] = new River();
    }
    for (Position* pos : arrayFortification) {
        terrain[pos->getRow()][pos->getCol()] = new Fortification();
    }
    for (Position* pos : arrayUrban) {
        terrain[pos->getRow()][pos->getCol()] = new Urban();
    }
    for (Position* pos : arraySpecialZone) {
        terrain[pos->getRow()][pos->getCol()] = new SpecialZone();
    }
}

BattleField::~BattleField() {
    for (int i = 0; i < n_rows; ++i) {
        delete[] terrain[i];
    }
    delete[] terrain;
}
void BattleField::applyTerrainEffects(Army* army) {
    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols; j++) {
            if (terrain[i][j]) {
                terrain[i][j]->getEffect(army);  // Áp dụng tác động từ địa hình lên quân đội
            }
        }
    }
}

// Phương thức trả về chuỗi thông tin của trận địa
string BattleField::str() const {
    ostringstream oss;
    oss << "BattleField[n_rows=" << n_rows
        << ",n_cols=" << n_cols << "]";
    return oss.str();
}


// CONFIGURATION ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Configuration::Configuration(const string& filepath)
    : num_rows(0),
    num_cols(0),
    eventCode(0),
    numARVNUnits(0),
    numLiberationUnits(0)
{

    liberationUnits = nullptr;
    ARVNUnits = nullptr;
    liberationArmy = nullptr;
    arvn = nullptr;
    battleField = nullptr;

    ifstream fin(filepath);
    if (!fin.is_open()) 
        cout << "Cannot open file: " << filepath << endl;
        return;
    

    string line;
    vector<Unit*> libVec, arvnVec;

    while (getline(fin, line)) {
        if (line.find("NUM_ROWS=") == 0) {
            num_rows = stoi(line.substr(9));
        }
        else if (line.find("NUM_COLS=") == 0) {
            num_cols = stoi(line.substr(9));
        }
        else if (line.find("ARRAY_FOREST=") == 0) {
            string s = line.substr(13);
            s = s.substr(1, s.length() - 2);  // Bỏ dấu ngoặc
            stringstream ss(s);
            string token;
            while (getline(ss, token, ')')) {
                if (token.empty()) continue;
                token += ")";
                if (token[0] == ',') token = token.substr(1);

                int r, c;
                stringstream tokenStream(token);
                char discard;
                tokenStream >> discard >> r >> discard >> c >> discard;  // Xử lý chuỗi "(r,c)"
                arrayForest.push_back(new Position(r, c));
            }
        }
        else if (line.find("ARRAY_RIVER=") == 0) {
            string s = line.substr(12);
            s = s.substr(1, s.length() - 2);  // Bỏ dấu ngoặc
            stringstream ss(s);
            string token;
            while (getline(ss, token, ')')) {
                if (token.empty()) continue;
                token += ")";
                if (token[0] == ',') token = token.substr(1);

                int r, c;
                stringstream tokenStream(token);
                char discard;
                tokenStream >> discard >> r >> discard >> c >> discard;  // Xử lý chuỗi "(r,c)"
                arrayRiver.push_back(new Position(r, c));
            }
        }
        else if (line.find("ARRAY_FORTIFICATION=") == 0) {
            string s = line.substr(21);
            s = s.substr(1, s.length() - 2);  // Bỏ dấu ngoặc
            stringstream ss(s);
            string token;
            while (getline(ss, token, ')')) {
                if (token.empty()) continue;
                token += ")";
                if (token[0] == ',') token = token.substr(1);

                int r, c;
                stringstream tokenStream(token);
                char discard;
                tokenStream >> discard >> r >> discard >> c >> discard;  // Xử lý chuỗi "(r,c)"
                arrayFortification.push_back(new Position(r, c));
            }
        }
        else if (line.find("ARRAY_URBAN=") == 0) {
            string s = line.substr(12);
            s = s.substr(1, s.length() - 2);  // Bỏ dấu ngoặc
            stringstream ss(s);
            string token;
            while (getline(ss, token, ')')) {
                if (token.empty()) continue;
                token += ")";
                if (token[0] == ',') token = token.substr(1);

                int r, c;
                stringstream tokenStream(token);
                char discard;
                tokenStream >> discard >> r >> discard >> c >> discard;  // Xử lý chuỗi "(r,c)"
                arrayUrban.push_back(new Position(r, c));
            }
        }
        else if (line.find("ARRAY_SPECIAL_ZONE=") == 0) {
            string s = line.substr(19);
            s = s.substr(1, s.length() - 2);  // Bỏ dấu ngoặc
            stringstream ss(s);
            string token;
            while (getline(ss, token, ')')) {
                if (token.empty()) continue;
                token += ")";
                if (token[0] == ',') token = token.substr(1);

                int r, c;
                stringstream tokenStream(token);
                char discard;
                tokenStream >> discard >> r >> discard >> c >> discard;  // Xử lý chuỗi "(r,c)"
                arraySpecialZone.push_back(new Position(r, c));
            }
        }
        else if (line.find("UNIT_LIST=") == 0) {
            string s = line.substr(10);
            int level = 0;
            string temp;
            vector<string> unitStrs;

            for (char ch : s) {
                if (ch == ',' && level == 0) {
                    unitStrs.push_back(temp);
                    temp.clear();
                }
                else {
                    if (ch == '(') level++;
                    if (ch == ')') level--;
                    temp += ch;
                }
            }
            if (!temp.empty()) unitStrs.push_back(temp);

            for (string unitStr : unitStrs) {
                size_t pos = unitStr.find('(');
                string type = unitStr.substr(0, pos);
                string args = unitStr.substr(pos + 1, unitStr.length() - pos - 2);
                int quantity, weight, row, col, belong;

                // Sử dụng stringstream để phân tích args
                stringstream argsStream(args);
                char discard;
                argsStream >> quantity >> discard >> weight >> discard >> row >> discard >> col >> discard >> belong;

                Position p(row, col);
                Unit* u = nullptr;

                if (type == "TANK") {
                    u = new Vehicle(quantity, weight, p, TANK);
                }
                else if (type == "TRUCK") {
                    u = new Vehicle(quantity, weight, p, TRUCK);
                }
                else if (type == "ARTILLERY") {
                    u = new Vehicle(quantity, weight, p, ARTILLERY);
                }
                else if (type == "MORTAR") {
                    u = new Vehicle(quantity, weight, p, MORTAR);
                }
                else if (type == "ANTIAIRCRAFT") {
                    u = new Vehicle(quantity, weight, p, ANTIAIRCRAFT);
                }
                else if (type == "ARMOREDCAR") {
                    u = new Vehicle(quantity, weight, p, ARMOREDCAR);
                }
                else if (type == "APC") {
                    u = new Vehicle(quantity, weight, p, APC);
                }

                else if (type == "SPECIALFORCES") {
                    u = new Infantry(quantity, weight, p, SPECIALFORCES);
                }
                else if (type == "REGULARINFANTRY") {
                    u = new Infantry(quantity, weight, p, REGULARINFANTRY);
                }
                else if (type == "SNIPER") {
                    u = new Infantry(quantity, weight, p, SNIPER);
                }
                else if (type == "ANTIAIRCRAFTSQUAD") {
                    u = new Infantry(quantity, weight, p, ANTIAIRCRAFTSQUAD);
                }
                else if (type == "MORTARSQUAD") {
                    u = new Infantry(quantity, weight, p, MORTARSQUAD);
                }
                else if (type == "ENGINEER") {
                    u = new Infantry(quantity, weight, p, ENGINEER);
                }


                if (belong == 0) libVec.push_back(u);
                else arvnVec.push_back(u);
            }
        }
        else if (line.find("EVENT_CODE=") == 0) {
            int code = stoi(line.substr(11));
            if (code < 0) eventCode = 0;
            else eventCode = code % 100;
        }
    }


    numLiberationUnits = libVec.size();
    numARVNUnits = arvnVec.size();

    liberationUnits = new Unit * [numLiberationUnits];
    for (int i = 0; i < numLiberationUnits; ++i)
        liberationUnits[i] = libVec[i];

    ARVNUnits = new Unit * [numARVNUnits];
    for (int i = 0; i < numARVNUnits; ++i)
        ARVNUnits[i] = arvnVec[i];


    battleField = new BattleField(num_rows, num_cols, arrayForest, arrayRiver,
        arrayFortification, arrayUrban, arraySpecialZone);
    liberationArmy = new LiberationArmy(liberationUnits, numLiberationUnits, "GiaiPhong", battleField);
    arvn = new ARVN(ARVNUnits, numARVNUnits, "ARVN", battleField);
}


Configuration::~Configuration() {
    for (auto p : arrayForest) delete p;
    for (auto p : arrayRiver) delete p;
    for (auto p : arrayFortification) delete p;
    for (auto p : arrayUrban) delete p;
    for (auto p : arraySpecialZone) delete p;

    for (int i = 0; i < numLiberationUnits; ++i) delete liberationUnits[i];
    for (int i = 0; i < numARVNUnits; ++i) delete ARVNUnits[i];

    delete[] liberationUnits;
    delete[] ARVNUnits;
    delete liberationArmy;
    delete arvn;
    delete battleField;
}
string Configuration::str() const {
    stringstream ss;

    auto formatArray = [](const vector<Position*>& arr) -> string {
        stringstream s;
        s << "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            s << "(" << arr[i]->getRow() << "," << arr[i]->getCol() << ")";
            if (i != arr.size() - 1) s << ",";
        }
        s << "]";
        return s.str();
        };

    ss << "[num_rows=" << num_rows
        << ",num_cols=" << num_cols
        << ",arrayForest=" << formatArray(arrayForest)
        << ",arrayRiver=" << formatArray(arrayRiver)
        << ",arrayFortification=" << formatArray(arrayFortification)
        << ",arrayUrban=" << formatArray(arrayUrban)
        << ",arraySpecialZone=" << formatArray(arraySpecialZone);

    ss << ",liberationUnits=[";
    for (int i = 0; i < numLiberationUnits; ++i) {
        ss << liberationUnits[i]->str();
        if (i != numLiberationUnits - 1) ss << ",";
    }
    ss << "]";

    ss << ",ARVNUnits=[";
    for (int i = 0; i < numARVNUnits; ++i) {
        ss << ARVNUnits[i]->str();
        if (i != numARVNUnits - 1) ss << ",";
    }
    ss << "]";

    ss << ",eventCode=" << eventCode << "]";

    return ss.str();
}
// HCM CAMPAIGN ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HCMCampaign::HCMCampaign(const string& config_file_path) {
    // Khởi tạo config từ file
    config = new Configuration(config_file_path);

    // Lấy các đối tượng từ config
    battleField = config->getBattleField();
    liberationArmy = config->getLiberationArmy();
    arvn = config->getARVN();
}

void HCMCampaign::run() {
    // Áp dụng địa hình lên từng quân đội trước khi chiến đấu
    battleField->applyTerrainEffects(liberationArmy);
    battleField->applyTerrainEffects(arvn);

    // Lấy mã sự kiện
    int eventCode = config->getEventCode();

    // Nếu mã sự kiện < 75 => LiberationArmy tấn công trước
    if (eventCode < 75) {
        liberationArmy->fight(arvn, false);  // không ở thế phòng thủ
    }
    else {
        arvn->fight(liberationArmy, false);  // ARVN chủ động tấn công
        liberationArmy->fight(arvn, false);  // LiberationArmy phản công
    }

    // Sau giao tranh, xoá đơn vị có attackScore <= 5
    auto laList = liberationArmy->getUnitList();
    auto arvnList = arvn->getUnitList();

    // Duyệt danh sách LiberationArmy
    auto node = laList->getHead();
    while (node != nullptr) {
        Unit* u = node->unit;
        node = node->next;  // lưu trước vì node có thể bị xóa
        if (u->getAttackScore() <= 5) {
            laList->remove(u);
        }
    }

    // Duyệt danh sách ARVN
    node = arvnList->getHead();
    while (node != nullptr) {
        Unit* u = node->unit;
        node = node->next;
        if (u->getAttackScore() <= 5) {
            arvnList->remove(u);
        }
    }
}

string HCMCampaign::printResult() {
    // Gọi phương thức str() của mỗi quân đội
    return liberationArmy->str() + "-" + arvn->str();
}


////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////