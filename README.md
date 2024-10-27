# tt_bt
> A single header-only implementation of the bt-tree scripting language

![C++](https://img.shields.io/badge/-C++-333333?style=flat&logo=c%2B%2B&logoColor=00599C)
![Git](https://img.shields.io/badge/-Git-333333?style=flat&logo=git)
![GitHub](https://img.shields.io/badge/-GitHub-333333?style=flat&logo=github)
![Markdown](https://img.shields.io/badge/-Markdown-333333?style=flat&logo=markdown)



![](header.png)

## Build

```sh
git clone https://github.com/peitianyu/tt_bt.git

cd tt_bt
./rebuid.sh
```

## Usage example

```c++
#include "core/tt_test.h"
#include "core/bt_tree.h"

int task4() { std::cout << "4" << std::endl; return 0; }
int task5() { std::cout << "5" << std::endl; static int i = 3; return i--; }

JUST_RUN_TEST(bt_tree, test)
TEST(bt_tree, test)
{
    BtTree bt_tree;
    bt_tree.RegisterKeyword("task1", [](){ std::cout << "1" << std::endl; return 1; });
    bt_tree.RegisterKeyword("task2", [](){ std::cout << "2" << std::endl; return 1; });
    bt_tree.RegisterKeyword("task3", [](){ std::cout << "3" << std::endl; return 1; });
    bt_tree.RegisterKeyword("task4", task4);
    bt_tree.RegisterKeyword("task5", task5);
    bt_tree.RunScript("../test/test.bt");
}
```
```sh
# func定义脚本
var task1 = get_func "task1" # print 1
var task2 = get_func "task2" # print 2
var task3 = get_func "task3" # print 3
var task4 = get_func "task4" # retrurn 0
var task_5 = get_func "task5" # return 3--

func local_task
{
    task1
    task2
}
    
# 执行脚本
if task4
{
    task2
    task1
}
else 
{
    local_task
    
    parallel 
    {
        task3
        task2
    }
}

while task_5
{
    task3
}
```

## Development setup

Have no development setup, just !!!`rebuild.sh`!!!

## Meta

Pei tianyu – [微信](pty2182) – 1216582161@qq.com

Distributed under the MIT license. See LICENSE for more information.

[https://github.com/peitianyu/tt_bt](https://github.com/peitianyu/tt_bt)

## Contributing

1. Fork it (<https://github.com/peitianyu/tt_bt/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request
