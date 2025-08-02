#include <iostream>
#include <map>
#include <vector>

class Map {
    private:
    std::map<int, std::string> counter;
    public:
    void setCounter(int i, std::string name) {
        counter[i] = name;
    }
    std::map<int, std::string> getCounter() const {
        return counter;
    }

};

int main() {
    Map count;
    std::vector<std::string> names = {
        "Hassan",
        "Badr",
        "Moussa",
        "Abde Mo",
        "Oussam",
        "Ayoube",
    };
    for (size_t i = 0; i < names.size(); i++) {
        count.setCounter(i, names[i]);
    }
    std::map<int, std::string> c = count.getCounter();
    std::map<int, std::string>::iterator it;

    for (it = c.begin(); it != c.end(); it++) {
        std::cout << "cout[" << it->first + 1 << "] -> " << it->second << std::endl; 
    }
}