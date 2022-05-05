all: modisl1db mod14 bluemarble
all-docker: modisl1db-docker mod14-docker bluemarble-docker

modisl1db: modisl1db/Dockerfile
	podman build ./modisl1db/ -t modisl1db

mod14: mod14/Dockerfile
	podman build ./mod14/ -t mod14

bluemarble: bluemarble/Dockerfile
	podman build ./bluemarble/ -t bluemarble

modisl1db-docker: modisl1db/Dockerfile
	docker build ./modisl1db/ -t modisl1db

mod14-docker: mod14/Dockerfile
	docker build ./mod14/ -t mod14

bluemarble-docker: bluemarble/Dockerfile
	docker build ./bluemarble/ -t bluemarble
