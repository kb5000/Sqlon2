#include <iostream>
#include "Sqlon2.h"
#include <sstream>
#include <fstream>
#include <cstring>

using namespace std;

std::string Sqlon2::getTypeName() const {
    switch (type)
    {
    case Label2::Null:
        return "N";
    case Label2::Bool:
        return "B";
    case Label2::Int:
        return "I";
    case Label2::Double:
        return "D";
    case Label2::String:
        return "S";
    case Label2::List:
        return "L";
    case Label2::Object:
        return "O";
    case Label2::Array:
        return "A";
    default:
        return "";
    }
}

string Sqlon2::serialize() const {
    // string res;
    ostringstream oss;
    switch (type) {
    case Label2::Null: break;
    case Label2::Bool: {
        bool b = getBool();
        oss << (b ? 'T' : 'F');
        break;
    }
    case Label2::Int: {
        int64_t num = getInt();
        for (int i = 0; i < 8; i++) oss << ((char*)&num)[i];
        break;
    }
    case Label2::Double: {
        double fnum = getDouble();
        for (int i = 0; i < 8; i++) oss << ((char*)&fnum)[i];
        break;
    }
    case Label2::String: {
        string str = getString();
        if (varLen) {
            int64_t n(str.size());
            for (int i = 0; i < 8; i++) oss << ((char*)&n)[i];
        }
        oss << str;
        break;
    }
    case Label2::List: {
        auto vec = getList();
        for (const auto& i : vec) {
            oss << i.serialize();
        }
        break;
    }
    case Label2::Array: {
        auto vec = getList();
        int64_t n(vec.size());
        for (int i = 0; i < 8; i++) oss << ((char*)&n)[i];
        for (const auto& i : vec) {
            oss << i.serialize();
        }
        break;
    }
    case Label2::Object: {
        auto obj = getObject();
        for (const auto& i : obj) {
            int64_t n(i.first.size());
            for (int j = 0; j < 8; j++) oss << ((char*)&n)[j];

            oss << i.first;
            // res += Sqlon2(i.first).serialize();
            oss << i.second.serialize();
        }
        break;
    }
    default:
        break;
    }
    return oss.str();
}

string Sqlon2::describe() const {
    ostringstream oss;
    switch (type) {
    case Label2::Null:
        oss << 'N';
        break;
    case Label2::Bool:
        oss << 'B';
        break;
    case Label2::Int:
        oss << 'I';
        break;
    case Label2::Double:
        oss << 'D';
        break;
    case Label2::String:
        oss << 'S';
        if (!varLen) oss << getString().size();
        break;
    case Label2::List: {
        oss << 'L';
        auto vec = getList();
        oss << vec.size();
        for (auto i : vec) oss << i.describe();
        break;
    }
    case Label2::Array: {
        oss << 'A';
        auto vec = getList();
        if (vec.size() > 0)
            oss << vec[0].describe();
        else
            oss << 'N';
        break;
    }
    case Label2::Object: {
        oss << 'O';
        auto obj = getObject();
        oss << obj.size();
        for (auto i : obj) oss << i.second.describe();
        break;
    }
    default:
        break;
    }
    return oss.str();
}

// just to read one item and skip
static void fake_deserialize_item(const string& str, istringstream& iss, uint32_t& pos) {
    char c = iss.get();
    switch (c) {
    case 'N': break;
    case 'B': break;
    case 'I': {
        break;
    }
    case 'D': {
        break;
    }
    case 'S': {
        int64_t size = 0;
        int peek = iss.peek();
        if (peek < '0' || peek > '9') {

        } else {
            iss >> size;
        }
        break;
    }
    case 'L': {
        int64_t size = 0;

        iss >> size;

        for (int i = 0; i < size; i++) {
            fake_deserialize_item(str, iss, pos);
        }
        break;
    }
    case 'A': {
        fake_deserialize_item(str, iss, pos);
        break;
    }
    case 'O': {
        int64_t size = 0;

        iss >> size;

        for (int i = 0; i < size; i++) {
            fake_deserialize_item(str, iss, pos);
        }
        break;
    }
    default:
        break;
    }
}

