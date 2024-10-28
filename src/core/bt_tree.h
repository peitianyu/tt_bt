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
        while(!ifs.eof()) {
            token = GetToken(ifs);
            if(token == "") break;
            else if(token == "var")     VarParse(ifs, var_map);
            else if(token == "func")    FuncParse(ifs, var_map, func_map);
            else if(token == "if")      IfParse(ifs, var_map, func_map);
            else if(token == "while")   WhileParse(ifs, var_map, func_map);
            else if(token == "parallel") ParallelParse(ifs, var_map, func_map);
            else if(token == "try")     TryParse(ifs, var_map, func_map);
            else if(token == "repeat")  RepeatParse(ifs, var_map, func_map);
            else error("invalid token: ", token);
        }

        ifs.close();
    }
private:    
    void VarParse(std::ifstream &ifs, VarMap& var_map) {
        std::string var_name = GetToken(ifs);
        Expect(ifs, "=");
        Expect(ifs, "get_func");

        std::string func_name = GetToken(ifs);
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
        std::string func_name = GetToken(ifs);

        Expect(ifs, "{");
        
        std::vector<std::string> var_names;
        std::string var_name;
        while(1) {
            var_name = GetToken(ifs);
            if(var_name == "}") break;
            auto func = var_map[var_name];
            if(!func) error("var not found: ", var_name);
            var_names.push_back(var_name);
        }
        
        func_map[func_name] = var_names;
    }

    void IfParse(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map) {
        std::string if_term = GetToken(ifs);

        auto func = var_map[if_term];
        int ret = func();

        Expect(ifs, "{");
        IfExpr(ifs, var_map, func_map, ret);

        // FIXME: 这部分token不够严谨, 可能存在bug
        if(PeekToken(ifs) == "else") {
            std::string token;
            ifs >> token; // else
            Expect(ifs, "{");
            IfExpr(ifs, var_map, func_map, !ret);
        }
    }

    void WhileParse(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map) {
        std::string while_term = GetToken(ifs);
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
        std::vector<std::thread> threads;

        while(1) {
            token = GetToken(ifs);
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

    void TryParse(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map) {
        std::string try_term = GetToken(ifs);

        int cnt_num = atoi(GetToken(ifs).c_str());
        if(cnt_num <= 0) error("cnt_num <= 0: ", cnt_num);
        int pos = ifs.tellg();

        while(cnt_num--) {
            Expect(ifs, "{");
            auto func = var_map[try_term];
            if(!func) error("func not found: ", try_term); 
            if(func()) {
                IfExpr(ifs, var_map, func_map, true);
                break;
            }
            ifs.seekg(pos);
        }

        Expect(ifs, "catch");
        Expect(ifs, "{");
        IfExpr(ifs, var_map, func_map, (cnt_num == 0));
    }

    void RepeatParse(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map) {
        int cnt_num = atoi(GetToken(ifs).c_str());
        if(cnt_num <= 0) error("cnt_num <= 0: ", cnt_num);
        int pos = ifs.tellg();
        while(cnt_num--) {
            ifs.seekg(pos);
            Expect(ifs, "{");
            IfExpr(ifs, var_map, func_map, true);
        }
    }

    void SleepParse(std::ifstream &ifs) {
        int sleep_time = atoi(GetToken(ifs).c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }

    void IfExpr(std::ifstream &ifs, VarMap& var_map, std::unordered_map<std::string, std::vector<std::string>>& func_map, bool run) {
        std::string token;
        while(1) {
            token = GetToken(ifs);
            if(token == "}") break;
            if(run) {
                if(token == "if") IfParse(ifs, var_map, func_map);
                else if(token == "while") WhileParse(ifs, var_map, func_map);
                else if(token == "parallel") ParallelParse(ifs, var_map, func_map);
                else if(token == "try") TryParse(ifs, var_map, func_map);
                else if(token == "repeat") RepeatParse(ifs, var_map, func_map);
                else if(token == "sleepms") SleepParse(ifs);
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
        std::string token = GetToken(ifs);
        if(expected != token) error("expect: ", expected, ", but found: ", token);
    }

    std::string PeekToken(std::ifstream &ifs) {
        std::string token = GetToken(ifs);
        ifs.unget();
        return token;
    }

    std::string GetToken(std::ifstream &ifs) {
        std::string token;
        ifs >> token;
        while(token == "#" && !ifs.eof()) {
            ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            ifs >> token; 
        }
        if(token == "#") return "";
        return token;
    }
private:
    VarMap m_func;
};

#endif // __BT_TREE_H__