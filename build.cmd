@echo off
del cpsw.exe
cl /Ox2 /GL /Wall /Wp64 /GS- /GF cpsw.c /link /release /subsystem:windows /opt:ref /opt:nowin98 /out:cpsw.exe
del *.obj