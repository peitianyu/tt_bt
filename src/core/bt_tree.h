#ifndef __BT_TREE_H__
#define __BT_TREE_H__

#include <unordered_map>
#include <vector>
#include <functional>
#include <fstream>
#include <iostream>
#include <thread>

template <typename... Args, typename O>
O & print_base(O &out, Args&&... args) {
    auto a = {(out << std::forward<Args>(args), 0)...};
    (void)a;
    return out;
} 

#define error(...) do { print_base(std::cerr, __VA_ARGS__) << std::endl; exit(1);} while (0)

class BtTree
{
public:
    BtTree() = default;

    using VarMap = std::unordered_map<std::string, std::function<int()>>;

    void RegisterKeyword(std::string name, std::function<int()> func) { m_func[name] = func; }

    void RunScript(const std::string &script)
    {
        std::ifstream ifs(script);
        if(!ifs.is_open()) error("can not open file: ", script);

        VarMap var_map;
        std::unordered_map<std::string, std::vector<std::string>> func_map;
        std::string token;
        while(ifs >> token) {
            if(token == "#") {
                ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue; 
            }
            if(token == "var") VarParse(ifs, var_map);
            else if(token == "func") FuncParse(ifs, var_map, func_map);
            else if(token == "if") IfParse(ifs, var_map, func_map);
            else if(token == "while") WhileParse(ifs, var_map, func_map);
            else if(token == "parallel") ParallelParse(ifs, var_map, func_map);
            else error("invalid token: ", token);
        }
    }
private:    
    void VarParse(std::ifstream &ifs, VarMap& var_map) {
        std::string var_name;
        ifs >> var_name;
        Expect(ifs, "=");
        Expect(ifs, "get_func");

        std::string func_name;
        ifs >> func_name;
        if(!IsStr(func_name)) error("func name is not string: ", func_name);

        auto func = m_func[func_name.substr(1, func_name.size() - 2)];
        if(!func) error("func not found: ", func_name);
        var_map[var_name] = func;
    } 

    bool IsStr(const std::string &str) {
        // FIXME: 这里仅做了最粗略的判断
        if(str[0] != '"' || str[str.size() - 1] != '"') return false;
        return true;
    }

    void FuncParse(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map) {
        std::string func_name;
        ifs >> func_name;

        Expect(ifs, "{");
        
        std::vector<std::string> var_names;
        std::string var_name;
        while(ifs >> var_name) {
            if(var_name == "}") break;
            auto func = var_map[var_name];
            if(!func) error("var not found: ", var_name);
            var_names.push_back(var_name);
        }
        
        func_map[func_name] = var_names;
    }

    void IfParse(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map) {
        std::string if_term;
        ifs >> if_term;

        auto func = var_map[if_term];
        int ret = func();

        Expect(ifs, "{");
        IfExpr(ifs, var_map, func_map, ret);

        if(PeekToken(ifs) == "else") {
            std::string token;
            ifs >> token; // else
            Expect(ifs, "{");
            IfExpr(ifs, var_map, func_map, !ret);
        }
    }

    void WhileParse(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map) {
        std::string while_term;
        ifs >> while_term;
        auto func = var_map[while_term];
        int pos = ifs.tellg();
        
        int ret = func();
        while(ret) {
            ifs.seekg(pos);
            Expect(ifs, "{");
            IfExpr(ifs, var_map, func_map, true);
            ret = func();
        }
    }

    void ParallelParse(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map) {
        Expect(ifs, "{");
        std::string token;
        std::vector<std::thread> threads; // 用于存储线程

        while(ifs >> token) {
            if(token == "}") break;

            auto func = var_map[token];
            if(func) threads.emplace_back([func]() {func();});
            else {
                if(func_map.find(token) == func_map.end()) error("func not found: ", token); 
                for(auto& var : func_map[token]) {
                    auto func = var_map[var];
                    if(func) threads.emplace_back([func]() { func(); });
                    else error("var not found: ", var);
                }
            }
        }

        for(auto& thread : threads) {
            if(thread.joinable()) thread.join();        
        }
    }

    void IfExpr(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map, bool run) {
        std::string token;
        while(ifs >> token) {
            if(token == "}") break;
            if(run) {
                if(token == "if") IfParse(ifs, var_map, func_map);
                else if(token == "while") WhileParse(ifs, var_map, func_map);
                else if(token == "parallel") ParallelParse(ifs, var_map, func_map);
                else {
                    auto func = var_map[token];
                    if(func) func();
                    else {
                        if(func_map.find(token) == func_map.end()) error("func not found: ", token);
                        for(auto& var : func_map[token]) {
                            auto func = var_map[var];
                            if(func) func();
                            else error("var not found: ", var);
                        }
                    }
                }
            }
        }
    }

    void Expect(std::ifstream &ifs, const std::string &expected){
        std::string token;
        ifs >> token;
        if(expected != token) error("expect: ", expected, ", but found: ", token);
    }

    std::string PeekToken(std::ifstream &ifs) {
        std::string token;
        ifs >> token;
        ifs.unget();
        return token;
    }
private:
    VarMap m_func;
};

#endif // __BT_TREE_H__