static Sqlon2 deserialize_item(const string& str, istringstream& iss, uint32_t& pos) {
    Sqlon2 res;
    char c = iss.get();
    switch (c) {
    case 'N': res = Sqlon2(); break;
    case 'B': res = Sqlon2(str[pos] == 'T'); pos += 1; break;
    case 'I': {
        int64_t num = 0;
        for (int i = 0; i < 8; i++) {
            ((char*)&num)[i] = str[pos + i];
        }
        pos += 8;
        res = Sqlon2(num);
        break;
    }
    case 'D': {
        double fnum = 0;
        for (int i = 0; i < 8; i++) {
            ((char*)&fnum)[i] = str[pos + i];
        }
        pos += 8;
        res = Sqlon2(fnum);
        break;
    }
    case 'S': {
        bool varLen = false;
        string restr;
        int64_t size = 0;
        int peek = iss.peek();
        if (peek < '0' || peek > '9') {
            for (int i = 0; i < 8; i++) {
                ((char*)&size)[i] = str[pos + i];
            }
            pos += 8;
            varLen = true;
        } else {
            iss >> size;
        }
        restr = str.substr(pos, size);
        pos += size;
        res = Sqlon2(restr, varLen);
        break;
    }
    case 'L': {
        int64_t size = 0;
        int peek = iss.peek();

        iss >> size;

        vector<Sqlon2> vec(size);
        for (int i = 0; i < size; i++) {
            vec[i] = (deserialize_item(str, iss, pos));
        }
        res = Sqlon2(vec, false);
        break;
    }
    case 'A': {
        int64_t size = 0;

        for (int i = 0; i < 8; i++) {
            ((char*)&size)[i] = str[pos + i];
        }
        pos += 8;

        vector<Sqlon2> vec(size);

        auto preserved = iss.tellg();

        for (int i = 0; i < size; i++) {
            vec[i] = (deserialize_item(str, iss, pos));
            if (i != size - 1) iss.seekg(preserved);
        }

        if (preserved == iss.tellg()) fake_deserialize_item(str, iss, pos);

        res = Sqlon2(vec, true);
        break;
    }
    case 'O': {
        map<string, Sqlon2> obj;
        int64_t size = 0;

        iss >> size;

        for (int i = 0; i < size; i++) {
            int64_t strSize = 0;

            for (int i = 0; i < 8; i++) {
                ((char*)&strSize)[i] = str[pos + i];
            }
            pos += 8;

            string key = str.substr(pos, strSize);
            pos += strSize;
            Sqlon2 content = deserialize_item(str, iss, pos);
            obj.insert({key, content});
        }
        res = Sqlon2(obj);
        break;
    }
    default:
        break;
    }
    return res;
}

Sqlon2 Sqlon2::deserialize(const string& str, const string& desc) {
    istringstream iss(desc);
    uint32_t pos = 0;
    return deserialize_item(str, iss, pos);
}

string Sqlon2::to_string() const {
    switch (this->type)
    {
    case Label2::Null:
        return "null";
    case Label2::Bool:
        return getBool() ? "true" : "false";
    case Label2::Int:
        return std::to_string(getInt());
    case Label2::Double:
        return std::to_string(getDouble());
    case Label2::String:
        return "\"" + string(getString().c_str()) + "\"";
    case Label2::List: case Label2::Array: {
        string res = "[";
        bool first = true;
        for (const auto& i : getList()) {
            if (!first) {
                res += ", ";
            }
            first = false;
            res += i.to_string();
        }
        res += "]";
        return res;
    }
    case Label2::Object: {
        string res = "{";
        bool first = true;
        for (const auto& i : getObject()) {
            if (!first) {
                res += ", ";
            }
            first = false;
            res += "\"" + i.first + "\": " + i.second.to_string();
        }
        res += "}";
        return res;
    }
    default:
        break;
    }
    return "";
}

static size_t size_by_desc_helper(istringstream& iss) {
    int c = iss.get();
    switch (c)
    {
    case 'N':
        return 0;
    case 'B':
        return 1;
    case 'I':
        return 4;
    case 'D':
        return 8;
    case 'S': {
        int n;
        iss >> n;
        return n + 4;
    }
    case 'L': {
        int n, r = 0;
        iss >> n;
        for (int i = 0; i < n; i++) {
            r += size_by_desc_helper(iss);
        }
        return r;
    }
    case 'O': {
        int n, l, r = 0;
        char c;
        iss >> n >> c >> l;
        for (int i = 0; i < l; i++) {

        }
    }
    default:
        break;
    }
    return 0;
}

