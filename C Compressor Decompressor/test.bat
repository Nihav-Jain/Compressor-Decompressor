@echo off
FOR /L %%i IN (5,1,16) DO (Compressor.exe %%i "verts.txt" & Decompressor.exe "verts_compressed.bin" "verts_orig_data.txt" & automate516.bat "verts_compressed.bin")