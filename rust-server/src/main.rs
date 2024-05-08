use std::{
    fs,
    io::{BufRead, BufReader, Write},
    net::{TcpListener, TcpStream},
    thread,
    thread::sleep,
    time::Duration,
};

fn main() {
    let port = "3000";
    let listener = match TcpListener::bind(format!("0.0.0.0:{port}")) {
        Ok(listener) => listener,
        Err(e) => {
            panic!("Error binding to address, {e}");
        }
    };
    println!("Sever running on {port}");

    //let pool = ThreadPool::new(5);

    for stream in listener.incoming() {
        let stream = match stream {
            Ok(tcp_stream) => tcp_stream,
            Err(_) => {
                panic!("Error creating stream");
            }
        };
        thread::spawn(move || handle_connections(stream));
    }
}

fn handle_connections(mut stream: TcpStream) {
    let buf_reader = BufReader::new(&mut stream);
    let request_line = match buf_reader.lines().next() {
        Some(Ok(line)) => line,
        Some(Err(e)) => {
            eprintln!("Error reading line: {}", e);
            return;
        }
        None => {
            eprintln!("No lines in request buffer");
            return;
        }
    };

    let mut splited = request_line.split(' ');

    let method = match splited.next() {
        Some(line) => line,
        None => {
            eprintln!("Error with splitting line");
            return;
        }
    };

    let path = match splited.next() {
        Some(line) => line,
        None => {
            eprintln!("Error with splitting line");
            return;
        }
    };

    // println!("{method} {path}");

    let (status_line, filename) = match (method, path) {
        // convert request_line to str slice to be able to compare
        ("GET", "/") => ("HTTP/1.1 200 OK", "index.html"),
        ("GET", "/index.html") => ("HTTP/1.1 200 OK", "index.html"),
        ("GET", "/sleep") => {
            sleep(Duration::from_secs(5));
            ("HTTP/1.1 200 OK", "index.html")
        }
        _ => ("HTTP/1.1 404 Not Found", "error.html"),
    };

    let contents = match fs::read_to_string(format!("../html/{filename}")) {
        Ok(line) => line,
        Err(e) => {
            eprintln!("Error reading line: {}", e);
            return;
        }
    };

    let response = format!(
        "{status_line}\r\nContent-Type: text/html\r\nContent-Length: {}\r\n\r\n{}",
        contents.len(),
        contents
    );

    match stream.write_all(response.as_bytes()) {
        Ok(bytes) => bytes,
        Err(e) => {
            eprintln!("Error writing response: {}", e);
            return;
        }
    };
}
