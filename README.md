# contract

      create a new folder to save contract file, for example helloworld
      enter the contract folder, then create helloworld.cpp and helloworld.hpp

## Compile

      enter the contract folder, for example "cd testHelloWorld", then compile cpp:
      python ../gentool.py --type common --file helloworld.cpp
      then common.hpp will be created. next add '#include "common.hpp"' as the last "#include" to your helloWorld.cpp.
      
      enter the contract folder, for example "cd testHelloWorld", then compile cpp:
      python ../gentool.py --type wasm --file helloworld.cpp
    
      then helloworld.wast and helloworld.wasm will be created.
    
      generate abi file:
      python ../gentool.py --file helloworld.hpp
    
      then helloworld.abi will be created.

​      
ref to https://github.com/WebAssembly/wabt