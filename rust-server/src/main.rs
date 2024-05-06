use std::{fs, io::{BufRead, BufReader, Write}, net::{TcpListener, TcpStream}, thread::sleep, time::Duration};
use threadpool::ThreadPool;

fn main() {
    let listener = match TcpListener::bind("127.0.0.1:3000") {
        Ok(listener) => listener,
        Err(e) => { 
            eprintln!("Error binding to address {}", e);
            return;
        }
    };

    let pool = ThreadPool::new(5);  
    
    for stream in listener.incoming() {
        let stream = match stream {
            Ok(tcp_stream) => tcp_stream, 
            Err(e) => {
                eprintln!("Error creating stream: {}", e);
                return;
            }
        };
        pool.execute(move || handle_connections(stream));
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
    
    let (status_line, filename) = match &request_line[..] {     // convert request_line to str slice to be able to compare
        "GET / HTTP/1.1" | "GET /index.html HTTP/1.0" => ("HTTP/1.1 200 OK", "index.html"),
        "GET /sleep.html HTTP/1.0" => {
            sleep(Duration::from_secs(5));
            ("HTTP/1.1 200 OK", "index.html")
        }
        _ => ("HTTP/1.1 404 Not Found", "error.html")
    };
    
    let contents = match fs::read_to_string(filename) {
        Ok(line) => line,
        Err(e) => {
            eprintln!("Error reading line: {}", e);
            return;
        }
    };
        
    let length = contents.len();

    let response =
        format!("{status_line}\r\nContent-Type: text/html\r\nContent-Length: {}\r\n\r\n{}", length, contents);

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
}