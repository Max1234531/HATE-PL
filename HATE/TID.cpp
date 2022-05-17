#pragma once
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <set>
#include <queue>
#include <typeinfo>
#include <cmath>

using namespace std;


static int adress = 1;
static int fun_id = 1;

struct Var {
    Var() : type(""), value(""), adrs(-1) {}
    Var(string type_, string value_, int adrs_ = -1) : type(type_), adrs(adrs_) {
        if (value_ == "") {
            if (type == "INT" or type == "CONST") { value = "0"; }
            else if (type == "DOUBLE" or type == "DCONST") { value = "0.0"; }
            else if (type == "BOOL") { value = "0"; }
            else { value = ""; }
        }
        else {
            value = value_;
        }
    }

    string type;
    string value;
    int adrs;
    //for arrays
    vector<Var*> mass;
    bool is_final;
};

struct Fun {
    Fun() : type(""), args({}) {}
    Fun(string name_, string type_, vector<string> args_) : name(name_), type(type_), args(args_) {}
    string type;
    vector<string> args;
    int poliz_begin_index;
    int args_amount;
    string name;
};

class Global_Memory
{
    //vector<stack<Var>> global_memory;
    vector<Var> global_memory;
    map<string, Fun> funs;


public:

    stack<Var> args_stack;
    stack<int> return_stack;


    //ok
    void set_array(int adress_, vector<int>matrix_sizes) {
        string type = global_memory[adress_].type;
        if (adress_ >= global_memory.size()) {
            for (int i = 0; i < 100; i++) { global_memory.push_back(Var("UNDEF", "")); }
        }
        Var Temp = Var(type, "", adress_);
        Temp.is_final = false;
        vector<Var*> cur;
        vector<Var*> ncur;
        Var* Curr;
        cur.push_back(&Temp);
        for (int i = 0; i < matrix_sizes.size(); i++) {
            for (int s = 0; s < cur.size(); s++) {
                Curr = cur[s];
                for (int k = 0; k < matrix_sizes[i]; k++) {
                    Var* Clean = new Var(type, "");
                    if (i != matrix_sizes.size() - 1) {
                        Clean->is_final = false;
                    }
                    Curr->mass.push_back(Clean);
                    ncur.push_back(Clean);
                }
            }
            cur = ncur;
            ncur.clear();

        }

        global_memory[adress_] = Temp;

    }



    //ok
    Var get_array_v(int adress_, vector<int>matrix) {
        if (matrix[0] >= global_memory[adress_].mass.size()) {
            cout << "Array index out of range!";
            exit(3);
        }
        Var* Curr = global_memory[adress_].mass[matrix[0]];
        for (int i = 1; i < matrix.size(); i++) {
            if (matrix[i] >= Curr->mass.size()) {
                cout << "Array index out of range!";
                exit(3);
            }
            Curr = Curr->mass[matrix[i]];
        }
        return *Curr;
    }


    void set_array_v(int adress_, vector<int>matrix, string value_) {
        Var* Curr = global_memory[adress_].mass[matrix[0]];
        for (int i = 1; i < matrix.size(); i++) {
            Curr = Curr->mass[matrix[i]];
        }
        Curr->value = value_;
    }











    //ok
    void set_v(int adress_, Var var) {
        if (adress_ >= global_memory.size()) {
            for (int i = 0; i < 100; i++) { global_memory.push_back(Var("UNDEF", "")); }
        }
        if ((var.type == "INT" and var.value == "") || (var.type == "CONST" and var.value == "")) { var.value = "0"; }
        else if ((var.type == "DOUBLE" and var.value == "") || (var.type == "DCONST" and var.value == "")) { var.value = "0.0"; }
        global_memory[adress_] = var;
    }

    void set_f(string name_, int args_amount_, int poliz_begin_adress) {
        funs[name_].args_amount = args_amount_;
        funs[name_].poliz_begin_index = poliz_begin_adress;
    }

    string get_v_type(int adress_) {
        return global_memory[adress_].type;
    }
    Var get_v(int adress_) {
        return global_memory[adress_];
    }

    int get_f_args_amount(string name) {
        return funs[name].args_amount;
    }
    int get_begin_adress(string name) {
        return funs[name].poliz_begin_index;
    }

    //ok
    void blink(int amount, string type) {
        Var temp(type, "");
        for (int i = 0; i < amount; i++) {
            temp.adrs = adress;
            set_v(adress, temp);
            adress += 1;
        }

    }

    //ok
    void print() {
        for (int i = 1; i < global_memory.size(); i++) {
            if (global_memory[i].type != "UNDEF") { cout << "[" << i << "] " << global_memory[i].type << ": " << global_memory[i].value << " [" << global_memory[i].adrs << "]" << endl; }
        }
        for (pair<string, Fun> p : funs) {
            cout << "[" << p.first << "]" << "{" << p.second.args_amount << "}" << "[" << p.second.poliz_begin_index << "]" << endl;
        }
    }



};

