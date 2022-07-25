@echo 
cd %~dp0
regsvr32 /u %~dp0LAV\LAVAudio.ax /s
regsvr32 /u %~dp0LAV\LAVSplitter.ax /s
regsvr32 /u %~dp0LAV\LAVVideo.ax /s
regsvr32 /u %~dp0LAV64\LAVAudio.ax /s
regsvr32 /u %~dp0LAV64\LAVSplitter.ax /s
regsvr32 /u %~dp0LAV64\LAVVideo.ax /s
@echo
@pause