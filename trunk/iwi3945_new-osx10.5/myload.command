#/bin/sh

LOCPATH=`/usr/bin/dirname "$0"`
cd "$LOCPATH"
sudo kextunload build/Debug/iwi3945-loadable.kext
sudo rm -rf build/Debug/iwi3945-loadable.kext
sudo chown -R root:wheel build/Debug/iwi3945.kext
sudo chmod -R 755 build/Debug/iwi3945.kext
sudo mv build/Debug/iwi3945.kext build/Debug/iwi3945-loadable.kext
sudo sync
sudo kextload build/Debug/iwi3945-loadable.kext