extern Global_Memory Memory;
//Global_Memory Memory;

class TID {


    map<string, Var> var_map;
    map<string, vector<Fun>> fun_map;

public:
    TID(TID* parent_, string name_ = "basic") : parent(parent_), TID_name(name_), return_type(parent_->return_type) {}
    TID() : parent(nullptr) {}

    string TID_name;
    string return_type;
    TID* parent;

    /*void Delete_Table() {
        for (pair<string, Var> p : var_map) {
            Memory.del(p.second.adrs);
        }
    }*/

    bool check_here_v(string name) {
        if (var_map.count(name) != 0) { return true; }
        else {
            return false;
        }
    }
    bool check_v(string name) {
        TID* current = this;
        while (current != nullptr) {
            if (current->var_map.count(name) != 0) { return true; }
            else {
                current = current->parent;
            }
        }
        return false;
    }





    void add_v(string type, string name, string value = "") {
        //if (check_here_v(name)) throw "ÒÛ ×Î ÀÔÅÐÈÑÒ????????????";
        var_map[name] = Var(type, value, adress);
        Memory.set_v(adress, var_map[name]);
        adress += 1;
    }
    string get_type_v(string name) {
        TID* current = this;
        while (!current->check_here_v(name)) {
            current = current->parent;
        }
        return current->var_map[name].type;
    }
    int get_adress_v(string name) {
        TID* current = this;
        while (!current->check_here_v(name)) {
            current = current->parent;
        }
        return current->var_map[name].adrs;
    }


    bool check_here_f(string name, vector<string> args_) {
        if (fun_map.count(name) != 0) {
            for (Fun f : fun_map[name]) {
                if (f.args == args_) { return true; }
            }
        }
        return false;
    }

    bool check_f(string name, vector<string> args_) {
        TID* current = this;
        while (current != nullptr) {
            if (current->check_here_f(name, args_)) { return true; }
            else {
                current = current->parent;
            }
        }
        return false;
    }

    void add_f(string type, vector<string> args_, string name, int poliz_begin_adress) {

        fun_map[name].push_back(Fun("fun" + to_string(fun_id), type, args_));
        Memory.set_f("fun" + to_string(fun_id), args_.size(), poliz_begin_adress);
        fun_id++;
    }

    string get_name_f(string name, vector<string> args_) {
        TID* current = this;
        while (!current->check_here_f(name, args_)) {
            current = current->parent;
        }
        for (Fun f : current->fun_map[name]) {
            if (f.args == args_) {
                return f.name;
            }
        }

    }

    string get_type_f(string name, vector<string> args_) {
        TID* current = this;
        while (!current->check_here_f(name, args_)) {
            current = current->parent;
        }
        for (Fun f : current->fun_map[name]) {
            if (f.args == args_) {
                return f.type;
            }
        }

    }

};

struct GStack {
    stack<string> st;

