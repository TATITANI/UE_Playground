@echo off
cd /d "D:\UnrealProjects\UE_Playground_5.4\Binaries\Win64"
PlaygroundServer.exe -log > "%~dp0server_log.txt" 2>&1
pause
