forfiles /s /m *.glsl /c "cmd /c %VULKAN_SDK%/Bin/glslangValidator.exe @path -gVS -V -o @fname.spv"
pause