    bool check_bi() {
        if (st.empty()) return false;
        string op1 = st.top();
        st.pop();
        if (st.empty()) return false;
        string act = st.top();
        st.pop();
        if (st.empty()) return false;
        string op2 = st.top();
        st.pop();

        if (op1.find("POINTER") != std::string::npos) { op1 = "POINTER"; }
        if (op2.find("POINTER") != std::string::npos) { op2 = "POINTER"; }


        if (op1 == "VOID" and op2 == "VOID" and act == "=") {
            return true;
        }

        if (op1 == "CONST" and op2 == "CONST" and act == "SIMPLE SIGN") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "COMPLEX SIGN") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "COMPARE OPERATOR") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "OR") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "AND") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "^") {
            st.push("CONST");
            return true;
        }




        else if (op1 == "DCONST" and op2 == "DCONST" and act == "SIMPLE SIGN") {
            st.push("DCONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "COMPLEX SIGN") {
            st.push("DCONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "COMPARE OPERATOR") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "OR") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "AND") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "^") {
            st.push("DCONST");
            return true;
        }




        else if (op1 == "DCONST" and op2 == "CONST" and act == "SIMPLE SIGN") {
            st.push("DCONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "SIMPLE SIGN") {
            st.push("DCONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "COMPLEX SIGN") {
            st.push("DCONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "COMPLEX SIGN") {
            st.push("DCONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "COMPARE OPERATOR") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "COMPARE OPERATOR") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "OR") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "OR") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "AND") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "AND") {
            st.push("CONST");
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "^") {
            st.push("DCONST");
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "^") {
            st.push("DCONST");
            return true;
        }






        else if (op1 == "CONST" and op2 == "POINTER" and act == "SIMPLE SIGN") {
            st.push("POINTER");
            return true;
        }
        else if (op1 == "POINTER" and op2 == "CONST" and act == "SIMPLE SIGN") {
            st.push("POINTER");
            return true;
        }



        else if (op1 == "STRING" and op2 == "STRING" and act == "SIMPLE SIGN") {
            st.push("STRING");
            return true;
        }
        else if (op1 == "STRING" and op2 == "STRING" and act == "COMPARE OPERATOR") {
            st.push("CONST");
            return true;
        }



        // buff == "=" || buff == "-=" || buff == "+=" || buff == "/=" || buff == "*="

        else if (op1 == "CONST" and op2 == "CONST" and act == "=") {
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "-=") {
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "+=") {
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "/=") {
            return true;
        }
        else if (op1 == "CONST" and op2 == "CONST" and act == "*=") {
            return true;
        }

        else if (op1 == "DCONST" and op2 == "DCONST" and act == "=") {
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "-=") {
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "+=") {
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "/=") {
            return true;
        }
        else if (op1 == "DCONST" and op2 == "DCONST" and act == "*=") {
            return true;
        }


        else if (op1 == "DCONST" and op2 == "CONST" and act == "=") {
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "-=") {
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "+=") {
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "/=") {
            return true;
        }
        else if (op1 == "DCONST" and op2 == "CONST" and act == "*=") {
            return true;
        }

        else if (op1 == "CONST" and op2 == "DCONST" and act == "=") {
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "-=") {
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "+=") {
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "/=") {
            return true;
        }
        else if (op1 == "CONST" and op2 == "DCONST" and act == "*=") {
            return true;
        }



        else if (op1 == "POINTER" and op2 == "POINTER" and act == "=") {
            return true;
        }
        else if (op1 == "STRING" and op2 == "STRING" and act == "=") {
            return true;
        }
        else if (op1 == "STRING" and op2 == "STRING" and act == "+=") {
            return true;
        }

        else return false;
    }

    bool check_un() {
        set<string> test;
        if (st.empty()) return false;
        test.insert(st.top());
        st.pop();
        if (st.empty()) return false;
        test.insert(st.top());
        st.pop();


        if (test.count("CONST") != 0 and test.count("SIMPLE SIGN") != 0) {
            st.push("CONST");
            return true;
        }
        else if (test.count("CONST") != 0 and test.count("INCREMENT") != 0) {
            st.push("CONST");
            return true;
        }
        else if (test.count("DCONST") != 0 and test.count("INCREMENT") != 0) {
            st.push("DCONST");
            return true;
        }
        else if (test.count("DCONST") != 0 and test.count("SIMPLE SIGN") != 0) {
            st.push("DCONST");
            return true;
        }
        else if (test.count("CONST") != 0 and test.count("!") != 0) {
            st.push("CONST");
            return true;
        }
        else return false;





    }

    void push(string lex) {
        st.push(lex);
    }

    string pop() {
        string ans = st.top();
        st.pop();
        return ans;
    }
    string top() {
        return st.top();
    }

    string get_type(string type) {
        if (type == "CHAR" || type == "STRING CONST") return "STRING";
        else if (type == "INT" || type == "BOOL") return "CONST";
        else if (type == "DOUBLE") return "DCONST";
        else if (type.find('*') != std::string::npos) return get_type(type.substr(0, type.find('*'))) + "_POINTER";
        else return type;
    }
};

enum class Type { var_link, service, int_const, double_const, string_const, const_, operation, var_value, function_call, indexation, array_link, arr_creation, indexation_value, poliz_adress, type_change };
inline string type_to_str(Type type) {
    switch (type)
    {
    case Type::var_link:		    return "var_link";
    case Type::service:			    return "service";
    case Type::const_:			    return "const";
    case Type::int_const:			return "int_const";
    case Type::double_const:		return "double_const";
    case Type::string_const:		return "string_const";
    case Type::indexation:		    return "indexation";
    case Type::operation:		    return "operation";
    case Type::var_value:		    return "var_value";
    case Type::function_call:	    return "function_call";
    case Type::poliz_adress:        return "poliz_adress";
    case Type::arr_creation:        return "arr_creation";
    case Type::indexation_value:    return "indexation_value";
    case Type::array_link:          return "array_link";
    case Type::type_change:         return "type_change";


    default:				        return "[Unknown Type]";
    }
}

struct element {
    element(string sval_, Type tp_) : value(sval_), type(tp_), args_amount(0) {}
    element(string sval_, Type tp_, int am_) : value(sval_), type(tp_), args_amount(am_) {}
    element(Var var) {
        if (var.type == "CONST") type = Type::int_const;
        else if (var.type == "DCONST") type = Type::double_const;
        else type = Type::string_const;
        value = var.value;
    }
    string value;
    Type type;
    //for functions only:
    int args_amount;
};
#pragma region shaman
inline element operator+ (element c1, element c2) {
    if (c1.type == Type::string_const || c2.type == Type::string_const) {
        return element(c1.value + c2.value, Type::string_const);
    }
    else if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) + stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) + stoi(c2.value)), Type::int_const);
    }
}
inline element operator- (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) - stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) - stoi(c2.value)), Type::int_const);
    }
}
inline element operator/ (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) / stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) / stoi(c2.value)), Type::int_const);
    }
}
inline element operator* (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) * stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) * stoi(c2.value)), Type::int_const);
    }
}
inline element operator|| (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) || stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) || stoi(c2.value)), Type::int_const);
    }
}
inline element operator&& (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) && stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) && stoi(c2.value)), Type::int_const);
    }
}
inline element operator! (element c1) {
    if (c1.value == "0") {
        return element("1", Type::int_const);
    }
    else {
        return element("0", Type::int_const);
    }
}
inline element operator++ (element c1) {
    if (c1.type == Type::double_const) {
        return element(to_string(stod(c1.value) + 1), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) + 1), Type::int_const);
    }
}
inline element operator-- (element c1) {
    if (c1.type == Type::double_const) {
        return element(to_string(stod(c1.value) - 1), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) - 1), Type::int_const);
    }
}
inline element operator> (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) > stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) > stoi(c2.value)), Type::int_const);
    }
}
inline element operator< (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) < stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) < stoi(c2.value)), Type::int_const);
    }
}
inline element operator>= (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) >= stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) >= stoi(c2.value)), Type::int_const);
    }
}
inline element operator<= (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) <= stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) <= stoi(c2.value)), Type::int_const);
    }
}
inline element operator== (element c1, element c2) {
    if (c1.type == Type::string_const || c2.type == Type::string_const) {
        return element(to_string(c1.value == c2.value), Type::int_const);
    }
    else if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) == stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) == stoi(c2.value)), Type::int_const);
    }
}
inline element operator!= (element c1, element c2) {
    if (c1.type == Type::string_const || c2.type == Type::string_const) {
        return element(to_string(c1.value != c2.value), Type::int_const);
    }
    else if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(stod(c1.value) != stod(c2.value)), Type::double_const);
    }
    else {
        return element(to_string(stoi(c1.value) != stoi(c2.value)), Type::int_const);
    }
}
inline element operator% (element c1, element c2) {
    return element(to_string(stoi(c1.value) % stoi(c2.value)), Type::int_const);
}
inline element operator>> (element c1, element c2) {
    if (c1.type == Type::double_const || c2.type == Type::double_const) {
        return element(to_string(pow(stod(c1.value), stod(c2.value))), Type::double_const);
    }
    else {
        return element(to_string(pow(stoi(c1.value), stoi(c2.value))), Type::int_const);
    }
}

