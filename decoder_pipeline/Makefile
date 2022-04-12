all: modisl1db mod14 bluemarble

modisl1db: modisl1db/Dockerfile
	podman build ./modisl1db/ -t modisl1db

mod14: mod14/Dockerfile
	podman build ./mod14/ -t mod14

bluemarble: bluemarble/Dockerfile
	podman build ./bluemarble/ -t bluemarble
