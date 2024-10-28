# tt_bt
> A single header-only implementation of the bt-tree scripting language

![C++](https://img.shields.io/badge/-C++-333333?style=flat&logo=c%2B%2B&logoColor=00599C)
![Git](https://img.shields.io/badge/-Git-333333?style=flat&logo=git)
![GitHub](https://img.shields.io/badge/-GitHub-333333?style=flat&logo=github)
![Markdown](https://img.shields.io/badge/-Markdown-333333?style=flat&logo=markdown)

## Build

```sh
git clone https://github.com/peitianyu/tt_bt.git

cd tt_bt
./rebuid.sh
```

## Script Language Description
```
Each token is separated by ' ', i.e. be careful when using tokens, e.g..
    var v = get_func "task1" √
    var v = get_func"task1" ×
# : single line comments using #
var : means that the variable is only used to get the function
func : denotes a function combination, used only for sequential function combinations.
if ... else ... : judgment function
while : repeatable
repeat : repeat several times
parallal : parallal execution, all functions inside will construct a new thread and execute in parallel.
try catch : try several times, if still fails, go to catch processing, note that as long as the execution of try, catch must be constructed
sleepms : delay of several ms
```

## Usage example

```txt
# func定义脚本
var task1 = get_func "task1" # print 1
var task2 = get_func "task2" # print 2
var task3 = get_func "task3" # print 3
var task4 = get_func "task4" # retrurn 0
var task_5 = get_func "task5" # return 3--
var task6 = get_func "task6"

func local_task { task1 task2}

if task4 { task2 task1 }
else { local_task parallel { task3 task2 } }
    
while task_5 { task3 }

try task6 3 { sleepms 1000 task3 }
catch { task2 }

repeat 3 { task1 }
```

## Development setup

Have no development setup, just !!!`rebuild.sh`!!!

## Meta

Pei tianyu – [wechat](pty2182) – 1216582161@qq.com

Distributed under the MIT license. See LICENSE for more information.

[https://github.com/peitianyu/tt_bt](https://github.com/peitianyu/tt_bt)

## Contributing

1. Fork it (<https://github.com/peitianyu/tt_bt/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request
