# Tai-su
這是 2021 年的師大進階程式設計作業：Legend of Code and Magic

## 名稱由來
由「大師」臺語羅馬拼音：Tāi-su 簡化而來

## folder structure
```
.
├── authors.json   # author info shall be recorded here
├── auto_report.py
├── code           # your workplace
│   ├── main.cpp
│   ├── makefile   
│   ├── tai-su.cpp # main library
│   └── tai-su.h   # main library header
├── main_stub.cpp
├── make_c.py
├── makefile       # export production makefile
├── readme.md
├── requirements.txt # python requirements
└── tcc_template.docx # docx template
```

## 核心程式碼
code 資料夾中的 main.cpp 和 tau-su.cpp 為可供使用者使用的程式碼  
tai-su.cpp 的最下面有提供使用者定義計算行動分數的 functions ，注意是分數越低的越會被優先選取

## how 2 use
    
```sh
make
```
* Answer.cpp:
    
    the submission for coding game 
* ```studentId```.pdf/.zip
    
    the file submit to moodle
```sh
make clean # clean the current build
```
## how it works
[a work similar to this project](https://github.com/AlaRduTP/GOD) has already conducted. however, this project does not preserve some properties for binary and different compilers. for instance if you want to use the lastest c++20, you may suffer that the online judge does not support the syntax you need. as a result, this project use a different approach. it compiles the object code **locally** and inject it to the **stub code** then deflated to the remote server(see main_stub.cpp and make_c.py for further details). In addition to code generation, it also automaticially generate report and packed code on given template file(```tcc_template.docx```). it will scan the style inside the word file and write report depending on what you tracked in your git repository. author informations and student id prefix can be set in ```authors.json``` and ```makefile```.

**NOTE**: **DO NOT** track **binary files** for git. it will lead to undefined behaviours.

# requirements
* clang-format
* python3
* unoconv
    ```sh
    pip3 install -r requirements.txt && apt install unoconv
    ```
