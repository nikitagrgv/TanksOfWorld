@echo off
chcp 65001
setlocal EnableDelayedExpansion
set app=bin\TanksOfWorld_x64.exe
$(ADDITIONAL_PATH)
start ""  "%app%" 
