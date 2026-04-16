pkill konsole
rm app

g++ -O2 -Oz -s -fdata-sections -ffunction-sections -Wl,--gc-sections tetrisV2.cpp -o app 

chmod +x app
konsole -e ./app