#pragma endregion



//a1 > a2;
class Poliz {
    vector<element> trip;
    //Table table;
    stack<element> acts;
    stack<vector<element>> vars;
    map<string, int> prioritys = {
        {"=", 0},
        {"+=", 0},
        {"-=", 0},
        {"/=", 0},
        {"*=", 0},
        {"(", 0},
        {"[", 0},
        {"==", 1},
        {">=", 1},
        {"<=", 1},
        {"!=", 1},
        {">", 1},
        {"<", 1},
        {"or", 2},
        {"and", 3},
        {"+", 4},
        {"-", 4},
        {"*", 5},
        {"/", 5},
        {"%", 5},
        {"^", 6},
        {"++", 6},
        {"--", 6},
        {"!", 7},
        {"_+_", 7},
        {"_-_", 7},
        {"_*_", 7},

    };


public:

    //àíàëèçèðóåò òåêóùóþ ëåêñåìó è ïðèíèìàåò ðåøåíèå ïî âûçîâó íóæíîé ôóíêöèè
    void analise(string lex, string buff, Type type, int args_amount = 0) {
        if ((lex == "ID" && type != Type::function_call and type != Type::indexation and type != Type::arr_creation and type != Type::indexation_value and type != Type::type_change) || lex == "DCONST" || lex == "CONST" || lex == "STRING CONST" || lex == "TRUE" || lex == "FALSE") {
            if (lex == "STRING CONST") { buff = buff.substr(1, buff.size() - 2); }
            var_add(buff, type);
        }
        else if (buff == "+" || buff == "-" || buff == "*" || buff == "/"
            || buff == "+=" || buff == "-=" || buff == "*=" || buff == "/="
            || buff == "=" || buff == "==" || buff == "or" || buff == "and"
            || buff == "<=" || buff == ">=" || buff == ">" || buff == "<"
            || buff == "!=" || buff == "!" || buff == "%" || buff == "++" || buff == "--") {
            if (acts.size() != 0) {
                while (acts.size() > 0 && (prioritys[acts.top().value] >= prioritys[buff])) {
                    staks_operation();
                }
            }
            act_add(buff, Type::operation);
        }
        else if (buff == "^") {
            if (acts.size() != 0) {
                while (acts.size() > 0 && (prioritys[acts.top().value] > prioritys[buff])) {
                    staks_operation();
                }
            }
            act_add(buff, Type::operation);
        }
        else if (buff == "(") {
            act_add(buff, Type::operation);
        }
        else if (buff == "[") {
            act_add(buff, Type::operation);
        }
        else if (buff == ")") {
            while (acts.top().value != "(") {
                staks_operation();
            }
            acts.pop();
            if (acts.size() != 0 and (acts.top().type == Type::function_call or acts.top().type == Type::type_change)) {
                staks_operation();
            };
        }
        else if (buff == "]") {
            acts.pop();
            staks_operation();
        }
        else if (lex == "ID" and type == Type::function_call) {
            act_add(buff, Type::function_call, args_amount);
        }
        else if (lex == "ID" and type == Type::indexation) {
            act_add(buff, Type::indexation, args_amount);
        }
        else if (lex == "ID" and type == Type::arr_creation) {
            act_add(buff, Type::arr_creation, args_amount);
        }
        else if (lex == "ID" and type == Type::type_change) {
            act_add(buff, Type::type_change, args_amount);
        }
        else if (lex == "ID" and type == Type::indexation_value) {
            act_add(buff, Type::indexation_value, args_amount);
        }
        else if (buff == ",") {
            while (acts.top().value != "(") {
                staks_operation();
            }
        }
        else if (buff == ",,") {
            while (acts.top().value != "[") {
                staks_operation();
            }
        }
        else if (buff == ";") {
            add_to_answer();
        }


    }
    string value_here(int index) {
        return trip[index].value;
    }
    void var_add(string var, Type type) {
        vars.push({ element(var,type) });
    }
    void act_add(string act, Type type, int args_amount = 0) {
        acts.push(element(act, type, args_amount));
    }
    void add_to_answer() {
        if (acts.size() != 0) {
            while (acts.size() != 0) {
                staks_operation();
            }
        }
        while (vars.size() != 0) {
            for (element i : vars.top()) {
                trip.push_back(i);
            }
            vars.pop();
        }
    }

