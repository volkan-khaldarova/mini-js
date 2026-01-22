@echo off
echo [AURA] Proje Derleniyor...

gcc -Wall -Wextra -std=c99 -Iinclude -Isrc/scanner -Isrc/value -o aura.exe src/main.c src/scanner/scanner.c src/value/value.c

if %errorlevel% neq 0 (
    echo [HATA] Derleme basarisiz oldu!
    pause
    exit /b %errorlevel%
)

echo [BASARILI] aura.exe olusturuldu.