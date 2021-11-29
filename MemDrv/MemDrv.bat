@echo off
net stop MemDrv
sc delete MemDrv
sc create MemDrv binpath="C:\Users\LEGION\Desktop\MemDrv.sys" start=demand type=filesys
net start MemDrv
pause