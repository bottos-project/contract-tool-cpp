# contract

      create a new folder to save contract file, for example helloworld
      enter the contract folder, then create helloworld.cpp and helloworld.hpp
      
## Compile

      enter the contract folder, compile cpp:
      python ../gentool.py wasm helloworld.cpp

      then helloworld.wasm and helloworld.wasm will be created.

      generate abi file:
      python ../gentool.py helloworld.hpp

      then helloworld.abi will be created.

      
ref to https://github.com/WebAssembly/wabt