    string im_callin_de_polis() {
        string ans;
        for (element word : trip) {
            ans += "<" + word.value + ":" + type_to_str(word.type) + " " + to_string(word.args_amount) + ">";
        }
        return ans;
    }
    string im_callin_de_simple_polis() {
        string ans;
        for (int i = 0; i < trip.size(); i++) {
            ans += "[" + to_string(i) + "]" + trip[i].value + "\n";
        }
        return ans;
    }

    int get_current_index() {
        return trip.size();
    }
    void dash() {
        trip.push_back(element("<_>", Type::service));
    }

    /*void push(string str, Type type) {
        trip.push_back(element(str, type));
    }*/
    void replace_here(string value, Type type, int index) {
        trip[index] = element(value, type);
    }
    void insert_here(string value, Type type, int index) {
        trip.insert(trip.begin() + index, element(value, type));
    }
    void push(string value, Type type) {
        trip.push_back(element(value, type));
    }

    void fix_fun(string first_name, string second_name, int args_amount_) {
        stack<element> helper;
        while (!(acts.top().value == first_name && acts.top().type == Type::function_call)) {
            helper.push(acts.top());
            acts.pop();
        }
        acts.top().value = second_name;
        acts.top().args_amount = args_amount_;
        while (helper.size() != 0) {
            acts.push(helper.top());
            helper.pop();
        }
    }

