#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cassert>

// like json
enum class Label2 {
    Null, Bool, Int, Double, String, List, Object, Array
};

class Sqlon2 {
private:
    // store raw data
    void* data;
    Label2 type;
    // varLen: the length(number) of the type is stored into the data file, not describtion file
    bool varLen = false;
public:
    Sqlon2() : data(nullptr), type(Label2::Null) {}
    Sqlon2(const bool& n) : data(new bool(n)), type(Label2::Bool) {}
    Sqlon2(const int& n) : data(new int64_t(n)), type(Label2::Int) {}
    Sqlon2(const int64_t& n) : data(new int64_t(n)), type(Label2::Int) {}
    Sqlon2(const std::size_t& n) : data(new int64_t(n)), type(Label2::Int) {}
    Sqlon2(const double& n) : data(new double(n)), type(Label2::Double) {}
    Sqlon2(const std::string& n, bool varLen = true) : data(new std::string(n)), varLen(varLen), type(Label2::String) {}
    Sqlon2(const std::vector<Sqlon2>& n, bool isArray = false) {
        if (isArray) {
            data = new std::vector<Sqlon2>(n);
            type = Label2::Array;
        } else {
            data = new std::vector<Sqlon2>(n);
            type = Label2::List;
        }
    }
    Sqlon2(const std::map<std::string, Sqlon2>& n) 
        : data(new std::map<std::string, Sqlon2>(n)), type(Label2::Object) {}
    Sqlon2(const char* n, bool varLen = true) : data(new std::string(n)), varLen(varLen), type(Label2::String) {}
    Sqlon2(const Sqlon2& rhs) : varLen(rhs.varLen), type(rhs.type) {
        data = rhs.clone_data();
        type = rhs.type;
    }
    //Sqlon2(Sqlon2&& rhs) : data(rhs.data), varLen(rhs.varLen), varVal(rhs.varVal) {}

    Sqlon2& operator=(const Sqlon2& rhs) {
        if (this == &rhs) return *this;
        
        destroy();
        this->data = rhs.clone_data();

        type = rhs.type;
        varLen = rhs.varLen;

        return *this;
    }

 
    bool operator==(const Sqlon2& others) const;
    bool operator<(const Sqlon2& others) const;
    bool operator>(const Sqlon2& others) const;
    bool operator<=(const Sqlon2& others) const;
    bool operator>=(const Sqlon2& others) const;
    bool operator!=(const Sqlon2& others) const;

    std::string getTypeName() const;

    bool isNull() const {
        return type == Label2::Null;
    }
    
    template<typename T>
    T& get() {
        return *reinterpret_cast<T*>(data);
    }

    template<typename T>
    const T& get() const {
        return *reinterpret_cast<const T*>(data);
    }

    bool& getBool() {
        assert(type == Label2::Bool);
        return *reinterpret_cast<bool*>(data);
    }

    const bool& getBool() const {
        assert(type == Label2::Bool);
        return *reinterpret_cast<const bool*>(data);
    }

    int64_t& getInt() {
        assert(type == Label2::Int);
        return *reinterpret_cast<int64_t*>(data);
    }

    const int64_t& getInt() const {
        assert(type == Label2::Int);
        return *reinterpret_cast<const int64_t*>(data);
    }

    double& getDouble() {
        assert(type == Label2::Double);
        return *reinterpret_cast<double*>(data);
    }

    const double& getDouble() const {
        assert(type == Label2::Double);
        return *reinterpret_cast<const double*>(data);
    }

    std::string& getString() {
        assert(type == Label2::String);
        return *reinterpret_cast<std::string*>(data);
    }

    const std::string& getString() const {
        assert(type == Label2::String);
        return *reinterpret_cast<const std::string*>(data);
    }

    std::vector<Sqlon2>& getList() {
        assert(type == Label2::List || type == Label2::Array);
        return *reinterpret_cast<std::vector<Sqlon2>*>(data);
    }

    const std::vector<Sqlon2>& getList() const {
        assert(type == Label2::List || type == Label2::Array);
        return *reinterpret_cast<const std::vector<Sqlon2>*>(data);
    }

    std::map<std::string, Sqlon2>& getObject() {
        assert(type == Label2::Object);
        return *reinterpret_cast<std::map<std::string, Sqlon2>*>(data);
    }

    const std::map<std::string, Sqlon2>& getObject() const {
        assert(type == Label2::Object);
        return *reinterpret_cast<const std::map<std::string, Sqlon2>*>(data);
    }

    std::string to_string() const;

    std::string serialize() const;
    std::string describe() const;
    static Sqlon2 deserialize(const std::string& str, const std::string& desc);
    static std::size_t size_by_description(const std::string& desc);

    ~Sqlon2();

private:
    void* clone_data() const;
    void destroy();
};