size_t Sqlon2::size_by_description(const string& desc) {
    istringstream iss(desc);
    return size_by_desc_helper(iss);
}

bool Sqlon2::operator==(const Sqlon2& others) const {
    if (type != others.type) return false;
    switch (type) {
    case Label2::Null:
        return true;
    case Label2::Bool:
        return getBool() == others.getBool();
    case Label2::Int:
        return getInt() == others.getInt();
    case Label2::Double:
        return getDouble() == others.getDouble();
    case Label2::String:
        return getString() == others.getString();
    case Label2::List: case Label2::Array:
        if (getList().size() != others.getList().size()) {
            return false;
        } else {
            for (auto i = 0; i < getList().size(); i++) {
                if (!(getList()[i] == others.getList()[i])) return false;
            }
            return true;
        }
    default:
        return false;
    }
}

bool Sqlon2::operator!=(const Sqlon2& others) const {
    if (type != others.type) return false;
    switch (type) {
    case Label2::Null:
        return false;
    case Label2::Bool:
        return getBool() != others.getBool();
    case Label2::Int:
        return getInt() != others.getInt();
    case Label2::Double:
        return getDouble() != others.getDouble();
    case Label2::String:
        return getString() != others.getString();
    default:
        return false;
    }
}

bool Sqlon2::operator<(const Sqlon2& others) const {
    if (type != others.type) return false;
    switch (type) {
    case Label2::Null:
        return false;
    case Label2::Bool:
        return getBool() < others.getBool();
    case Label2::Int:
        return getInt() < others.getInt();
    case Label2::Double:
        return getDouble() < others.getDouble();
    case Label2::String:
        return getString() < others.getString();
    default:
        return false;
    }
}

bool Sqlon2::operator>(const Sqlon2& others) const {
    if (type != others.type) return false;
    switch (type) {
    case Label2::Null:
        return false;
    case Label2::Bool:
        return getBool() > others.getBool();
    case Label2::Int:
        return getInt() > others.getInt();
    case Label2::Double:
        return getDouble() > others.getDouble();
    case Label2::String:
        return getString() > others.getString();
    default:
        return false;
    }
}

bool Sqlon2::operator>=(const Sqlon2& others) const {
    if (type != others.type) return false;
    switch (type) {
    case Label2::Null:
        return true;
    case Label2::Bool:
        return getBool() >= others.getBool();
    case Label2::Int:
        return getInt() >= others.getInt();
    case Label2::Double:
        return getDouble() >= others.getDouble();
    case Label2::String:
        return getString() >= others.getString();
    default:
        return false;
    }
}

bool Sqlon2::operator<=(const Sqlon2& others) const {
    if (type != others.type) return false;
    switch (type) {
    case Label2::Null:
        return true;
    case Label2::Bool:
        return getBool() <= others.getBool();
    case Label2::Int:
        return getInt() <= others.getInt();
    case Label2::Double:
        return getDouble() <= others.getDouble();
    case Label2::String:
        return getString() <= others.getString();
    default:
        return false;
    }
}

void* Sqlon2::clone_data() const {
    switch (type) {
        case Label2::Null:
            return nullptr;
        case Label2::Int:
            return new int64_t(*(int64_t*)data);
        case Label2::Bool:
            return new bool(*(bool*)data);
        case Label2::Double:
            return new double(*(double*)data);
        case Label2::List: case Label2::Array: 
            return new std::vector<Sqlon2>(*(std::vector<Sqlon2>*)(data));
        case Label2::String:
            return new std::string(*(std::string*)data);
        case Label2::Object: 
            return new std::map<std::string, Sqlon2>(*(std::map<std::string, Sqlon2>*)(data));
    }
    return nullptr;
}

void Sqlon2::destroy() {
    switch (type) {
        case Label2::Null:
            break;
        case Label2::Int:
            delete (int64_t*)data;
            break;
        case Label2::Bool:
            delete (bool*)data;
            break;
        case Label2::Double:
            delete (double*)data;
            break;
        case Label2::List:
            delete (std::vector<Sqlon2>*)data;
            break;
        case Label2::String:
            delete (std::string*)data;
            break;
        case Label2::Object:
            delete (std::map<std::string, Sqlon2>*)data;
            break;
        case Label2::Array:
            delete (std::vector<Sqlon2>*)data;
            break;
    }
    type = Label2::Null;
}

Sqlon2::~Sqlon2() {
    destroy();
}