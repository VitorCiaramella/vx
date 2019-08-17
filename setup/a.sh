#!/usr/bin/env bash

# run with 
# sudo sh a.sh

# script that fixes weird macOS 10.14 SDK issues
# SO: https://stackoverflow.com/questions/51314888/ld-warning-text-based-stub-file-are-out-of-sync-falling-back-to-library-file#comment93288538_53111739
# script source: https://gist.github.com/wawiesel/eba461de5f5e38f7f0ac93ae3676b484
# slightly modified to include PrivateFrameworks too

F=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks
G=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/PrivateFrameworks

for d in $F/*.framework;
do
    [[ -d $d ]] || break
    sudo ln -s $d /Library/Frameworks/;
done

for d in $F/ApplicationServices.framework/Versions/A/Frameworks/*.framework;
do
    [[ -d $d ]] || break
    sudo ln -s $d /Library/Frameworks/;
done

for d in $F/CoreServices.framework/Versions/A/Frameworks/*.framework;
do
    [[ -d $d ]] || break
    sudo ln -s $d /Library/Frameworks/;
done


for d in $F/Carbon.framework/Versions/A/Frameworks/*.framework;
do
    [[ -d $d ]] || break
    sudo ln -s $d /Library/Frameworks/;
done

for d in $G/*.framework;
do
    [[ -d $d ]] || break
    sudo ln -s $d /Library/Frameworks/;
done




-cc1 
-triple 
x86_64-apple-macosx10.14.0 
-Wdeprecated-objc-isa-usage 
-Werror=deprecated-objc-isa-usage 
-emit-obj 
-mrelax-all 
-disable-free 
-disable-llvm-verifier 
-discard-value-names 
-main-file-name 
main.cpp 
-mrelocation-model pic 
-pic-level 2 
-mthread-model posix 
-mdisable-fp-elim 
-fno-strict-return 
-masm-verbose 
-munwind-tables 
-target-cpu penryn 
-dwarf-column-info 
-debug-info-kind=standalone 
-dwarf-version=4 
-debugger-tuning=lldb 
-target-linker-version 450.3 
-v 
-resource-dir /Library/Developer/CommandLineTools/usr/lib/clang/10.0.1 
-D _DEBUG 
-D ENABLE_PRECOMPILED_HEADERS=OFF 
-I /usr/local/Cellar/boost/1.70.0/include/boost/ 
-I /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/ 
-I /Users/vitorciaramella/Documents/GitHub/vx/extern/tiny-process-library 
-stdlib=libc++ 
-Wno-atomic-implicit-seq-cst 
-Wno-framework-include-private-from-public 
-Wno-atimport-in-framework-header 
-Wno-quoted-include-in-framework-header 
-std=c++17 
-fdeprecated-macro 
-fdebug-compilation-dir /Library/Developer/CommandLineTools/usr/bin 
-ferror-limit 19 
-fmessage-length 177 
-stack-protector 1 
-fblocks 
-fencode-extended-block-signature 
-fregister-global-dtors-with-atexit 
-fobjc-runtime=macosx-10.14.0 
-fcxx-exceptions 
-fexceptions 
-fmax-type-align=16 
-fdiagnostics-show-option 
-fcolor-diagnostics 
-o /var/folders/__/9hvxlz753rx0xphmy3xl32nc0000gn/T/main-04523a.o 
-x c++ 
/Users/vitorciaramella/Documents/GitHub/vx/src/vxBuild/main.cpp