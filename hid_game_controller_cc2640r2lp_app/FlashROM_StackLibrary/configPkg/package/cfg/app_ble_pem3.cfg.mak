# invoke SourceDir generated makefile for app_ble.pem3
app_ble.pem3: .libraries,app_ble.pem3
.libraries,app_ble.pem3: package/cfg/app_ble_pem3.xdl
	$(MAKE) -f C:\ti\ccsv8workspace\CC2640R2F_LaunchpadV150\hid_game_controller_cc2640r2lp_app\TOOLS/src/makefile.libs

clean::
	$(MAKE) -f C:\ti\ccsv8workspace\CC2640R2F_LaunchpadV150\hid_game_controller_cc2640r2lp_app\TOOLS/src/makefile.libs clean

