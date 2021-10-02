#include <iostream>
#include <functional>
#include <optional>
#include <algorithm>
#include <variant>
#include <fstream>
#include <string>

using namespace std;

class Person {
private:
    string _lastName;
    string _name;
    optional<string> _surName;
public:
    Person(const string& lastName,const string& name,const optional<string>& surName) : _lastName(lastName), _name(name), _surName(surName) {};
    
    friend bool operator<(const Person& left, const Person& right) {
        return tie(left._lastName, left._name, left._surName) < tie(right._lastName, right._name, right._surName);
    };

    friend bool operator==(const Person& left, const Person& right) {
        return tie(left._lastName, left._name, left._surName) == tie(right._lastName, right._name, right._surName);
    };

    friend ostream& operator<< (ostream& out, const Person& person) {
        out.width(12);
        out.fill(' ');
        out << right << person._lastName;
        out.width(15);
        out.fill(' ');
        out << right << person._name;
        out.width(20);
        out.fill(' ');
        if (person._surName.has_value())
            out << right << *person._surName;
        else
            out << right << " ";
        out.width(5);
        out.fill(' ');
        out << " ";
        return out;
    };

    const string& getLastName() const { return _lastName; };
};

class PhoneNumber {
private:
    size_t _countryCode;
    size_t _cityCode;
    string _number;
    optional<size_t> _additinalNumber;
public:
    PhoneNumber(const size_t& countryCode, const size_t& cityCode, const string& number, const optional<size_t>& additionalNumber) : _countryCode(countryCode), _cityCode(cityCode), _number(number), _additinalNumber(additionalNumber) {};
    
    friend ostream& operator<< (ostream& out, const PhoneNumber& phoneNumber) {
        out << "+" << phoneNumber._countryCode << "(" << phoneNumber._cityCode << ")" << phoneNumber._number;
        if (phoneNumber._additinalNumber.has_value())
            out << " " << *phoneNumber._additinalNumber;
        return out;
    };

    friend bool operator<(const PhoneNumber& left, const PhoneNumber& right) {
        return tie(left._countryCode, left._cityCode, left._number, left._additinalNumber) < tie(right._countryCode, right._cityCode, right._number, right._additinalNumber);
    };

    friend bool operator==(const PhoneNumber& left, const PhoneNumber& right) {
        return tie(left._countryCode, left._cityCode, left._number, left._additinalNumber) == tie(right._countryCode, right._cityCode, right._number, right._additinalNumber);
    };
};

class PhoneBook {
private:
    vector<pair<Person, PhoneNumber>> _phoneBook;
public:
    explicit PhoneBook(ifstream& in) {
        if (!in.is_open())
            throw std::runtime_error("File not found");
        while (!in.eof()) {
            string name;
            string lastName;
            string surName;
            int countryCode;
            int cityCode;
            string number;
            string additionalNumber;
            in >> lastName >> name >> surName >> countryCode >> cityCode >> number >> additionalNumber;
            _phoneBook.push_back(pair<Person, PhoneNumber>(Person(lastName, name, surName == "-" ? nullopt : optional<string>(surName)), PhoneNumber(countryCode, cityCode, number, additionalNumber == "-" ? nullopt : optional<size_t>(atoi(additionalNumber.c_str())))));
        }
        in.close();
    };

    friend ostream& operator<< (ostream& out, const PhoneBook& phoneBook) {
        for (const auto& [person, phoneNumber] : phoneBook._phoneBook)
            out << person << " " << phoneNumber << endl;
        return out;
    };

    void SortByName() {
        sort(_phoneBook.begin(), _phoneBook.end(), [](const auto& left,const auto& right) { return left.first < right.first; });
    }

    void SortByPhone() {
        sort(_phoneBook.begin(), _phoneBook.end(), [](const auto& left, const auto& right) { return left.second < right.second; });
    }

    const auto GetPhoneNumber(const string& lastName) const {
        PhoneNumber resultPhone(0,0,"",nullopt);
        string resultString = "";
        int _count = 0;
        for_each(_phoneBook.begin(), _phoneBook.end(), [&_count, lastName, &resultPhone](const auto& row) {
            if (row.first.getLastName()._Equal(lastName)) {
                resultPhone = row.second;
                _count++;
            }
        });
        resultString = _count == 0 ? "not found" : (_count == 1 ? "" : "found more than 1");
        return make_tuple(resultString, resultPhone);
    };

    void ChangePhoneNumber(const Person& person, const PhoneNumber& phoneNumber) {
        auto found = find_if(_phoneBook.begin(), _phoneBook.end(), [person](const auto& row) { return row.first == person; });
        if(found != _phoneBook.end())
            found->second = phoneNumber;
    }
};

int main()
{
    ifstream file("data.txt");
    PhoneBook book(file);
    cout << book;

    cout << "------SortByPhone-------" << endl;
    book.SortByPhone();
    cout << book;

    cout << "------SortByName--------" << endl;
    book.SortByName();
    cout << book;

    cout << "-----GetPhoneNumber-----" << endl;
    // лямбда функция, которая принимает фамилию и выводит номер телефона этого    	человека, либо строку с ошибкой
    auto print_phone_number = [&book](const string& surname) {
        cout << surname << "\t";
        auto answer = book.GetPhoneNumber(surname);
        if (get<0>(answer).empty())
            cout << get<1>(answer);
        else
            cout << get<0>(answer);
        cout << endl;
    };

    // вызовы лямбды
    print_phone_number("Ivanov");
    print_phone_number("Petrov");

    cout << "----ChangePhoneNumber----" << endl;
    book.ChangePhoneNumber(Person{ "Kotov", "Vasilii", "Eliseevich" }, PhoneNumber{ 7, 123, "15344458", nullopt });
    book.ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" }, PhoneNumber{ 16, 465, "9155448", 13 });
    cout << book;
}