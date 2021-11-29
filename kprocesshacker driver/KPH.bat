@echo off
net stop KPH
sc delete KPH
sc create KPH binpath="C:\Users\LEGION\Desktop\kph.sys" start=demand type=filesys
net start KPH
pause