#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "Sqlon2.h"

using namespace std;
using std::cout;

int main() {
    cout << "13" << endl;
    Sqlon2 s({
        {"a", Sqlon2(3)},
        {"b", Sqlon2({
            Sqlon2(1), Sqlon2(2), Sqlon2(11.1)
        })}
    });
    cout << "123" << endl;
    for (int i = 0; i < 256; i++) {
        s.getObject()["b"].getList().push_back(Sqlon2(to_string(i)));
    }
    cout << "124" << endl;
    ofstream ofs("b.txt", ios::binary);
    ofs << s.serialize() << flush;
    ofstream dofs("Db.txt");
    dofs << s.describe() << flush;
    string str = s.serialize(), desc = s.describe();
    Sqlon2 on = s.deserialize(str, desc);
}