use std::{fs, io::{Read, Write}, net::{TcpListener, TcpStream}};

fn main() {
    let listener = TcpListener::bind("127.0.0.1:3000").unwrap();    // unwrap to not handle errors

    for stream in listener.incoming() {
        let stream = stream.unwrap();
        handle_connections(stream);
        println!("Connection established!");
    }
}

fn handle_connections(mut stream: TcpStream) {
    let mut buf = [0; 1024];
    stream.read(&mut buf).unwrap();

    let response = "HTTP/1.1 200 OK\r\n\r\n";
    let contents = fs::read_to_string("hello.html").unwrap();
    let length = contents.len();

    let response =
        format!("{response}\r\nContent-Length: {length}\r\n\r\n{contents}");
    stream.write(response.as_bytes()).unwrap();
    stream.flush().unwrap();

}