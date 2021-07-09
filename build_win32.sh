if [ ! -e ./bin/win32 ]; then
    mkdir -p ./bin/win32
fi
i686-w64-mingw32-gcc -w ./src/cert_helper.c -lcrypt32 -shared -s -o ./bin/win32/cert_helper.dll
i686-w64-mingw32-gcc -w ./src/cert_helper.c -DTEST_BIN -lcrypt32 -s -o ./bin/win32/cert_helper.exe