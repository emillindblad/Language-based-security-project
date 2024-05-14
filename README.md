# Language-based-security-project
This was a project in the course __TDA602 - Language-based security__ at Chalmers University of Technology.

The repo contains the source code for the HTTP servers that were written as part of the project.

## Running the servers

### C server
`cd c-server`

Run the server with:

`make run`

Additional make commands are found in the makefile.

### Rust server

`cd rust-server`

Run the server with:

`cargo run`

## Benchmarking

The benchmarks were made using [Apache Bench](https://httpd.apache.org/docs/2.4/programs/ab.html):

`ab -n 10000 -c 500 http://localhost:3000/`