    void fix_arr(string first_name, string second_name, int args_amount_) {
        stack<element> helper;
        while (!(acts.top().value == first_name && acts.top().type == Type::arr_creation)) {
            helper.push(acts.top());
            acts.pop();
        }
        acts.top().value = second_name;
        acts.top().args_amount = args_amount_;
        while (helper.size() != 0) {
            acts.push(helper.top());
            helper.pop();
        }
    }
    void fix_arr_call(string first_name, string second_name, int args_amount_) {
        stack<element> helper;
        while (!(acts.top().value == first_name && acts.top().type == Type::indexation)) {
            helper.push(acts.top());
            acts.pop();
        }
        acts.top().value = second_name;
        acts.top().args_amount = args_amount_;
        while (helper.size() != 0) {
            acts.push(helper.top());
            helper.pop();
        }
    }
    void fix_arr_call_value(string first_name, string second_name, int args_amount_) {
        stack<element> helper;
        while (!(acts.top().value == first_name && acts.top().type == Type::indexation_value)) {
            helper.push(acts.top());
            acts.pop();
        }
        acts.top().value = second_name;
        acts.top().args_amount = args_amount_;
        while (helper.size() != 0) {
            acts.push(helper.top());
            helper.pop();
        }
    }
    void pop_acts(int pops_amount) {
        for (int i = 0; i < pops_amount; i++) {
            acts.pop();
        }
    }
    void staks_operation() {
        if (acts.top().value == "+" || acts.top().value == "-" || acts.top().value == "*" || acts.top().value == "/"
            || acts.top().value == "+=" || acts.top().value == "-=" || acts.top().value == "*=" || acts.top().value == "/="
            || acts.top().value == "=" || acts.top().value == "==" || acts.top().value == "or" || acts.top().value == "and"
            || acts.top().value == "<=" || acts.top().value == ">=" || acts.top().value == ">" || acts.top().value == "<"
            || acts.top().value == "!=" || acts.top().value == "%" || acts.top().value == "^") {
            vector<element> right = vars.top();
            vars.pop();
            vector<element> left = vars.top();
            vars.pop();
            vector<element> ans;
            for (element i : left) {
                ans.push_back(i);
            }
            for (element i : right) {
                ans.push_back(i);
            }
            ans.push_back(acts.top());
            //string ans = (left.type == Type::link ? table.getlink(left.value) : table.getvalue(left.value)) + right.value + acts.top().value;
            acts.pop();
            vars.push(ans);
        }

        else if (acts.top().value == "!" || acts.top().value == "++" || acts.top().value == "--") {
            vector<element> left = vars.top();
            vars.pop();
            vector<element> ans;
            for (element i : left) {
                ans.push_back(i);
            }
            ans.push_back(acts.top());
            acts.pop();
            vars.push(ans);
        }
        else if (acts.top().type == Type::function_call || acts.top().type == Type::arr_creation || acts.top().type == Type::indexation || acts.top().type == Type::indexation_value || acts.top().type == Type::type_change) {
            vector<element> ans;
            stack<vector<element>> help;
            for (int i = 0; i < acts.top().args_amount; i++) {
                help.push(vars.top());
                vars.pop();
            }
            for (int i = 0; i < acts.top().args_amount; i++) {
                for (element k : help.top()) {
                    ans.push_back(k);
                }
                help.pop();
            }
            ans.push_back(acts.top());
            acts.pop();
            vars.push(ans);
        }
        else {
            cout << "POLIZPOLIZ";
            throw 1111111111111;
        }
    }

    //template <typename T>
    //Type define_type(T var) {
    //    if (typeid(var).name()[0] == 'i' || typeid(var).name()[0] == 'b') { return Type::int_const; }
    //    if (typeid(var).name()[0] == 'd') { return Type::double_const; }
    //    else {
    //        return Type::string_const;
    //    };
    //}

    vector<string> Split(string s, string delimiter = " ") {

        vector <string> ans;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            ans.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        ans.push_back(s);
        return ans;
    }

