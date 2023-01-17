To set up the decoder pipeline:

1) Install dependencies:
* podman
* make
* GNU coreutils (the scripts aren't strictly POSIX compliant)

2) Add source files:
* `modisl1db/src/MODISL1DB_1.9_SPA_1.2.tar.gz`
* `mod14/src/MOD14_6.2.1_SPA_1.1.tar.gz`
* `bluemarble/src/BLUEMARBLE_2.2_SPA_2.2.tar.gz`

3) Add netrc file to `modisl1db/netrc`, according to [this format](https://wiki.earthdata.nasa.gov/display/EL/How+To+Access+Data+With+cURL+And+Wget)

4) Build the container images
```
$ make
$ podman images
REPOSITORY                TAG         IMAGE ID      CREATED             SIZE
localhost/bluemarble      latest      861f0a9bad73  11 seconds ago      2.38 GB
localhost/mod14           latest      1b63190befa8  About a minute ago  560 MB
localhost/modisl1db       latest      fb93d33c9906  2 minutes ago       12.6 GB
docker.io/library/centos  latest      5d0da3dc9764  13 months ago       239 MB
```

6) Add test input data
* `./data/input/leapsec.2022012900.dat`
* `./data/input/utcpole.2022012900.dat`
* `./data/input/MYD00F.A2015299.2110.20152992235.001.PDS`

5) Run some data processing
```
$ ./run_all.sh ./data
```
