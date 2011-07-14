all: appdata_create appdata_load
appdata_create: appdata_create.c
	gcc -g `pkg-config --libs --cflags libxml-2.0 glib-2.0 gio-2.0 gio-unix-2.0 gnome-desktop-2.0` appdata_create.c -o appdata_create
appdata_load: appdata_load.c
	gcc -g `pkg-config --libs --cflags libxml-2.0 glib-2.0 gio-2.0 gio-unix-2.0 gnome-desktop-2.0` appdata_load.c -o appdata_load
clean:
	rm appdata_create appdata_load