    void release_poliz() {
        Memory.return_stack.push(get_current_index());
        stack<element> rel_stack;
        for (int i = 0; i < trip.size(); i++) {
            if (i >= trip.size()) { break; }
            switch (trip[i].type)
            {
            case Type::var_link: {
                rel_stack.push(trip[i]);
                continue;
            }
            case Type::type_change: {
                element arg = rel_stack.top();
                rel_stack.pop();
                if (trip[i].value == "STR") {
                    arg.type = Type::string_const;
                    rel_stack.push(arg);
                }
                else if (trip[i].value == "INT") {
                    arg.type = Type::int_const;
                    rel_stack.push(arg);
                }
                else if (trip[i].value == "DBL") {
                    arg.type = Type::double_const;
                    rel_stack.push(arg);
                }

                continue;
            }
            case Type::service: {
                if (trip[i].value == "goto") {
                    i = stoi(rel_stack.top().value) - 1;
                    rel_stack.pop();
                    continue;
                }
                else if (trip[i].value == "goto_F") {
                    int adr = stoi(rel_stack.top().value) - 1;
                    rel_stack.pop();
                    if (rel_stack.top().value == "0") {
                        i = adr;
                    }
                    rel_stack.pop();
                    continue;
                }
                else if (trip[i].value == "return_stack.top()") {
                    rel_stack.push(element(to_string(Memory.return_stack.top()), Type::poliz_adress));
                    Memory.return_stack.pop();
                }
                else if (trip[i].value == "args_stack.top()") {
                    if (Memory.args_stack.top().type == "CONST") rel_stack.push(element(Memory.args_stack.top().value, Type::int_const));
                    else if (Memory.args_stack.top().type == "DCONST") rel_stack.push(element(Memory.args_stack.top().value, Type::double_const));
                    else rel_stack.push(element(Memory.args_stack.top().value, Type::string_const));
                    Memory.args_stack.pop();
                }
                else if (trip[i].value == "<<") {
                    cout << rel_stack.top().value;
                    rel_stack.pop();
                }
                else if (trip[i].value == ">>") {
                    string arg;
                    cin >> arg;
                    element var = rel_stack.top();
                    Memory.set_v(stoi(var.value), Var(Memory.get_v_type(stoi(var.value)), arg, stoi(var.value)));
                    rel_stack.pop();
                }
                continue;
            }
            case Type::int_const: {
                rel_stack.push(trip[i]);
                continue;
            }
            case Type::double_const: {
                rel_stack.push(trip[i]);
                continue;
            }
            case Type::string_const: {
                rel_stack.push(trip[i]);
                continue;
            }
            case Type::operation: {
                if (trip[i].value == "=" || trip[i].value == "+=" || trip[i].value == "-=" || trip[i].value == "*=" || trip[i].value == "/=") {
                    element arg = rel_stack.top();
                    rel_stack.pop();

                    if (rel_stack.top().type == Type::array_link) {
                        vector<string> matrix = Split(rel_stack.top().value);
                        vector<int> good_matrix;
                        int adr = stoi(matrix[0]);
                        matrix.erase(matrix.begin());
                        std::transform(matrix.begin(), matrix.end(), std::back_inserter(good_matrix), [](const std::string& str) { return std::stoi(str); });


                        if (trip[i].value == "=") {
                            Memory.set_array_v(adr, good_matrix, arg.value);
                        }
                        else if (trip[i].value == "+=") {
                            element t1 = element(Memory.get_array_v(adr, good_matrix));
                            element anss = t1 + arg;
                            Memory.set_array_v(adr, good_matrix, anss.value);
                        }
                        else if (trip[i].value == "-=") {
                            element t1 = element(Memory.get_array_v(adr, good_matrix));
                            element anss = t1 - arg;
                            Memory.set_array_v(adr, good_matrix, anss.value);
                        }
                        else if (trip[i].value == "*=") {
                            element t1 = element(Memory.get_array_v(adr, good_matrix));
                            element anss = t1 * arg;
                            Memory.set_array_v(adr, good_matrix, anss.value);
                        }
                        else if (trip[i].value == "/=") {
                            element t1 = element(Memory.get_array_v(adr, good_matrix));
                            element anss = t1 / arg;
                            Memory.set_array_v(adr, good_matrix, anss.value);
                        }
                    }
                    else {
                        int adr = stoi(rel_stack.top().value);
                        rel_stack.pop();

                        if (trip[i].value == "=") {
                            Memory.set_v(adr, Var(Memory.get_v_type(adr), arg.value, adr));
                        }
                        else if (trip[i].value == "+=") {
                            Memory.set_v(adr, Var(Memory.get_v_type(adr), (element(Memory.get_v(adr)) + arg).value, adr));
                        }
                        else if (trip[i].value == "-=") {
                            Memory.set_v(adr, Var(Memory.get_v_type(adr), (element(Memory.get_v(adr)) - arg).value, adr));
                        }
                        else if (trip[i].value == "*=") {
                            Memory.set_v(adr, Var(Memory.get_v_type(adr), (element(Memory.get_v(adr)) * arg).value, adr));
                        }
                        else if (trip[i].value == "/=") {
                            Memory.set_v(adr, Var(Memory.get_v_type(adr), (element(Memory.get_v(adr)) / arg).value, adr));
                        }
                    }

                }
                else if (trip[i].value == "++" || trip[i].value == "--" || trip[i].value == "!") {
                    element arg = rel_stack.top();
                    rel_stack.pop();
                    if (trip[i].value == "++") {
                        rel_stack.push(++arg);
                    }
                    else if (trip[i].value == "--") {
                        rel_stack.push(--arg);
                    }
                    else if (trip[i].value == "!") {
                        rel_stack.push(!arg);
                    }

                }
                else {
                    element arg2 = rel_stack.top();
                    rel_stack.pop();
                    element arg1 = rel_stack.top();
                    rel_stack.pop();
                    if (trip[i].value == "+") {
                        rel_stack.push(arg1 + arg2);
                    }
                    else if (trip[i].value == "-") {
                        rel_stack.push(arg1 - arg2);
                    }
                    else if (trip[i].value == "*") {
                        rel_stack.push(arg1 * arg2);
                    }
                    else if (trip[i].value == "/") {
                        rel_stack.push(arg1 / arg2);
                    }
                    else if (trip[i].value == "or") {
                        rel_stack.push(arg1 || arg2);
                    }
                    else if (trip[i].value == "and") {
                        rel_stack.push(arg1 && arg2);
                    }
                    else if (trip[i].value == "==") {
                        rel_stack.push(arg1 == arg2);
                    }
                    else if (trip[i].value == "!=") {
                        rel_stack.push(arg1 != arg2);
                    }
                    else if (trip[i].value == ">") {
                        rel_stack.push(arg1 > arg2);
                    }
                    else if (trip[i].value == "<") {
                        rel_stack.push(arg1 < arg2);
                    }
                    else if (trip[i].value == ">=") {
                        rel_stack.push(arg1 >= arg2);
                    }
                    else if (trip[i].value == "<=") {
                        rel_stack.push(arg1 <= arg2);
                    }
                    else if (trip[i].value == "%") {
                        rel_stack.push(arg1 % arg2);
                    }
                    else if (trip[i].value == "^") {
                        rel_stack.push(arg1 >> arg2);
                    }
                }


                continue;
            }
            case Type::var_value: {
                if (Memory.get_v_type(stoi(trip[i].value)) == "CONST") { rel_stack.push(element(Memory.get_v(stoi(trip[i].value)).value, Type::int_const)); }
                else if (Memory.get_v_type(stoi(trip[i].value)) == "DCONST") { rel_stack.push(element(Memory.get_v(stoi(trip[i].value)).value, Type::double_const)); }
                else if (Memory.get_v_type(stoi(trip[i].value)) == "STRING") { rel_stack.push(element(Memory.get_v(stoi(trip[i].value)).value, Type::string_const)); }
                continue;
            }
            case Type::function_call: {
                //ïðè èñïîëíåíèè ïîëèçà êîíñòðóêöèÿ "1 2 fun" áóäåò âûïîëíÿòüñÿ òàê:
                //args_stack.push(2) args_stack.push(1) return_stack.push(poliz.get_curr_adress)
                //goto Memory.get_begin_adress("fun") arg1 = args_stack.top() arg2 = args_stack.top()
                for (int j = 0; j < Memory.get_f_args_amount(trip[i].value); j++) {
                    if (rel_stack.top().type == Type::int_const) Memory.args_stack.push(Var("CONST", rel_stack.top().value));
                    else if (rel_stack.top().type == Type::double_const) Memory.args_stack.push(Var("DCONST", rel_stack.top().value));
                    else Memory.args_stack.push(Var("STRING", rel_stack.top().value));
                    rel_stack.pop();
                }
                Memory.return_stack.push(i + 1);
                i = Memory.get_begin_adress(trip[i].value) - 1;
                continue;
            }
            case Type::arr_creation: {
                stack<int> helper;
                vector<int> mmm;
                for (int j = 0; j < trip[i].args_amount; j++) {
                    helper.push(stoi(rel_stack.top().value));
                    rel_stack.pop();
                }
                int repeat = helper.size();
                for (int j = 0; j < repeat; j++) {
                    mmm.push_back(helper.top());
                    helper.pop();
                }
                Memory.set_array(stoi(trip[i].value), mmm);
                continue;
            }
            case Type::indexation: {
                string kkk = trip[i].value;

                stack<int> helper;
                vector<int> mmm;
                for (int j = 0; j < trip[i].args_amount; j++) {
                    helper.push(stoi(rel_stack.top().value));
                    rel_stack.pop();
                }
                int repeat = helper.size();
                for (int j = 0; j < repeat; j++) {
                    mmm.push_back(helper.top());
                    helper.pop();
                }

                for (int j = 0; j < mmm.size(); j++) {
                    kkk = kkk + " " + to_string(mmm[j]);
                }

                rel_stack.push(element(kkk, Type::array_link));
                continue;
            }
            case Type::indexation_value: {
                stack<int> helper;
                vector<int> mmm;
                for (int j = 0; j < trip[i].args_amount; j++) {
                    helper.push(stoi(rel_stack.top().value));
                    rel_stack.pop();
                }
                int repeat = helper.size();
                for (int j = 0; j < repeat; j++) {
                    mmm.push_back(helper.top());
                    helper.pop();
                }

                Var t = Memory.get_array_v(stoi(trip[i].value), mmm);
                rel_stack.push(element(t));
                continue;



            }

            case Type::poliz_adress: {
                rel_stack.push(trip[i]);
                continue;
            }

            default:				cout << "ÒÛ ÏÎÕÎÄÓ ÏÅÐÅÏÓÒÀË....."; throw 11;
            }
        }
    }
};



