#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std;

class YamlParser {
public:
    YamlParser(const string &filename) { //konstruktorn laddar YAML-filen
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Kunde inte öppna filen: " << filename << endl;
            return;
        }

        string line;
        string currentKey;
        while (getline(file, line)) {
            line = trim(line); //trimma bort eventualla mellanslag innan och efter

            if (line.empty() || line[0] == '#') continue; //hoppa över tomma rader och kommentarer med #

            size_t colonPos = line.find(':'); //hantera nyckel-värde-par
            if (colonPos != string::npos) {
                string key = trim(line.substr(0, colonPos));
                string value = trim(line.substr(colonPos + 1));

                if(value.empty()) { //om värdet är tomt betyder det att nästa rader är en lista
                    currentKey = key;
                    data[key] = vector<string>{};

                } else {
                    data[key] = value; //annars lagra nyckel värdet som stäng
                }
            } else if (!currentKey.empty() && line[0] == '-') {
                string listItem = trim(line.substr(1));
                if (auto it = data.find(currentKey); it != data.end()) {
                    get<vector<string>>(it->second).push_back(listItem);
                }
            }
        }

        file.close();
    }

    string getString(const string& key) const { //metod för att extrahera en sträng från yaml filen
        if (auto it = data.find(key); it != data.end() && holds_alternative<string>(it->second)) {
            return get<string>(it->second);
        }
        throw runtime_error("Nyckel ej funnen eller ej en sträng: " + key);
    }

    int getInt(const string& key) const { //metod för att extrahera ett heltal från yaml filen
        if (auto it = data.find(key); it != data.end() && holds_alternative<string>(it->second)) {
            return stoi(get<string>(it->second));
        }
        throw runtime_error("Nyckel ej funnen eller ej ett heltal: " + key);
    }

    vector<string> getStringList(const string& key) const { //metod för att extrahera en lista från yaml filen
        if (auto it = data.find(key); it != data.end() && holds_alternative<vector<string>>(it->second)) {
            return get<vector<string>>(it->second);
        }
        throw runtime_error("Nyckel ej funnen eller ej en lista: " + key);

    }

private:
    static string trim(const string& str) { //hjälpfunktion för att trimma bort mellanslag början och slutet
        size_t first = str.find_first_not_of(' ');
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    unordered_map<string, variant<string, vector<string>>> data;
};

int main() {
    try {
        YamlParser parser("blaha.yaml"); // skpar ett parser objekt och läs in filen

        // hämta data exempel
        string name = parser.getString("name");
        int age = parser.getInt("age");
        
        vector<string> items = parser.getStringList("items");

        // skriv ut data
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "Items:" << endl;
        for (const auto& item : items) {
            cout << "- " << item << endl;
        }
    } catch (const exception& e) {
        cerr << "Fel: " << e.what() << endl;
    }

    return 0;
}