@echo off
chcp 65001
setlocal EnableDelayedExpansion
set app=bin\TanksOfWorld_x64d.exe
$(ADDITIONAL_PATH)
start ""  "%app%" 
