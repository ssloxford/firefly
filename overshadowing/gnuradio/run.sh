docker run -dit --rm --privileged -p 2225:22 --env-file variables.env -v $(pwd)/scripts:/data/scripts -v $(pwd)/samples:/data/samples --name firefly-gr-satnogs firefly-gr-satnogs
docker exec firefly-gr-satnogs service ssh restart
