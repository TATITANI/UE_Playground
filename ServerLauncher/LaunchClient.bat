@echo off
cd /d "D:\UnrealProjects\Playground_DS_5.5\Binaries\Win64"
PlaygroundClient.exe -windowed -resx-800 -resy=450 > "%~dp0client_log.txt" 2>&1
pause
