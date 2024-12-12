@echo off
cd /d "D:\UnrealProjects\Playground_DS_5.5\Binaries\Win64"
PlaygroundServer.exe -log -LogCmds="LogNet Verbose, LogBlueprintUserMessages LogTemp"> "%~dp0server_log.txt" 2>&1
pause
