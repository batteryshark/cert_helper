if [ ! -e ./bin/win64 ]; then
    mkdir -p ./bin/win64
fi
x86_64-w64-mingw32-gcc -w ./src/cert_helper.c -lcrypt32 -shared -s -o ./bin/win64/cert_helper.dll
x86_64-w64-mingw32-gcc -w ./src/cert_helper.c -DTEST_BIN -lcrypt32 -s -o ./bin/win64/cert_helper.exe