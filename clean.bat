@echo off
cls
for /R "build\bin" %%f in (*.*) do del "%%f"
for /R "build\obj" %%f in (*.*) do del "%%f"
rem rmdir /S /Q build
del *.win
del *.h
del *.rc
del *.layout