@echo 
cd %~dp0
regsvr32 /u %~dp0x86\obs-virtualsource.dll /s
regsvr32 /u %~dp0x64\obs-virtualsource.dll /s
@echo
@pause