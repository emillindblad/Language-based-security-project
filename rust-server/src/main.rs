use std::{fs, io::{BufRead, BufReader, Write}, net::{TcpListener, TcpStream}};

fn main() {
    let listener = match TcpListener::bind("127.0.0.1:3000") {
        Ok(listener) => listener,
        Err(e) => { 
            eprintln!("Error binding to address {}", e);
            return;
        }
    };

    for stream in listener.incoming() {
        let stream = match stream {
            Ok(tcp_stream) => tcp_stream, 
            Err(e) => {
                eprintln!("Error creating stream: {}", e);
                return;
            }
        };
        handle_connections(stream);
        println!("Connection established!");
    }
}

fn handle_connections(mut stream: TcpStream) {
    let buf_reader = BufReader::new(&mut stream);
    let request_line = match buf_reader.lines().next() {
        Some(Ok(line)) => line,
        Some(Err(e)) => {
            eprintln!("Error reading line: {}", e);
            return; 
        },
        None => {
            eprintln!("No lines in buffer");
            return;
        }
    };
    
    if request_line == "GET / HTTP/1.1" {
        let response: &str = "HTTP/1.1 200 OK";
        let contents = match fs::read_to_string("index.html") {
            Ok(line) => line,
            Err(e) => {
                eprintln!("Error reading line: {}", e);
                return;
            }
        };
            
        let length = contents.len();

        let response =
            format!("{response}\r\nContent-Type: text/html\r\nContent-Length: {}\r\n\r\n{}", length, contents);

        match stream.write(response.as_bytes()) {
            Ok(bytes) => bytes,
            Err(e) => {
                eprintln!("Error writing bytes: {}", e);
                return;
            }
        };
        match stream.flush() {
            Ok(x) => x,
            Err(e) => {
                eprintln!("Error flushing stream: {}", e);
                return;
            }
        };
    } else {
        // other request 
    }
    

}