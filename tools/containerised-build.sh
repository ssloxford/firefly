#!/bin/sh
set -e

DATA_PATH="$(realpath .)"

# TODO: test if docker image already installed

if ! type podman > /dev/null; then
	CONTAINER_RUNTIME=docker
else
	CONTAINER_RUNTIME=podman
fi

$CONTAINER_RUNTIME build . -t firefly-tools-build
$CONTAINER_RUNTIME run -v "$DATA_PATH":/root/src --rm -it firefly-tools-build
